#include "client.h"
#include "./ui_client.h"

// Konstruktor klasy Client
Client::Client(QWidget *parent)
    : QMainWindow(parent) // Dziedziczenie z QMainWindow
    , ui(new Ui::Client) // Inicjalizacja interfejsu użytkownika
{
    struct sockaddr_in serverAddr;

    // Tworzenie gniazda klienta
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Błąd podczas tworzenia gniazda.\n";
        return; // Zakończ, jeśli nie uda się utworzyć gniazda
    }

    // Konfiguracja adresu serwera
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    // Konwersja adresu IP na format binarny
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Nieprawidłowy adres IP.\n";
        return;
    }

    // Próba połączenia z serwerem
    if (::connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Błąd połączenia z serwerem.\n";
        return;
    }

    // Inicjalizacja interfejsu użytkownika
    ui->setupUi(this);

    // Ustawienie pierwszego widoku (ekranu logowania)
    ui->stackedWidget->setCurrentIndex(0);
    ui->Password_in->setEchoMode(QLineEdit::Password);
}

// Destruktor klasy Client
Client::~Client()
{
    Leave_server(); // Wylogowanie i zamknięcie połączenia
}

// Funkcja rejestracji
void Client::Registration(const QString &username, const QString &password) {
    int option = 1; // Opcja wysyłania danych rejestracyjnych
    send(clientSocket, &option, sizeof(option), 0);

    // Przygotowanie danych logowania
    Register registerData;
    QByteArray byteArray = username.toUtf8();
    QByteArray byteArray2 = password.toUtf8();

    // Kopiowanie danych do struktury
    strncpy(registerData.username, byteArray.constData(), sizeof(registerData.username) - 1);
    strncpy(registerData.password, byteArray2.constData(), sizeof(registerData.password) - 1);

    // Ustawienie zakończenia ciągu znaków
    registerData.username[sizeof(registerData.username) - 1] = '\0';
    registerData.password[sizeof(registerData.password) - 1] = '\0';

    // Wysłanie danych rejestracyjnych
    if (send(clientSocket, &registerData, sizeof(registerData), 0) < 0) {
        std::cerr << "Błąd wysyłania danych logowania.\n";
        return;
    }

    // Odbieranie odpowiedzi
    char response[256];
    if (recv(clientSocket, response, sizeof(response), 0) < 0) {
        std::cerr << "Błąd podczas odbierania odpowiedzi serwera.\n";
    }

    // Ustawienie odpowiedzi na etykiecie
    QString Qresponse = QString::fromUtf8(response);
    ui->menurl_label->setText(Qresponse);
}

// Funkcja logowania
void Client::Login(const QString &username, const QString &password) {
    int option = 2; // Opcja logowania
    send(clientSocket, &option, sizeof(option), 0);

    // Przygotowanie danych logowania
    Register loginData;
    QByteArray byteArray = username.toUtf8();
    QByteArray byteArray2 = password.toUtf8();

    // Kopiowanie danych do struktury
    strncpy(loginData.username, byteArray.constData(), sizeof(loginData.username) - 1);
    strncpy(loginData.password, byteArray2.constData(), sizeof(loginData.password) - 1);

    // Ustawienie zakończenia ciągu znaków
    loginData.username[sizeof(loginData.username) - 1] = '\0';
    loginData.password[sizeof(loginData.password) - 1] = '\0';

    // Wysłanie danych logowania
    if (send(clientSocket, &loginData, sizeof(loginData), 0) < 0) {
        std::cerr << "Błąd wysyłania danych logowania.\n";
        return;
    }

    // Odbieranie identyfikatora użytkownika
    if (recv(clientSocket, &userId, sizeof(userId), 0) < 0) {
        std::cerr << "Błąd podczas odbierania id od serwera.\n";
    }

    // Sprawdzenie poprawności logowania
    if (userId > 0) {
        ui->stackedWidget->setCurrentIndex(1); // Przełącz na menu użytkownika
        this->setWindowTitle("Menu użytkownika " + username);
        ui->menurl_label->clear();
        ui->Username_in->clear();
        ui->Password_in->clear();
    } else {
        ui->menurl_label->setText("Nieprawidłowe dane logowania.");
    }
}

// Funkcja wylogowania i zamknięcia połączenia
void Client::Leave_server() {
    if (userId > 0) {
        // Wysłanie opcji wylogowania
        int option = -1; // Opcja wylogowania
        if (send(clientSocket, &option, sizeof(option), 0) < 0){
            std::cerr << "Błąd podczas wysyłania wiadomości do serwera.\n";
        }

        // Zamykanie wszystkich okien wiadomości
        for (Message* messageWindow : openMessages) {
            messageWindow->close();
        }
        openMessages.clear();  // Opróżniamy listę
    }
    sleep(0.2);

    // Zamknięcie połączenia
    Msg_to_server request;
    request.option = -1;

    if (send(clientSocket, &request, sizeof(request), 0) < 0){
        std::cerr << "Błąd podczas zamykania klienta.\n";
    }

    ::close(clientSocket); // Zamykanie gniazda
    delete ui; // Usunięcie interfejsu użytkownika
}

