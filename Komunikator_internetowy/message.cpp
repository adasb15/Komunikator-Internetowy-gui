#include "message.h"
#include "ui_message.h"

// Konstruktor klasy Message
Message::Message(int clientSocket, int userId, int conversationId, QString username, QWidget *parent)
    : QMainWindow(parent)  // Inicjalizacja klasy bazowej QMainWindow
    , ui(new Ui::Message)  // Inicjalizacja interfejsu UI
    , clientSocket(clientSocket)  // Przypisanie gniazda klienta
    , userId(userId)  // Przypisanie ID użytkownika
    , conversationId(conversationId)  // Przypisanie ID konwersacji
    , userName(username)  // Przypisanie nazwy użytkownika
{
    ui->setupUi(this);  // Inicjalizacja interfejsu użytkownika
    loadHistory();  // Ładowanie historii wiadomości

    // Tworzenie nowego wątku
    workerThread = QThread::create([this, conversationId, userId]() {
        qDebug() << "Rozpoczynam działanie wątku dla rozmowy ID:" << conversationId << "i użytkownika ID:" << userId;

        int ConvSocket = socket(PF_INET, SOCK_STREAM, 0);  // Tworzenie gniazda dla nowego połączenia
        if (ConvSocket < 0) {
            qDebug() << "Błąd tworzenia gniazda!";
            return;
        }

        struct sockaddr_in sa;
        sa.sin_addr.s_addr = INADDR_ANY;  // Adres IP
        sa.sin_family = AF_INET;  // Rodzina adresów
        sa.sin_port = htons(SERVER_PORT + conversationId + userId);  // Ustalamy port na podstawie ID konwersacji i użytkownika

        // Ustawienie opcji SO_REUSEADDR przed bind
        int opt = 1;
        if (setsockopt(ConvSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            qDebug() << "Błąd ustawiania opcji SO_REUSEADDR!";
            ::close(ConvSocket);
            return;
        }

        if (::bind(ConvSocket, (struct sockaddr *)&sa, sizeof(sa)) < 0) {  // Wiązanie gniazda
            std::cerr << "Błąd wiązania gniazda!\n";
            return;
        }

        if (listen(ConvSocket, 10) < 0) {  // Nasłuchiwanie na gnieździe
            std::cerr << "Błąd nasłuchiwania na gnieździe!\n";
            return;
        }

        qDebug() << SERVER_PORT + conversationId + userId;
        convSocket = ConvSocket;

        while (this->isVisible()) {  // Kontynuuj tylko, jeśli okno jest widoczne
            int serwersocket = accept(ConvSocket, nullptr, nullptr);  // Akceptacja połączenia
            if (serwersocket < 0) {
                std::cerr << "Błąd akceptacji połączenia!";
                break;  // Przerywamy pętlę, jeśli błąd
            }

            QString buff;
            // Odbieramy ostatnią wiadomość
            Message_stru msg;
            if (recv(serwersocket, &msg, sizeof(msg), 0) < 0) {  // Odbieranie pojedynczej wiadomości
                std::cerr << "Błąd odbierania wiadomości\n";
                break;
            }

            QString sender = QString::fromUtf8(msg.senderUsername);  // Odbieranie nazwy nadawcy
            QString content = QString::fromUtf8(msg.content);  // Odbieranie treści wiadomości

            buff.append(QString("%1: %2").arg(sender, content));  // Tworzenie bufora z wiadomościami

            // Dodaj wiadomość do widoku tekstowego
            QMetaObject::invokeMethod(this, [this, buff]() {
                ui->textEdit->append(buff);  // Dodaj nową wiadomość
                ui->textEdit->moveCursor(QTextCursor::End);  // Przewiń widok na dół
            });

            ::close(serwersocket);  // Zamknięcie gniazda po przetworzeniu wiadomości
        }

        ::close(ConvSocket);  // Zamknięcie gniazda konwersacji
        qDebug() << "Wątek zakończył działanie.";
    });

    // Uruchom wątek
    workerThread->start();
}

// Destruktor klasy Message
Message::~Message()
{
    // Zatrzymywanie wątku i zamykanie gniazd
    workerThread->quit();  // Zakończenie wątku
    workerThread->wait();  // Czekaj na zakończenie wątku
    ::close(convSocket);
    delete ui;  // Usuwanie UI
}


// Metoda do obsługi zmian wprowadzanych w polu tekstowym
void Message::on_lineEdit_textChanged(const QString &arg1)
{
    text = arg1;  // Przechowuj tekst wprowadzonego przez użytkownika
}

// Metoda obsługująca kliknięcie przycisku wyślij
void Message::on_pushButton_clicked()
{
    send_message(text);  // Wyślij wiadomość
    ui->lineEdit->clear();  // Wyczyść pole tekstowe po wysłaniu
}

// Metoda wysyłająca wiadomość do serwera
void Message::send_message(QString message)
{
    Msg_to_server request;

    request.option = 5;  // Opcja wysyłania wiadomości
    request.conversationId = conversationId;  // ID konwersacji

    QByteArray byteArray = userName.toUtf8();  // Konwersja nazwy użytkownika na UTF-8
    strncpy(request.message_stru.senderUsername, byteArray.constData(), sizeof(request.message_stru.senderUsername) - 1);
    request.message_stru.senderUsername[sizeof(request.message_stru.senderUsername) - 1] = '\0';  // Zabezpieczenie przed przekroczeniem bufora

    QByteArray byteArray2 = message.toUtf8();  // Konwersja wiadomości na UTF-8
    strncpy(request.message_stru.content, byteArray2.constData(), sizeof(request.message_stru.content) - 1);
    request.message_stru.content[sizeof(request.message_stru.content) - 1] = '\0';  // Zabezpieczenie przed przekroczeniem bufora

    // Wyślij wiadomość do serwera
    if (send(clientSocket, &request, sizeof(request), 0) < 0) {
        std::cerr << "Błąd podczas wysyłania wiadomości.\n";
        return;
    }
}

// Metoda do ładowania historii wiadomości
void Message::loadHistory()
{
    int messageCount;
    // Odbierz liczbę wiadomości
    if (recv(clientSocket, &messageCount, sizeof(messageCount), 0) < 0) {
        std::cerr << "Błąd odbierania liczby wiadomości.\n";
        return;
    }

    // Wyczyść aktualny tekst w widoku historii
    ui->textEdit->clear();

    QString buff;

    if (messageCount > 0) {
        size_t bufSize = messageCount * sizeof(Message_stru);
        std::vector<Message_stru> messages(messageCount);

                    qDebug() << "dupa";

        if (recv(clientSocket, messages.data(), bufSize, 0) < 0) {  // Odbieranie wiadomości
            std::cerr << "Błąd odbierania wiadomości.\n";
            return;
        }
                    qDebug() << "dupa2";

        // Wyświetl wszystkie wiadomości
        for (auto& msg : messages) {
            QString sender = QString::fromStdString(msg.senderUsername);
            QString content = QString::fromStdString(msg.content);

            qDebug() << sender;
            qDebug() << content;
            buff.append(QString("%1: %2\n").arg(sender, content));  // Tworzenie bufora z wiadomościami
        }
    }

    // Dodaj wiadomość do widoku tekstowego
    ui->textEdit->setText(buff);  // Ustawienie wiadomości w widoku
    ui->textEdit->moveCursor(QTextCursor::End);  // Przewiń widok na dół
}

