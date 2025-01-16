#ifndef USERMENU_H
#define USERMENU_H

#include <QMainWindow>
#include <QString>
#include <QByteArray>
#include <QLabel>
#include <QInputDialog>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace Ui {
class Usermenu;
}

class Usermenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Usermenu(int socket, int id, QWidget *parent = nullptr);
    ~Usermenu();

private slots:
    void on_add_friend_button_clicked();

    void on_remove_friend_button_clicked();

    void on_view_friend_button_clicked();

    void on_message_button_clicked();


private:
    struct FriendAction {
        int userId;
        char friendUsername[256];
    };

    Ui::Usermenu *ui;
    int clientSocket;
    int userId;
};

#endif // USERMENU_H
