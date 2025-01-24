#include "server.h"

using json = nlohmann::json;

// Funkcja wczytująca użytkowników z pliku JSON
void loadUsersFromFile() {
    std::ifstream inFile("resources/users.json");
    if (inFile.is_open()) {
        json j;
        inFile >> j;

        // Blokujemy mutex na czas modyfikacji zasobów współdzielonych
        std::lock_guard<std::mutex> lock(usersMutex);

        for (const auto& userJson : j) {
            User user;
            user.id = userJson["id"];
            user.username = userJson["username"];
            user.password = userJson["password"];
            user.friends = userJson["friends"].get<std::vector<std::string>>(); // Wczytujemy listę znajomych

            // Dodajemy użytkownika do mapy users
            users[user.id] = user;

            // Aktualizujemy wartość nextUserId, aby uniknąć konfliktów ID
            if (user.id >= nextUserId) {
                nextUserId = user.id + 1;
            }
        }
        inFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku users.json!" << std::endl;
    }
}

// Funkcja aktualizująca dane użytkownika w pliku JSON
void updateUserInFile(const User& updatedUser) {
    std::ifstream inFile("resources/users.json");
    json j;

    // Wczytanie istniejącego pliku JSON
    if (inFile.is_open()) {
        inFile >> j;
        inFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku users.json do odczytu\n";
        return;
    }

    // Szukamy użytkownika i aktualizujemy jego dane
    bool userFound = false;
    for (auto& userJson : j) {
        if (userJson["id"] == updatedUser.id) {
            userJson["username"] = updatedUser.username;
            userJson["password"] = updatedUser.password;
            userJson["friends"] = updatedUser.friends;
            userFound = true;
            break;
        }
    }

    // Jeśli użytkownik nie istnieje, dodajemy nowy rekord
    if (!userFound) {
        j.push_back({
            {"id", updatedUser.id},
            {"username", updatedUser.username},
            {"password", updatedUser.password},
            {"friends", updatedUser.friends}
        });
    }

    // Zapisujemy zaktualizowane dane do pliku
    std::ofstream outFile("resources/users.json");
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku users.json do zapisu\n";
    }
}

// Funkcja zapisująca wszystkich użytkowników do pliku JSON
void saveUsersToFile() {
    std::lock_guard<std::mutex> lock(usersMutex); // Blokujemy mutex na czas zapisu
    json j;

    for (const auto& pair : users) {
        const User& user = pair.second;
        j.push_back({{"id", user.id}, {"username", user.username}, {"password", user.password}, {"friends", user.friends}});
    }

    std::ofstream outFile("resources/users.json");
    outFile << j.dump(4); // Zapisujemy dane w formacie czytelnego JSON
    outFile.close();
}

// Funkcja wczytująca konwersacje z pliku JSON
void loadConversationsFromFile() {
    std::ifstream inFile("resources/conversations.json");
    if (inFile.is_open()) {
        json j;
        inFile >> j;

        // Blokujemy mutex na czas modyfikacji zasobów współdzielonych
        std::lock_guard<std::mutex> lock(conversationsMutex);

        for (const auto& convJson : j) {
            nextConversationId += 100; // Aktualizacja identyfikatora konwersacji
            Conversation conversation;
            conversation.conversationId = convJson["conversationId"];
            conversation.participantsCount = convJson["participantsCount"];
            strncpy(conversation.group_name, convJson["group_name"].get<std::string>().c_str(), sizeof(conversation.group_name) - 1);
            conversation.group_name[sizeof(conversation.group_name) - 1] = '\0';

            memset(conversation.participants, 0, sizeof(conversation.participants));

            // Wczytanie uczestników
            if (convJson.contains("participants")) {
                const auto& participantsJson = convJson["participants"];
                for (int i = 0; i < participantsJson.size() && i < 20; ++i) {
                    strncpy(
                        conversation.participants[i],
                        participantsJson[i].get<std::string>().c_str(),
                        sizeof(conversation.participants[i]) - 1
                    );
                    conversation.participants[i][sizeof(conversation.participants[i]) - 1] = '\0';
                }
                
                conversation.participantsCount = participantsJson.size();
            }

            // Wczytanie wiadomości
            if (convJson.contains("messages")) {
                for (const auto& msgJson : convJson["messages"]) {
                    Message msg;
                    strncpy(msg.senderUsername, msgJson["username"].get<std::string>().c_str(), sizeof(msg.senderUsername) - 1);
                    msg.senderUsername[sizeof(msg.senderUsername) - 1] = '\0';

                    strncpy(msg.content, msgJson["content"].get<std::string>().c_str(), sizeof(msg.content) - 1);
                    msg.content[sizeof(msg.content) - 1] = '\0';
                    conversation.messages.push_back(msg);
                }
            }

            conversations.push_back(conversation);
        }

        inFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku conversations.json!" << std::endl;
    }
}

