#include "server.h"
#include "server_file.cpp"
#include "server_findconv.cpp"

void handleRegistration(int clientSocket) {
    // Struktury do przechowywania danych nowego użytkownika i rejestracji
    User newUser;
    Register newregister;

    // Odbieranie danych od klienta
    recv(clientSocket, &newregister, sizeof newregister, 0);

    // Sprawdzenie, czy użytkownik już istnieje
    bool userExists = false;
    {
        // Bezpieczny dostęp do mapy użytkowników (mutex chroni przed wielowątkowością)
        std::lock_guard<std::mutex> lock(usersMutex);
        for (const auto& pair : users) {
            if (pair.second.username == newregister.username) {
                userExists = true; // Znaleziono użytkownika o podanej nazwie
                break;
            }
        }
    }

    // Jeśli użytkownik istnieje, wyślij informację zwrotną do klienta
    if (userExists) {
        char msg_buff[30] = "Uzytkownik juz istnieje";
        send(clientSocket, msg_buff, strlen(msg_buff) + 1, 0);
        return;
    }

    // Inicjalizacja nowego użytkownika
    newUser.id = nextUserId++; // Unikalne ID dla nowego użytkownika
    newUser.username = newregister.username;
    newUser.password = newregister.password;

    // Dodanie użytkownika do mapy w sposób bezpieczny dla wątków
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        users[newUser.id] = newUser;
    }

    // Zapis nowego użytkownika do pliku (utrwalenie danych)
    saveUsersToFile();

    // Logowanie na serwerze o nowej rejestracji
    std::cout << "Zarejestrowano użytkownika o id: " << newUser.id 
              << " o nazwie " << newUser.username 
              << " hasło: " << newUser.password << std::endl;

    // Informacja o sukcesie do klienta
    char msg_buff[18] = "Rejestracja udana";
    send(clientSocket, msg_buff, sizeof(msg_buff), 0);
}

void handleLogin(int clientSocket) {
    Register loginData; // Przechowuje dane logowania

    // Odbieranie danych logowania od klienta
    int bytesRead = recv(clientSocket, &loginData, sizeof loginData, 0);
    if (bytesRead <= 0) {
        std::cerr << "Błąd podczas odbierania danych logowania.\n";
        return;
    }

    // Przekształcenie na ciągi znaków
    std::string username = loginData.username;
    std::string password = loginData.password;

    bool loginSuccess = false;
    int loggedInUserId = -1;

    // Sprawdzenie poprawności danych logowania
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        for (const auto& pair : users) {
            const User& user = pair.second;
            if (user.username == username && user.password == password) {
                loginSuccess = true;
                loggedInUserId = user.id;
                break;
            }
        }
    }

    // Odpowiedź do klienta w zależności od powodzenia logowania
    if (loginSuccess) {
        std::cout << "Użytkownik się zalogował\n";
        if (send(clientSocket, &loggedInUserId, sizeof(loggedInUserId), 0) < 0) {
            std::cerr << "Błąd wysłania ID użytkownika.\n";
        }
        handleUserMenu(clientSocket); // Przekierowanie do menu użytkownika
    } else {
        // Nieudane logowanie
        if (send(clientSocket, &loggedInUserId, sizeof(loggedInUserId), 0) < 0) {
            std::cerr << "Błąd wysłania informacji o niepowodzeniu logowania.\n";
        }
    }
}