// Funkcja obsługująca zmiany tekstu w polu nazwy użytkownika
void Client::on_Username_in_textChanged(const QString &arg1) {
    text = arg1; // Przypisanie wartości nazwy użytkownika
}

// Funkcja obsługująca zmiany tekstu w polu hasła
void Client::on_Password_in_textChanged(const QString &arg1) {
    text2 = arg1; // Przypisanie wartości hasła
}

// Funkcja obsługująca kliknięcie przycisku logowania
void Client::on_login_button_clicked() {
    userName = text; // Przypisanie nazwy użytkownika
    Login(text, text2); // Wywołanie funkcji logowania
}

// Funkcja obsługująca kliknięcie przycisku rejestracji
void Client::on_register_button_clicked() {
    Registration(text, text2); // Wywołanie funkcji rejestracji
}

// Funkcja obsługująca kliknięcie przycisku dodawania przyjaciela
void Client::on_add_friend_button_clicked() {
    bool ok;
    QString friendName = QInputDialog::getText(this,
                                               "Dodaj przyjaciela",
                                               "Wpisz nazwę przyjaciela:",
                                               QLineEdit::Normal,
                                               "",
                                               &ok); // Pobranie nazwy przyjaciela

    if (ok && !friendName.isEmpty()) {
        ui->text_lebel->clear();
        ui->text_lebel_2->clear();

        Msg_to_server request;
        request.option = 1; // Opcja dodawania przyjaciela

        request.friend_action.userId = userId;

        // Kopiowanie nazwy przyjaciela
        QByteArray byteArray = friendName.toUtf8();
        strncpy(request.friend_action.friendUsername, byteArray.constData(), sizeof(request.friend_action.friendUsername) - 1);
        request.friend_action.friendUsername[sizeof(request.friend_action.friendUsername) - 1] = '\0';

        // Wysłanie prośby do serwera
        if (send(clientSocket, &request, sizeof(request), 0) < 0) {
            std::cerr << "Błąd wysyłania prośby o dodanie znajomego.\n";
            return;
        }

        // Odbieranie odpowiedzi serwera
        char buff[256];
        if (recv(clientSocket, buff, sizeof(buff), 0) < 0) {
            std::cerr << "Błąd odebrania komunikatu od serwera.\n";
            return;
        }

        // Wyświetlanie odpowiedzi na etykiecie
        ui->text_lebel->setText(QString::fromUtf8(buff));
    }
}

// Funkcja obsługująca kliknięcie przycisku usuwania przyjaciela
void Client::on_remove_friend_button_clicked() {
    bool ok;
    QString friendName = QInputDialog::getText(this,
                                               "Usuń przyjaciela",
                                               "Wpisz nazwę przyjaciela:",
                                               QLineEdit::Normal,
                                               "",
                                               &ok); // Pobranie nazwy przyjaciela

    if (ok && !friendName.isEmpty()) {
        ui->text_lebel->clear();
        ui->text_lebel_2->clear();

        Msg_to_server request;
        request.option = 2; // Opcja usuwania przyjaciela

        request.friend_action.userId = userId;

        // Kopiowanie nazwy przyjaciela
        QByteArray byteArray = friendName.toUtf8();
        strncpy(request.friend_action.friendUsername, byteArray.constData(), sizeof(request.friend_action.friendUsername) - 1);
        request.friend_action.friendUsername[sizeof(request.friend_action.friendUsername) - 1] = '\0';

        // Wysłanie prośby do serwera
        if (send(clientSocket, &request, sizeof(request), 0) < 0) {
            std::cerr << "Błąd wysyłania prośby o usunięcie znajomego.\n";
            return;
        }

        // Odbieranie odpowiedzi serwera
        char buff[256];
        if (recv(clientSocket, buff, sizeof(buff), 0) < 0) {
            std::cerr << "Błąd odebrania komunikatu od serwera.\n";
            return;
        }

        // Wyświetlanie odpowiedzi na etykiecie
        ui->text_lebel->setText(QString::fromUtf8(buff));
    }
}

void Client::on_view_friend_button_clicked()
{
    Msg_to_server request;

    request.option = 3;  // Ustawiamy opcję, aby zapytać serwer o listę przyjaciół
    request.friend_action.userId = userId;  // Ustawiamy ID użytkownika, aby serwer wiedział, o jakich przyjaciół chodzi

    if (send(clientSocket, &request, sizeof(request), 0) < 0) {  // Wysyłamy żądanie do serwera
        std::cerr << "Błąd wysyłania prośby o wyświetlenie znajomych.\n";
        return;
    }

    char response[1024];
    recv(clientSocket, response, sizeof response, 0);  // Odbieramy odpowiedź od serwera

    ui->text_lebel_2->setText(QString::fromUtf8(response));  // Wyświetlamy odpowiedź na interfejsie użytkownika
}