// Funkcja aktualizująca dane konwersacji w pliku JSON
void updateConversationInFile(const Conversation& updatedConversation) {
    std::ifstream inFile("resources/conversations.json");
    json j;

    // Wczytanie istniejącego pliku JSON
    if (inFile.is_open()) {
        inFile >> j;
        inFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku conversations.json do odczytu\n";
        return;
    }

    // Szukamy konwersacji i aktualizujemy jej dane
    bool conversationFound = false;
    for (auto& convJson : j) {
        if (convJson["conversationId"] == updatedConversation.conversationId) {
            convJson["participantsCount"] = updatedConversation.participantsCount;

            // Aktualizacja uczestników
            json participants = json::array();
            for (int i = 0; i < updatedConversation.participantsCount; ++i) {
                participants.push_back(std::string(updatedConversation.participants[i]));
            }
            convJson["participants"] = participants;

            convJson["group_name"] = updatedConversation.group_name;

            // Aktualizacja wiadomości
            json messages = json::array();
            for (const auto& message : updatedConversation.messages) {
                json msgJson;
                msgJson["username"] = message.senderUsername;
                msgJson["content"] = message.content;
                messages.push_back(msgJson);
            }
            convJson["messages"] = messages;

            conversationFound = true;
            break;
        }
    }

    // Jeśli konwersacja nie istnieje, dodajemy nową
    if (!conversationFound) {
        json newConvJson;
        newConvJson["conversationId"] = updatedConversation.conversationId;
        newConvJson["participantsCount"] = updatedConversation.participantsCount;

        // Dodajemy uczestników
        json participants = json::array();
        for (int i = 0; i < updatedConversation.participantsCount; ++i) {
            participants.push_back(std::string(updatedConversation.participants[i]));
        }
        newConvJson["participants"] = participants;

        newConvJson["group_name"] = updatedConversation.group_name;

        // Dodajemy wiadomości
        json messages = json::array();
        for (const auto& message : updatedConversation.messages) {
            json msgJson;
            msgJson["username"] = message.senderUsername;
            msgJson["content"] = message.content;
            messages.push_back(msgJson);
        }
        newConvJson["messages"] = messages;
        j.push_back(newConvJson);
    }

    // Zapisujemy zaktualizowane dane do pliku
    std::ofstream outFile("resources/conversations.json");
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku conversations.json do zapisu\n";
    }
}

// Funkcja zapisująca wszystkie konwersacje do pliku JSON
void saveConversationsToFile() {
    std::lock_guard<std::mutex> lock(conversationsMutex); // Blokujemy mutex na czas zapisu
    json j = json::array();

    for (const auto& conversation : conversations) {
        json convJson;
        convJson["conversationId"] = conversation.conversationId;
        convJson["participantsCount"] = conversation.participantsCount;

        // Tworzymy listę uczestników
        json participants = json::array();
        for (int i = 0; i < conversation.participantsCount; ++i) {
            participants.push_back(std::string(conversation.participants[i]));
        }
        convJson["participants"] = participants;

        // Tworzymy listę wiadomości
        json messages = json::array();
        for (const auto& message : conversation.messages) {
            json msg;
            msg["username"] = message.senderUsername;
            msg["content"] = message.content;
            messages.push_back(msg);
        }
        convJson["messages"] = messages;

        j.push_back(convJson);
    }

    std::ofstream outFile("resources/conversations.json");
    outFile << j.dump(4); // Zapisujemy dane w czytelnym formacie JSON
    outFile.close();
}