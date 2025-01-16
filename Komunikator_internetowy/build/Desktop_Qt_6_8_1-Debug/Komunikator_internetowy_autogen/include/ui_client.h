/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Client
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QLabel *menurl_label;
    QLabel *username_label;
    QLineEdit *Username_in;
    QLabel *password_label;
    QLineEdit *Password_in;
    QPushButton *login_button;
    QPushButton *register_button;
    QWidget *page_2;
    QLabel *text_lebel;
    QPushButton *add_friend_button;
    QPushButton *remove_friend_button;
    QPushButton *view_friend_button;
    QPushButton *logout_button;
    QPushButton *group_button;
    QPushButton *priv_button;
    QPushButton *create_group_button;
    QTextEdit *text_lebel_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Client)
    {
        if (Client->objectName().isEmpty())
            Client->setObjectName("Client");
        Client->resize(819, 638);
        centralwidget = new QWidget(Client);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(0, 0, 821, 621));
        stackedWidget->setMaximumSize(QSize(821, 621));
        page = new QWidget();
        page->setObjectName("page");
        page->setMinimumSize(QSize(821, 0));
        menurl_label = new QLabel(page);
        menurl_label->setObjectName("menurl_label");
        menurl_label->setGeometry(QRect(10, 10, 801, 51));
        QFont font;
        font.setPointSize(20);
        menurl_label->setFont(font);
        menurl_label->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        menurl_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        username_label = new QLabel(page);
        username_label->setObjectName("username_label");
        username_label->setGeometry(QRect(350, 80, 121, 41));
        username_label->setFont(font);
        Username_in = new QLineEdit(page);
        Username_in->setObjectName("Username_in");
        Username_in->setGeometry(QRect(250, 120, 321, 71));
        QFont font1;
        font1.setPointSize(18);
        Username_in->setFont(font1);
        password_label = new QLabel(page);
        password_label->setObjectName("password_label");
        password_label->setGeometry(QRect(350, 200, 121, 41));
        password_label->setFont(font);
        Password_in = new QLineEdit(page);
        Password_in->setObjectName("Password_in");
        Password_in->setGeometry(QRect(250, 250, 321, 71));
        Password_in->setFont(font1);
        login_button = new QPushButton(page);
        login_button->setObjectName("login_button");
        login_button->setGeometry(QRect(320, 350, 171, 51));
        login_button->setFont(font);
        register_button = new QPushButton(page);
        register_button->setObjectName("register_button");
        register_button->setGeometry(QRect(320, 430, 171, 51));
        register_button->setFont(font);
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        text_lebel = new QLabel(page_2);
        text_lebel->setObjectName("text_lebel");
        text_lebel->setGeometry(QRect(170, 20, 481, 41));
        text_lebel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        add_friend_button = new QPushButton(page_2);
        add_friend_button->setObjectName("add_friend_button");
        add_friend_button->setGeometry(QRect(110, 70, 261, 51));
        remove_friend_button = new QPushButton(page_2);
        remove_friend_button->setObjectName("remove_friend_button");
        remove_friend_button->setGeometry(QRect(110, 140, 261, 51));
        view_friend_button = new QPushButton(page_2);
        view_friend_button->setObjectName("view_friend_button");
        view_friend_button->setGeometry(QRect(110, 210, 261, 51));
        logout_button = new QPushButton(page_2);
        logout_button->setObjectName("logout_button");
        logout_button->setGeometry(QRect(720, 20, 87, 26));
        group_button = new QPushButton(page_2);
        group_button->setObjectName("group_button");
        group_button->setGeometry(QRect(440, 140, 261, 51));
        priv_button = new QPushButton(page_2);
        priv_button->setObjectName("priv_button");
        priv_button->setGeometry(QRect(440, 70, 261, 51));
        create_group_button = new QPushButton(page_2);
        create_group_button->setObjectName("create_group_button");
        create_group_button->setGeometry(QRect(440, 210, 261, 51));
        text_lebel_2 = new QTextEdit(page_2);
        text_lebel_2->setObjectName("text_lebel_2");
        text_lebel_2->setGeometry(QRect(230, 320, 351, 211));
        text_lebel_2->setReadOnly(true);
        stackedWidget->addWidget(page_2);
        Client->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Client);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 819, 23));
        Client->setMenuBar(menubar);
        statusbar = new QStatusBar(Client);
        statusbar->setObjectName("statusbar");
        Client->setStatusBar(statusbar);

        retranslateUi(Client);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Client);
    } // setupUi

    void retranslateUi(QMainWindow *Client)
    {
        Client->setWindowTitle(QCoreApplication::translate("Client", "Client", nullptr));
        menurl_label->setText(QCoreApplication::translate("Client", "Wpisz dane", nullptr));
        username_label->setText(QCoreApplication::translate("Client", "Username", nullptr));
        Username_in->setText(QString());
        password_label->setText(QCoreApplication::translate("Client", "Password", nullptr));
        Password_in->setText(QString());
        login_button->setText(QCoreApplication::translate("Client", "Log in", nullptr));
        register_button->setText(QCoreApplication::translate("Client", "Sign up", nullptr));
        text_lebel->setText(QString());
        add_friend_button->setText(QCoreApplication::translate("Client", "Dodaj znajomego", nullptr));
        remove_friend_button->setText(QCoreApplication::translate("Client", "Usu\305\204 znajomego", nullptr));
        view_friend_button->setText(QCoreApplication::translate("Client", "Wy\305\233wietl znajomych", nullptr));
        logout_button->setText(QCoreApplication::translate("Client", "Wyloguj", nullptr));
        group_button->setText(QCoreApplication::translate("Client", "Wy\305\233lij wiadomo\305\233\304\207 grupow\304\205", nullptr));
        priv_button->setText(QCoreApplication::translate("Client", "Wy\305\233lij wiadomo\305\233\304\207 prywatn\304\205", nullptr));
        create_group_button->setText(QCoreApplication::translate("Client", "Utw\303\263rz grup\304\231", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Client: public Ui_Client {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
