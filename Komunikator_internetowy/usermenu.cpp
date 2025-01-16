#include "usermenu.h"
#include "ui_usermenu.h"
#include "messagemenu.h"

Usermenu::Usermenu(int socket, int id, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Usermenu)
    , clientSocket(socket)
    , userId(id)
{
    ui->setupUi(this);
}

Usermenu::~Usermenu()
{
    delete ui;
}

void Usermenu::on_add_friend_button_clicked()
{
    // Wyświetlenie dialogu do wpisania nazwy przyjaciela
    bool ok;
    QString friendName = QInputDialog::getText(this,
                                               "Dodaj przyjaciela",
                                               "Wpisz nazwę przyjaciela:",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);
    if (ok && !friendName.isEmpty()) {
        int option = 1;
        send(clientSocket, &option, sizeof(option), 0);

        FriendAction request;

        request.userId = userId;

        QByteArray byteArray = friendName.toUtf8();

        strncpy(request.friendUsername, byteArray.constData(), sizeof(request.friendUsername) - 1);

        request.friendUsername[sizeof(request.friendUsername) - 1] = '\0';

        if (send(clientSocket, &request, sizeof(request), 0) < 0) {
            std::cerr << "Błąd wysyłania prośby o dodanie znajomego.\n";
            return;
        }

        char buff[256];
        if (recv(clientSocket, buff, sizeof(buff), 0) < 0) {
            std::cerr << "Błąd odebrania komunikatu od serwera.\n";
            return;
        }

        ui->text_lebel->setText(QString::fromUtf8(buff));
    }
}


void Usermenu::on_remove_friend_button_clicked()
{
    // Wyświetlenie dialogu do wpisania nazwy przyjaciela
    bool ok;
    QString friendName = QInputDialog::getText(this,
                                               "Usuń przyjaciela",
                                               "Wpisz nazwę przyjaciela:",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);
    if (ok && !friendName.isEmpty()) {
        int option = 2;
        send(clientSocket, &option, sizeof(option), 0);

        FriendAction request;

        request.userId = userId;

        QByteArray byteArray = friendName.toUtf8();

        strncpy(request.friendUsername, byteArray.constData(), sizeof(request.friendUsername) - 1);

        request.friendUsername[sizeof(request.friendUsername) - 1] = '\0';

        if (send(clientSocket, &request, sizeof(request), 0) < 0) {
            std::cerr << "Błąd wysyłania prośby o usuniecie znajomego.\n";
            return;
        }

        char buff[256];
        if (recv(clientSocket, buff, sizeof(buff), 0) < 0) {
            std::cerr << "Błąd odebrania komunikatu od serwera.\n";
            return;
        }

        ui->text_lebel->setText(QString::fromUtf8(buff));
    }
}


void Usermenu::on_view_friend_button_clicked()
{
    int option = 3;
    send(clientSocket, &option, sizeof(option), 0);

    FriendAction request;
    request.userId = userId;

    if (send(clientSocket, &request, sizeof(request), 0) < 0) {
        std::cerr << "Błąd wysyłania prośby o wyświetlenie znajomych.\n";
        return;
    }

    char response[1024];
    recv(clientSocket, response, sizeof response, 0);
    ui->text_lebel_2->setText(QString::fromUtf8(response));
}


void Usermenu::on_message_button_clicked()
{
    Messagemenu *messagemenu = new Messagemenu(clientSocket, userId);
    messagemenu->show(); // Wyświetlenie nowego okna
    //this->close();    // Zamknięcie bieżącego okna
}