void Client::on_logout_button_clicked()
{
    Msg_to_server request;
    request.option = -1;  // Opcja wylogowania
    if (userId > 0) {  // Jeśli użytkownik jest zalogowany
        if (send(clientSocket, &request, sizeof(request), 0) < 0) {  // Wysyłamy prośbę o wylogowanie
            std::cerr << "Błąd podczas wysyłania wiadomości do serwera serwera.\n";
        }
        ui->stackedWidget->setCurrentIndex(0); // Powrót do logowania
        ui->text_lebel->clear();  // Czyszczenie etykiety z wiadomościami
        ui->text_lebel_2->clear();  // Czyszczenie drugiej etykiety

        // Zamykanie wszystkich okien wiadomości
        for (Message* messageWindow : openMessages) {
            messageWindow->close();
        }
        openMessages.clear();  // Opróżniamy listę
    }
}

void Client::on_priv_button_clicked()
{
    // Wyświetlenie dialogu do wpisania nazwy przyjaciela
    QString friendName;
    bool ok;

    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("Wybierz przyjaciela, któremu chcesz wysłać wiadomość.");
    inputDialog.setLabelText("Wpisz nazwę przyjaciela:");
    inputDialog.setTextValue("");
    inputDialog.setInputMode(QInputDialog::TextInput);

    inputDialog.resize(600, 150);  // Ustawiamy rozmiar okna

    // Pokazujemy okno dialogowe
    if (inputDialog.exec() == QDialog::Accepted) {
        friendName = inputDialog.textValue();
        ok = true;
    }

    if (ok && !friendName.isEmpty()) {
        ui->text_lebel->clear();
        ui->text_lebel_2->clear();

        Msg_to_server request;

        request.option = 4;  // Opcja dla prywatnej wiadomości

        QByteArray byteArray = friendName.toUtf8();  // Konwertujemy nazwę na format UTF-8

        strncpy(request.friend_action.friendUsername, byteArray.constData(), sizeof(request.friend_action.friendUsername) - 1);  // Przypisujemy nazwę

        request.friend_action.friendUsername[sizeof(request.friend_action.friendUsername) - 1] = '\0';  // Upewniamy się, że ciąg jest zakończony null-em

        request.friend_action.userId = userId;  // Ustawiamy ID użytkownika, aby serwer wiedział, od kogo pochodzi żądanie
        if (send(clientSocket, &request, sizeof(request), 0) < 0) {  // Wysyłamy dane do serwera
            std::cerr << "Błąd podczas wysyłania danych użytkownika.\n";
            return;
        }

        int conversationId;

        if (recv(clientSocket, &conversationId, sizeof(conversationId), 0) < 0) {  // Odbieramy ID konwersacji od serwera
            std::cerr << "Błąd podczas odbierania ID konwersacji.\n";
            return;
        }

        if (conversationId == -1) {  // Jeśli konwersacja nie istnieje
            ui->text_lebel->setText("Nie znaleziono użytkownika lub nie można utworzyć konwersacji.\n");
            return;
        }

        Message *message = new Message(clientSocket, userId, conversationId, userName);
        message->show();  // Pokazujemy nowe okno z wiadomościami
        message->setWindowTitle("Prywatna konwersacja z " + friendName);

        // Dodajemy otwarte okno do listy
        openMessages.append(message);
    }
}

void Client::on_group_button_clicked()
{
    QString groupName;
    bool ok;

    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("Wybierz grupę, do której chcesz wysłać wiadomość.");
    inputDialog.setLabelText("Wpisz nazwę grupy:");
    inputDialog.setTextValue("");
    inputDialog.setInputMode(QInputDialog::TextInput);

    inputDialog.resize(600, 150);  // Ustawiamy rozmiar okna

    // Pokazujemy okno dialogowe
    if (inputDialog.exec() == QDialog::Accepted) {
        groupName = inputDialog.textValue();
        ok = true;
    }

    if (ok && !groupName.isEmpty()) {
        ui->text_lebel->clear();
        ui->text_lebel_2->clear();

        Msg_to_server request;

        request.option = 8;  // Opcja dla grupy

        QByteArray byteArray = groupName.toUtf8();  // Konwertujemy nazwę grupy na UTF-8

        strncpy(request.group_name, byteArray.constData(), sizeof(request.group_name) - 1);  // Ustawiamy nazwę grupy

        request.group_name[sizeof(request.group_name) - 1] = '\0';  // Zapewniamy, że ciąg znaków jest zakończony null-em

        request.friend_action.userId = userId;  // Ustawiamy ID użytkownika

        if (send(clientSocket, &request, sizeof(request), 0) < 0) {  // Wysyłamy dane do serwera
            std::cerr << "Błąd podczas wysyłania danych użytkownika.\n";
            return;
        }

        int conversationId;

        if (recv(clientSocket, &conversationId, sizeof(conversationId), 0) < 0) {  // Odbieramy ID konwersacji od serwera
            std::cerr << "Błąd podczas odbierania ID konwersacji.\n";
            return;
        }

        if (conversationId == -1) {  // Jeśli grupa nie została znaleziona
            ui->text_lebel->setText("Nie znaleziono tej grupy, musisz ją najpierw utworzyć.\n");
            return;
        }

        Message *message = new Message(clientSocket, userId, conversationId, userName);
        message->show();  // Wyświetlamy okno z wiadomościami dla grupy
        message->setWindowTitle("Konwersacja grupy " + groupName);

        // Dodajemy otwarte okno do listy
        openMessages.append(message);
    }
}

