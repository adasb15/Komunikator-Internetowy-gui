#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QString>
#include <QByteArray>
#include <QLabel>
#include <QInputDialog>
#include <iostream>
#include <string>
#include <arpa/inet.h>  // Do obsługi socketów
#include <unistd.h>     // Do operacji na systemie
#include <sys/socket.h> // Tworzenie socketów
#include <netinet/in.h> // Struktury dla socketów
#include "message.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }  // Forward declaration dla klasy UI
QT_END_NAMESPACE

// Klasa Client dziedziczy po QMainWindow, odpowiada za interfejs klienta
class Client : public QMainWindow
{
    Q_OBJECT  // Makro umożliwiające integrację z systemem sygnałów i slotów w Qt

public:
    Client(QWidget *parent = nullptr);  // Konstruktor klasy Client
    ~Client();  // Destruktor klasy Client

private slots:
    // Sloty odpowiedzialne za różne zdarzenia, np. zmiany tekstu, kliknięcie przycisków
    void on_Username_in_textChanged(const QString &arg1);
    void on_Password_in_textChanged(const QString &arg1);
    void on_login_button_clicked();
    void on_register_button_clicked();

    void on_add_friend_button_clicked();
    void on_remove_friend_button_clicked();
    void on_view_friend_button_clicked();
    void on_logout_button_clicked();
    void on_priv_button_clicked();
    void on_group_button_clicked();
    void on_create_group_button_clicked();

private:
    // Struktura do przechowywania danych rejestracji
    struct Register {
        char username[256];  // Nazwa użytkownika
        char password[256];  // Hasło użytkownika
    };

    // Struktura do obsługi akcji związanych z przyjaciółmi
    struct FriendAction {
        int userId;  // ID użytkownika wykonującego akcję
        char friendUsername[256];  // Nazwa przyjaciela użytkownika
    };

    // Struktura do przechowywania wiadomości
    struct Message_stru {
        char senderUsername[256];  // Nazwa nadawcy
        char content[1024];  // Treść wiadomości
    };

    // Struktura wiadomości wysyłanej do serwera
    struct Msg_to_server {
        int option;  // Opcja działania (np. rejestracja, logowanie, dodawanie przyjaciela)
        int conversationId = 0;  // ID konwersacji (domyślnie 0)
        struct FriendAction friend_action;  // Działanie związane z przyjacielem
        struct Message_stru message_stru;  // Wiadomość
        char friendsNames[20][256];  // Lista przyjaciół (maksymalnie 20)
        char group_name[256];  // Nazwa grupy (do grupowych wiadomości)
    };

    Ui::Client *ui;  // Wskaźnik do obiektu UI (interfejsu użytkownika)
    QString text, text2;  // Przechowywanie tekstów użytkownika (np. login, hasło)
    int clientSocket;  // Gniazdo klienta do komunikacji z serwerem
    int userId;  // ID użytkownika (po zalogowaniu)
    QString userName;  // Nazwa użytkownika
    QList<Message*> openMessages;  // Lista przechowująca wskaźniki do okien wiadomości

    const char* SERVER_IP = "127.0.0.1";  // Adres IP serwera
    const int SERVER_PORT = 5000;  // Port serwera

    // Funkcje pomocnicze
    void Login(const QString &username, const QString &password);  // Funkcja logowania
    void Registration(const QString &username, const QString &password);  // Funkcja rejestracji
    void Leave_server();  // Funkcja do wylogowania i zakończenia połączenia z serwerem
};

#endif // CLIENT_H
