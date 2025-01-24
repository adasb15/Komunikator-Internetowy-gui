#include "server.h"

int findOrCreateConversation(int senderId, const std::vector<std::string>& participantUsernames, const std::string& groupName = "") {
    std::lock_guard<std::mutex> lock(usersMutex); // Zabezpieczamy dostęp do mapy użytkowników

    // Sprawdzamy, czy nadawca istnieje
    if (users.find(senderId) == users.end()) {
        std::cerr << "Nie znaleziono nadawcy o ID: " << senderId << std::endl;
        return -1;  // Błąd, brak nadawcy
    }

    auto& sender = users[senderId];
    std::vector<std::string> allParticipants = participantUsernames;

    // Dodajemy nadawcę do listy uczestników, jeśli go tam nie ma
    if (std::find(allParticipants.begin(), allParticipants.end(), sender.username) == allParticipants.end()) {
        allParticipants.push_back(sender.username);
    }

    if (!groupName.empty()) {
        for (const auto& conv : conversations) {
            if (strncmp(conv.group_name, groupName.c_str(), sizeof(conv.group_name)) == 0) {
                for (const auto& part : conv.participants) {
                    if (sender.username == part) {
                        return conv.conversationId;
                    }
                }
            }
        }
        return -1;
    }

    // Sprawdzamy, czy wszyscy uczestnicy istnieją w systemie
    for (const auto& username : allParticipants) {
        bool userExists = false;
        for (const auto& pair : users) {
            if (pair.second.username == username) {
                userExists = true;
                break;
            }
        }

        if (!userExists) {
            std::cerr << "Nie znaleziono użytkownika o nazwie: " << username << std::endl;
            return -1; // Błąd, użytkownik nie istnieje
        }
    }

    // Sprawdzamy, czy konwersacja już istnieje na podstawie uczestników i nazwy grupy
    for (const auto& conv : conversations) {
        // Sprawdzamy nazwę grupy (dla konwersacji grupowych)
        if (!groupName.empty() && strncmp(conv.group_name, groupName.c_str(), sizeof(conv.group_name)) == 0) {
            std::cout << "Konwersacja o nazwie grupy '" << groupName << "' już istnieje." << std::endl;
            return conv.conversationId;
        }

        // Sprawdzamy, czy uczestnicy są tacy sami
        if (conv.participantsCount == allParticipants.size()) {
            bool allMatch = true;

            for (const auto& username : allParticipants) {
                bool found = false;
                for (int i = 0; i < conv.participantsCount; ++i) {
                    if (strncmp(conv.participants[i], username.c_str(), 256) == 0) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    allMatch = false;
                    break;
                }
            }

            if (allMatch) {
                return conv.conversationId; // Konwersacja już istnieje
            }
        }
    }

    // Tworzymy nową konwersację

    // Sprawdzamy, czy wszyscy uczestnicy są swoimi znajomymi
    for (size_t i = 0; i < allParticipants.size(); ++i) {
        for (size_t j = i + 1; j < allParticipants.size(); ++j) {
            const auto& user1 = allParticipants[i];
            const auto& user2 = allParticipants[j];

            // Znajdujemy użytkownika w mapie users
            auto it1 = std::find_if(users.begin(), users.end(),
                                    [&user1](const auto& pair) {
                                        return pair.second.username == user1;
                                    });
            auto it2 = std::find_if(users.begin(), users.end(),
                                    [&user2](const auto& pair) {
                                        return pair.second.username == user2;
                                    });

            // Jeśli któregokolwiek użytkownika nie ma, zwracamy błąd
            if (it1 == users.end() || it2 == users.end()) {
                std::cerr << "Nie znaleziono użytkownika: " 
                        << (it1 == users.end() ? user1 : user2) << std::endl;
                return -1;
            }

            const auto& user1Data = it1->second;
            const auto& user2Data = it2->second;

            // Sprawdzamy, czy user1 jest znajomym user2 i odwrotnie
            if (std::find(user1Data.friends.begin(), user1Data.friends.end(), user2) == user1Data.friends.end() ||
                std::find(user2Data.friends.begin(), user2Data.friends.end(), user1) == user2Data.friends.end()) {
                std::cerr << "Użytkownicy \"" << user1 << "\" i \"" << user2 
                        << "\" nie są znajomymi!" << std::endl;
                return -1;
            }
        }
    }

    Conversation newConversation;
    newConversation.conversationId = nextConversationId;
    nextConversationId =  nextConversationId + 100;
    newConversation.participantsCount = allParticipants.size();

    for (size_t i = 0; i < allParticipants.size(); ++i) {
        strncpy(newConversation.participants[i], allParticipants[i].c_str(), sizeof(newConversation.participants[i]) - 1);
        newConversation.participants[i][sizeof(newConversation.participants[i]) - 1] = '\0'; // Null-terminate
    }

    // Ustawiamy nazwę grupy tylko dla grup (3 lub więcej osób)
    if (allParticipants.size() >= 3 && !groupName.empty()) {
        strncpy(newConversation.group_name, groupName.c_str(), sizeof(newConversation.group_name) - 1);
        newConversation.group_name[sizeof(newConversation.group_name) - 1] = '\0'; // Null-terminate
    } else {
        newConversation.group_name[0] = '\0'; // Brak nazwy grupy dla konwersacji 1:1
    }

    newConversation.messages = {}; // Pusta historia wiadomości
    conversations.push_back(newConversation); // Dodajemy nową konwersację
    std::cout << "Utworzono nową konwersację.\n";
    
    return newConversation.conversationId; // Zwracamy ID nowej konwersacji
}