void Client::on_create_group_button_clicked()
{
    QString friend_number_str;
    bool ok;

    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("Podaj ilu osobową grupę chcesz utworzyć.");
    inputDialog.setLabelText("Wpisz liczbę:");
    inputDialog.setTextValue("");
    inputDialog.setInputMode(QInputDialog::TextInput);

    inputDialog.resize(400, 150);  // Ustawiamy rozmiar okna

    // Pokazujemy okno dialogowe
    if (inputDialog.exec() == QDialog::Accepted) {
        friend_number_str = inputDialog.textValue();
        ok = true;
    }

    if (ok && !friend_number_str.isEmpty()) {
        ui->text_lebel->clear();
        ui->text_lebel_2->clear();

        Msg_to_server request;
        request.option = 7;  // Opcja dla tworzenia grupy

        int friend_number = friend_number_str.toInt();  // Liczba przyjaciół w grupie

        if (friend_number == 0) {
            ui->text_lebel->setText("Podano nieprawidłową wartość.");
            return;
        }

        memset(&request.friendsNames, 0, sizeof(request.friendsNames));

        // Ustawiamy nazwę użytkownika jako pierwszego członka grupy
        QByteArray byteArray = userName.toUtf8();
        strncpy(request.friendsNames[0], byteArray.constData(), sizeof(request.friendsNames[0]) - 1);
        request.friendsNames[0][sizeof(request.friendsNames[0]) - 1] = '\0';

        bool ok2;

        // Pobieramy pozostałe nazwy przyjaciół
        for(int i = 0; i < friend_number - 1; i++) {
            QString friend_name;
            ok2 = false;
            QInputDialog inputDialog(this);
            inputDialog.setWindowTitle("Podaj " + QString::number(i+1) + " znajomego.");
            inputDialog.setLabelText("Wpisz nazwę:");
            inputDialog.setTextValue("");
            inputDialog.setInputMode(QInputDialog::TextInput);

            inputDialog.resize(300, 150);  // Ustawiamy rozmiar okna

            if (inputDialog.exec() == QDialog::Accepted) {
                friend_name = inputDialog.textValue();
                ok2 = true;
            }

            if (ok2 && !friend_name.isEmpty()) {
                QByteArray byteArray = friend_name.toUtf8();
                strncpy(request.friendsNames[i+1], byteArray.constData(), sizeof(request.friendsNames[i+1]) - 1);
                request.friendsNames[i+1][sizeof(request.friendsNames[i+1]) - 1] = '\0';
            }
        }

        QString group_name;
        bool ok3;

        QInputDialog inputDialog(this);
        inputDialog.setWindowTitle("Podaj nazwę grupy.");
        inputDialog.setLabelText("Wpisz nazwę:");
        inputDialog.setTextValue("");
        inputDialog.setInputMode(QInputDialog::TextInput);

        inputDialog.resize(300, 150);  // Ustawiamy rozmiar okna

        if (inputDialog.exec() == QDialog::Accepted) {
            group_name = inputDialog.textValue();
            ok3 = true;
        }

        if (ok3 && !group_name.isEmpty()) {
            QByteArray byteArray2 = group_name.toUtf8();
            strncpy(request.group_name, byteArray2.constData(), sizeof(request.group_name) - 1);
            request.group_name[sizeof(request.group_name) - 1] = '\0';

            if (send(clientSocket, &request, sizeof(request), 0) < 0) {
                std::cerr << "Błąd podczas wysyłania\n";
            }

            char buff[256];
            if (recv(clientSocket, &buff, sizeof(buff), 0) < 0) {
                std::cerr << "Błąd poczas odbierania\n";
            }

            ui->text_lebel->setText(QString::fromUtf8(buff));  // Wyświetlamy odpowiedź z serwera
        }
    }
}
