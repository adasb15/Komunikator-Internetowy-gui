#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include "resources/json.hpp"

// Stałe
const int PORT = 4000;  // Port serwera

// Struktury używane do przechowywania danych użytkowników, wiadomości i rozmów
struct User {
    int id;
    std::string username;
    std::string password;
    std::vector<std::string> friends;
};

struct Register {
    char username[256];
    char password[256];
};

struct FriendAction {
    int userId;
    char friendUsername[256];
};

struct Message {
    char senderUsername[256];
    char content[1024];
};

struct Conversation {
    char group_name[256];
    int conversationId;
    int participantsCount;
    char participants[20][256];
    std::vector<Message> messages;
};

struct Msg_to_server {
    int option = 0;
    int conversationId = 0;
    struct FriendAction friend_action;
    struct Message message;
    char friendsNames[20][256];
    char group_name[256];
};

// Zmienne globalne
std::unordered_map<int, User> users;  // Przechowywanie użytkowników
std::vector<Conversation> conversations;  // Lista rozmów
int nextConversationId = 500;  // Identyfikator dla nowej rozmowy
std::mutex usersMutex;  // Mutex do synchronizacji dostępu do użytkowników
std::mutex conversationsMutex;  // Mutex do synchronizacji dostępu do rozmów
int activeClient = 0;  // Liczba aktywnych klientów
int nextUserId = 1;  // Identyfikator dla nowego użytkownika
int serverSocket;  // Gniazdo serwera

// Funkcje obsługujące różne akcje serwera
void loadUsersFromFile();  // Ładowanie użytkowników z pliku
void updateUserInFile(const User& updatedUser);  // Aktualizacja danych użytkownika
void saveUsersToFile();  // Zapisywanie użytkowników do pliku
void loadConversationsFromFile();  // Ładowanie rozmów z pliku
void saveConversationsToFile();  // Zapisywanie rozmów do pliku
void handleRegistration(int clientSocket);  // Obsługuje rejestrację użytkownika
void handleLogin(int clientSocket);  // Obsługuje logowanie użytkownika
void handleAddFriend(int clientSocket, struct FriendAction request);  // Obsługuje dodawanie znajomych
void handleRemoveFriend(int clientSocket, struct FriendAction request);  // Obsługuje usuwanie znajomych
void handleViewFriends(int clientSocket, struct FriendAction request);  // Obsługuje wyświetlanie znajomych
int findOrCreateConversation(int senderId, const std::vector<std::string>& participantUsernames, const std::string& groupName);  // Tworzy rozmowę lub zwraca istniejącą
void handleSendPrivateMessage(int clientSocket, struct FriendAction request);  // Obsługuje wysyłanie wiadomości prywatnej
void handleSendMessage(int conversationId, struct Message request);  // Obsługuje wysyłanie wiadomości w rozmowie
void handleSendGroupMessage(int clientSocket, int userId, const std::string& groupName);  // Obsługuje wysyłanie wiadomości grupowej
void handleCreateGroup(int clientSocket, const std::vector<std::string>& friendsNames, const char* group_name);  // Obsługuje tworzenie grup
void handleUserMenu(int clientSocket);  // Obsługuje menu użytkownika
void handleClient(int clientSocket);  // Obsługuje połączenie z klientem
void handleSigInt(int signum);  // Obsługuje sygnał SIGINT (np. Ctrl+C)

#endif