void handleAddFriend(int clientSocket, struct FriendAction request) {
    int loggedInUserId = request.userId;  // ID użytkownika, który chce dodać znajomego

    std::lock_guard<std::mutex> lock(usersMutex);  // Blokada mutexa, aby zapewnić bezpieczeństwo wątkowe

    // Sprawdzamy, czy użytkownik, który chce dodać znajomego, istnieje
    if (users.find(loggedInUserId) == users.end()) {
        std::cout << "Nie znaleziono użytkownika o ID: " << loggedInUserId << "\n";
        return;  // Zakończenie funkcji, jeśli użytkownik nie istnieje
    }

    // Sprawdzamy, czy użytkownik, któremu chcemy dodać znajomego, istnieje
    bool friendExists = false;
    for (auto& pair : users) {
        if (pair.second.username == request.friendUsername) {
            friendExists = true;
            break;  // Znaleziono użytkownika, przerywamy pętlę
        }
    }

    if (!friendExists) {
        char errorMsg[50] = "Nie znaleziono użytkownika o podanej nazwie.";
        std::cout << errorMsg << std::endl;
        // Wysłanie odpowiedzi do klienta o niepowodzeniu
        send(clientSocket, errorMsg, strlen(errorMsg) + 1, 0);
        return;
    }

    auto& loggedInUser = users[loggedInUserId];  // Referencja do zalogowanego użytkownika

    // Sprawdzamy, czy użytkownik próbuje dodać samego siebie
    if (loggedInUser.username == request.friendUsername) {
        char errorMsg[50] = "Nie możesz dodać samego siebie do znajomych.";
        // Wysłanie odpowiedzi do klienta
        send(clientSocket, errorMsg, strlen(errorMsg) + 1, 0);
        return;
    }

    // Sprawdzamy, czy użytkownik już ma tego znajomego na liście
    auto it = std::find(loggedInUser.friends.begin(), loggedInUser.friends.end(), request.friendUsername);

    if (it != loggedInUser.friends.end()) {
        // Jeśli znajomy już istnieje na liście
        char errorMsg[50] = "Ten użytkownik jest już Twoim znajomym.";
        // Wysłanie odpowiedzi do klienta
        send(clientSocket, errorMsg, strlen(errorMsg) + 1, 0);
        return;
    }

    // Dodanie znajomego do listy znajomych użytkownika
    loggedInUser.friends.push_back(request.friendUsername);

    updateUserInFile(loggedInUser);  // Aktualizacja danych użytkownika w pliku

    std::cout << "Dodano znajomego: " << request.friendUsername << " dla użytkownika: " << loggedInUser.username << std::endl;

    // Wysłanie odpowiedzi do klienta
    char successMsg[50] = "Dodanie znajomego udane!";
    send(clientSocket, successMsg, strlen(successMsg) + 1, 0);
}

void handleRemoveFriend(int clientSocket, struct FriendAction request) {
    int loggedInUserId = request.userId;  
    std::string friendUsername = request.friendUsername; // Nazwa użytkownika znajomego, którego chcemy usunąć
    std::lock_guard<std::mutex> lock(usersMutex); // Zabezpieczamy dostęp do danych użytkowników przy pomocy mutexa

    // Sprawdzamy, czy użytkownik, który chce usunąć znajomego, istnieje w systemie
    if (users.find(loggedInUserId) == users.end()) {
        // Jeśli użytkownik nie istnieje, wypisujemy komunikat o błędzie i kończymy funkcję
        std::cout << "Nie znaleziono użytkownika o ID: " << loggedInUserId << "\n";
        return;
    }

    // Pobieramy dane użytkownika, który chce usunąć znajomego
    auto& loggedInUser = users[loggedInUserId];

    // Sprawdzamy, czy użytkownik ma znajomego o podanej nazwie na swojej liście znajomych
    auto it = std::find(loggedInUser.friends.begin(), loggedInUser.friends.end(), friendUsername);

    // Jeśli znajomego nie znaleziono
    if (it == loggedInUser.friends.end()) {
        // Tworzymy komunikat o błędzie, informujący, że znajomy nie istnieje w liście znajomych
        char errorMsg[60] = "Znajomy o podanej nazwie nie istnieje w twoich znajomych.";
        // Wysyłamy komunikat o błędzie do klienta
        send(clientSocket, errorMsg, strlen(errorMsg) + 1, 0);
        return;
    }

    // Usuwamy znajomego z listy znajomych użytkownika
    loggedInUser.friends.erase(it);

    // Aktualizujemy dane użytkownika w pliku, jeśli to wymagane (np. w bazie danych lub pliku)
    updateUserInFile(loggedInUser);

    // Informujemy, że operacja usunięcia zakończyła się sukcesem
    std::cout << "Usunięto znajomego: " << friendUsername << " dla użytkownika: " << loggedInUser.username << std::endl;

    // Wysyłamy komunikat o sukcesie do klienta
    char successMsg[50] = "Usunięcie znajomego udane!";
    send(clientSocket, successMsg, strlen(successMsg) + 1, 0);
}