void handleSendPrivateMessage(int clientSocket, struct FriendAction request) {
    // Sprawdź, czy istnieje już konwersacja lub utwórz nową
    int conversationId = findOrCreateConversation(request.userId, {request.friendUsername});

    // Wyślij ID konwersacji do klienta
    send(clientSocket, &conversationId, sizeof(conversationId), 0);

    if (conversationId == -1) { // Nie udało się znaleźć lub utworzyć konwersacji
        return;
    }
    
    for (const auto& conv : conversations) {
        if (conv.conversationId == conversationId) {
            int messageCount = conv.messages.size();
            
            // Wyślij liczbę wiadomości w historii
            send(clientSocket, &messageCount, sizeof(messageCount), 0);

            sleep(0.5);

            if (messageCount > 0) {
                // Oblicz rozmiar bufora
                size_t bufferSize = messageCount * sizeof(Message);

                // Pobierz wskaźnik do danych wektora (unikamy niepotrzebnego kopiowania)

                for (const auto& msg : conv.messages) {
                    std::cout << msg.senderUsername << " " << msg.content << std::endl;
                }

                const char* buffer = reinterpret_cast<const char*>(conv.messages.data());

                // Wyślij cały bufor w jednej operacji
                ssize_t sentBytes = send(clientSocket, buffer, bufferSize, 0);
                if (sentBytes == -1) {
                    perror("Błąd wysyłania wiadomości");
                    break;
                }

                std::cout << "Wysłano " << messageCount << " wiadomości (rozmiar: " << bufferSize << " bajtów)" << std::endl;
            }
        }
    }
}

void handleSendGroupMessage(int clientSocket, int userId, const std::string& groupName) {
    // Sprawdź, czy istnieje już konwersacja lub utwórz nową
    int conversationId = findOrCreateConversation(userId, {""}, groupName);

    // Wyślij ID konwersacji do klienta
    send(clientSocket, &conversationId, sizeof(conversationId), 0);

    if (conversationId == -1) { // Nie udało się znaleźć lub utworzyć konwersacji
        return;
    }

    for (const auto& conv : conversations) {
        if (conv.conversationId == conversationId) {
            int messageCount = conv.messages.size();
            
            // Wyślij liczbę wiadomości w historii
            send(clientSocket, &messageCount, sizeof(messageCount), 0);

            sleep(0.5);

            if (messageCount > 0) {
                // Oblicz rozmiar bufora
                size_t bufferSize = messageCount * sizeof(Message);

                // Pobierz wskaźnik do danych wektora (unikamy niepotrzebnego kopiowania)

                for (const auto& msg : conv.messages) {
                    std::cout << msg.senderUsername << " " << msg.content << std::endl;
                }

                const char* buffer = reinterpret_cast<const char*>(conv.messages.data());

                // Wyślij cały bufor w jednej operacji
                ssize_t sentBytes = send(clientSocket, buffer, bufferSize, 0);
                if (sentBytes == -1) {
                    perror("Błąd wysyłania wiadomości");
                    break;
                }

                std::cout << "Wysłano " << messageCount << " wiadomości (rozmiar: " << bufferSize << " bajtów)" << std::endl;
            }
        }
    }
}