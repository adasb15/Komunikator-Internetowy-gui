#ifndef MESSAGE_H
#define MESSAGE_H

#include <QMainWindow>  // Podstawowy nagłówek dla głównych okien aplikacji Qt
#include <QThread>      // Nagłówek umożliwiający tworzenie i zarządzanie wątkami
#include <QString>      // Typ danych dla stringów w Qt
#include <QByteArray>   // Typ danych umożliwiający operacje na surowych danych binarnych
#include <QLabel>       // Widżet do wyświetlania tekstu
#include <QInputDialog> // Do wyświetlania okien dialogowych do wprowadzania danych przez użytkownika
#include <iostream>     // Do korzystania z funkcji wejścia/wyjścia, np. std::cerr
#include <string>       // Standardowa biblioteka string (C++)
#include <arpa/inet.h>  // Funkcje do pracy z adresami IP
#include <unistd.h>     // Do systemowych operacji, takich jak zamykanie gniazd
#include <sys/socket.h> // Tworzenie i manipulowanie socketami
#include <netinet/in.h> // Struktury do adresów i portów w sieci

namespace Ui {
class Message;
}

// Klasa Message dziedziczy po QMainWindow, odpowiada za interfejs wiadomości
class Message : public QMainWindow
{
    Q_OBJECT

public:
    // Konstruktor klasy Message
    explicit Message(int clientSocket, int userId, int conversationId, QString username, QWidget *parent = nullptr);
    ~Message();  // Destruktor klasy Message

private slots:
    // Sloty obsługujące różne zdarzenia w aplikacji (np. zmiana tekstu w polu, kliknięcie przycisku)
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();

private:
    // Struktura reprezentująca akcje związane z przyjaciółmi (np. dodanie przyjaciela)
    struct FriendAction {
        int userId;  // ID użytkownika
        char friendUsername[256];  // Nazwa użytkownika przyjaciela
    };

    // Struktura przechowująca dane wiadomości (np. nadawca, treść)
    struct Message_stru {
        char senderUsername[256];  // Nazwa nadawcy wiadomości
        char content[1024];        // Treść wiadomości
    };

    // Struktura wiadomości wysyłanej do serwera (np. akcje przyjaciół, wiadomości)
    struct Msg_to_server {
        int option;  // Opcja określająca rodzaj akcji (np. wysyłanie wiadomości)
        int conversationId = 0;  // ID rozmowy (domyślnie 0)
        struct FriendAction friend_action;  // Akcja związana z przyjacielem
        struct Message_stru message_stru;  // Wiadomość
        char friendsNames[20][256];  // Lista przyjaciół
        char group_name[256];  // Nazwa grupy (do obsługi grupowych wiadomości)
    };

    QThread *workerThread;  // Wskaźnik na wątek, który będzie obsługiwał komunikację z serwerem

    const char* SERVER_IP = "127.0.0.1";  // Adres IP serwera
    const int SERVER_PORT = 5000;         // Port serwera, na którym działa aplikacja

    Ui::Message *ui;  // Wskaźnik na obiekt UI (interfejs użytkownika)
    int clientSocket;  // Gniazdo do komunikacji z serwerem
    int userId;        // ID użytkownika, uzyskane po zalogowaniu
    int conversationId; // ID konwersacji (dla wiadomości)
    QString userName;  // Nazwa użytkownika
    QString text;      // Przechowywanie tekstu (np. wiadomości do wysłania)

    // Funkcje pomocnicze
    void send_message(QString message);  // Funkcja do wysyłania wiadomości
    void loadHistory();  // Funkcja do ładowania historii wiadomości z serwera
};

#endif // MESSAGE_H