void handleViewFriends(int clientSocket, struct FriendAction request) {
    int loggedInUserId = request.userId;  // ID użytkownika, który chce zobaczyć znajomych

    // Zabezpieczamy dostęp do danych użytkowników
    std::lock_guard<std::mutex> lock(usersMutex);

    // Sprawdzamy, czy użytkownik istnieje
    if (users.find(loggedInUserId) == users.end()) {
        std::cerr << "Nie znaleziono użytkownika o ID: " << loggedInUserId << "\n";
        return;
    }

    // Pobieramy dane użytkownika
    const auto& loggedInUser = users[loggedInUserId];

    // Przygotowanie listy znajomych do wysłania
    std::string friendsList = "Znajomi użytkownika " + loggedInUser.username + ":\n";
    if (loggedInUser.friends.empty()) {
        friendsList += "Brak znajomych.\n";
    } else {
        for (const auto& friendUsername : loggedInUser.friends) {
            friendsList += "- " + friendUsername + "\n";  // Dodajemy znajomych do listy
        }
    }

    // Wysyłamy listę znajomych do klienta
    send(clientSocket, friendsList.c_str(), friendsList.length() + 1, 0);
}

void handleSendMessage(int conversationId, struct Message request) {
    // Znajdź konwersację i dodaj wiadomość
    for (auto& conv : conversations) {
        if (conv.conversationId == conversationId) {
            conv.messages.push_back(request);  // Dodaj wiadomość jako obiekt Message
            // Zapisz tylko zmodyfikowaną konwersację do pliku
            updateConversationInFile(conv);
            std::cout << "Wiadomość została zapisana w konwersacji.\n";
            int userId = 0;
            int i = 0;
            for (auto& part : conv.participants) {
                i++;
                if (i > conv.participantsCount) {
                    break;
                }
                struct sockaddr_in sa;
                int clientConv = socket(PF_INET, SOCK_STREAM, 0);

                if (clientConv < 0) {
                    std::cerr << "Błąd podczas tworzenia gniazda.\n";
                    return;
                }

                for (auto& pair : users) {
                    if (part == pair.second.username) {
                        userId = pair.first;
                    }
                }

                sa.sin_family = AF_INET;
                sa.sin_port = htons(PORT + conversationId + userId);

                std::cout << PORT + conversationId + userId << std::endl;

                if (inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr) <= 0) {
                    std::cerr << "Nieprawidłowy adres IP.\n";
                    return;
                }

                sleep(0.5);

                if (connect(clientConv, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
                    std::cerr << "Błąd połączenia z serwerem.\n";
                    continue;
                }

                Message new_msg;
                memset(&new_msg, 0, sizeof(sa));
                new_msg = conv.messages.back();   // Pobranie ostatniej wiadomości

                send(clientConv, &new_msg, sizeof(new_msg), 0);
                close(clientConv);
            }
            break;
        }
    } 
}

void handleCreateGroup(int clientSocket, const std::vector<std::string>& friendsNames, const char* group_name) {
    std::lock_guard<std::mutex> lock(usersMutex); // Zabezpieczamy dostęp do mapy użytkowników

    std::cout << "Rozpoczynamy tworzenie grupy o nazwie: \"" << group_name << "\"..." << std::endl;

    // Sprawdzamy, czy grupa o podanej nazwie już istnieje
    for (const auto& conv : conversations) {
        if (strncmp(conv.group_name, group_name, sizeof(conv.group_name)) == 0) {
            std::cout << "Grupa o nazwie \"" << group_name << "\" już istnieje!" << std::endl;
            char msg[256] = "Grupa o podanej nazwie już istnieje!\n";
            if (send(clientSocket, &msg, sizeof(msg), 0) < 0) {
                std::cerr << "Błąd podczas wysyłania odpowiedzi.\n";
                return;
            }
            return;
        }
    }

    // Sprawdzamy, czy wszyscy uczestnicy są swoimi znajomymi
    for (size_t i = 0; i < friendsNames.size(); ++i) {
        for (size_t j = i + 1; j < friendsNames.size(); ++j) {
            const auto& user1 = friendsNames[i];
            const auto& user2 = friendsNames[j];

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
                std::cout << "Nie znaleziono użytkownika: " << (it1 == users.end() ? user1 : user2) << std::endl;

                char response[50] = "Nie udało się utworzyć grupy.\n";
                if (send(clientSocket, response, sizeof(response), 0) < 0) {
                    std::cerr << "Bład podczas wysyłania odpowiedzi do klienta.\n";
                }
                return;

                return;
            }

            const auto& user1Data = it1->second;
            const auto& user2Data = it2->second;

            // Sprawdzamy, czy user1 jest znajomym user2 i odwrotnie
            if (std::find(user1Data.friends.begin(), user1Data.friends.end(), user2) == user1Data.friends.end() ||
                std::find(user2Data.friends.begin(), user2Data.friends.end(), user1) == user2Data.friends.end()) {
                std::cout << "Użytkownicy \"" << user1 << "\" i \"" << user2 << "\" nie są znajomymi!" << std::endl;

                char response[50] = "Nie udało się utworzyć grupy.\n";
                if (send(clientSocket, response, sizeof(response), 0) < 0) {
                    std::cerr << "Bład podczas wysyłania odpowiedzi do klienta.\n";
                }
                return;
            }
        }
    }

    // Tworzymy nową konwersację/grupę
    Conversation newConversation;
    newConversation.conversationId = nextConversationId; // Generujemy nowe ID konwersacji
    nextConversationId = nextConversationId + 100;

    std::cout << "Tworzymy nową konwersację z ID: " << newConversation.conversationId << std::endl;

    // Przypisujemy nazwę grupy
    strncpy(newConversation.group_name, group_name, sizeof(newConversation.group_name) - 1);
    newConversation.group_name[sizeof(newConversation.group_name) - 1] = '\0'; // Null-terminate

    // Dodajemy uczestników do grupy
    int participantsCount = 0;
    for (const auto& participant : friendsNames) {        
        // Zwracamy, jeśli nie ma uczestników
        if (participantsCount >= 20) break;
        
        strncpy(newConversation.participants[participantsCount], participant.c_str(), sizeof(newConversation.participants[participantsCount]) - 1);
        newConversation.participants[participantsCount][sizeof(newConversation.participants[participantsCount]) - 1] = '\0'; // Null-terminate
        participantsCount++;
    }

    newConversation.participantsCount = participantsCount; // Ustawiamy liczbę uczestników

    if (participantsCount < 3) {
        char msg[256] = "Grupa musi mieć co najmniej trzech uczestników!\n";
        if (send(clientSocket, &msg, sizeof(msg), 0) < 0) {
            std::cerr << "Błąd podczas wysyłania odpowiedzi.\n";
            return;
        }
        return; // Grupa musi mieć co najmniej 3 osoby
    }

    // Dodajemy wiadomości (pusta historia na początek)
    newConversation.messages = {}; 
    std::cout << "Historia wiadomości została zainicjowana." << std::endl;

    // Dodajemy nową konwersację (grupę) do systemu
    conversations.push_back(newConversation);
    std::cout << "Grupa została dodana do systemu." << std::endl;
    
    // Aktualizujemy konwersację w pliku
    updateConversationInFile(newConversation);
    std::cout << "Grupa została zaktualizowana w pliku." << std::endl;

    // Wysyłamy komunikat o sukcesie
    char msg[256] = "Utworzenie grupy przebiegło pomyślnie.\n";
    if (send(clientSocket, &msg, sizeof(msg), 0) < 0) {
        std::cerr << "Błąd podczas wysyłania odpowiedzi.\n";
        return;
    }

    std::cout << "Grupa o nazwie \"" << group_name << "\" została pomyślnie utworzona!" << std::endl;
}

void handleUserMenu(int clientSocket) {
    while (true) {
        struct Msg_to_server request;

        if (recv(clientSocket, &request, sizeof request, 0) <= 0) {
            close(clientSocket);
            return;
        }

        std::vector<std::string> friendsNamesVec;

        switch (request.option) {
            case -1:
                std::cout << "Użytkownik się wylogował\n";
                return;
            case 1: //Obsługa dodania znajomego
                std::cout << "Obsługuje dodanie znajomego\n";
                handleAddFriend(clientSocket, request.friend_action);
                break;
            case 2: 
                std::cout << "Obsługuje usunięcie znajomego\n";
                handleRemoveFriend(clientSocket, request.friend_action);
                break;
            case 3:
                std::cout << "Obsługuje wyświetlenia znajomych\n";
                handleViewFriends(clientSocket, request.friend_action);
                break;
            case 4:
                std::cout << "Obsługuje wysłania wiadomości prywatnej\n";
                handleSendPrivateMessage(clientSocket, request.friend_action);
                break;
            case 5:
                std::cout << "Obsługuje wysłania wiadomości\n";
                handleSendMessage(request.conversationId, request.message);
                break;
            case 7:
                std::cout << "Obsługuje utworzenie grupy.\n";
                friendsNamesVec.clear();
                for (int i = 0; i < 20 && request.friendsNames[i][0] != '\0'; i++) {
                    friendsNamesVec.push_back(request.friendsNames[i]);
                }
                handleCreateGroup(clientSocket, friendsNamesVec, request.group_name);
                break;
            case 8:
                std::cout <<"Obsługa wysłania wiadomości grupowej.\n";
                handleSendGroupMessage(clientSocket, request.friend_action.userId, request.group_name);
                break;
            default:
                continue;
        }
    }
}

void handleClient(int clientSocket) {
    while (true) {
        int option;
        if (recv(clientSocket, &option, sizeof option, 0) <= 0) {
            close(clientSocket);
            return;
        }

        switch (option) {
        case -1:
            activeClient--;
            std::cout << "Liczba aktywnych klientów: " << activeClient << std::endl;
            close(clientSocket);
            return;
        case 1:
            handleRegistration(clientSocket);
            break;
        case 2:
            handleLogin(clientSocket);
            break;
        default:
            continue;
        }
    }
}

// Funkcja do obsługi sygnałów
void handleSigInt(int signum) {
    std::cout << "\nWyłączanie serwera...\n";
    sleep(0.5);
    close(serverSocket);  // Zamknięcie gniazda serwera
    std::exit(0);  // Zakończenie programu
}

int main() {
    loadUsersFromFile();
    loadConversationsFromFile();

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Błąd tworzenia gniazda.\n";
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Błąd bindowania gniazda.\n";
        return 1;
    }

    listen(serverSocket, 10);
    std::cout << "Serwer nasłuchuje na porcie " << PORT << ".\n";

        // Rejestracja funkcji obsługi sygnału SIGINT (Ctrl+C)
    signal(SIGINT, handleSigInt);

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Błąd akceptacji połączenia.\n";
            continue;
        }

        std::cout << "Połączono z klientem.\n";
        activeClient++;
        std::cout << "Liczba aktywnych klientów: " << activeClient << std::endl;
        // Obsługa klienta w oddzielnym wątku
        std::thread(handleClient, clientSocket).detach();
    }

    close(serverSocket);
}