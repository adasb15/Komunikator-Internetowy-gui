/********************************************************************************
** Form generated from reading UI file 'usermenu.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMENU_H
#define UI_USERMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Usermenu
{
public:
    QWidget *centralwidget;
    QPushButton *add_friend_button;
    QPushButton *view_friend_button;
    QPushButton *remove_friend_button;
    QLabel *text_lebel;
    QLabel *text_lebel_2;
    QPushButton *message_button;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Usermenu)
    {
        if (Usermenu->objectName().isEmpty())
            Usermenu->setObjectName("Usermenu");
        Usermenu->resize(800, 600);
        centralwidget = new QWidget(Usermenu);
        centralwidget->setObjectName("centralwidget");
        add_friend_button = new QPushButton(centralwidget);
        add_friend_button->setObjectName("add_friend_button");
        add_friend_button->setGeometry(QRect(260, 70, 261, 51));
        view_friend_button = new QPushButton(centralwidget);
        view_friend_button->setObjectName("view_friend_button");
        view_friend_button->setGeometry(QRect(260, 210, 261, 51));
        remove_friend_button = new QPushButton(centralwidget);
        remove_friend_button->setObjectName("remove_friend_button");
        remove_friend_button->setGeometry(QRect(260, 140, 261, 51));
        text_lebel = new QLabel(centralwidget);
        text_lebel->setObjectName("text_lebel");
        text_lebel->setGeometry(QRect(150, 20, 481, 41));
        text_lebel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        text_lebel_2 = new QLabel(centralwidget);
        text_lebel_2->setObjectName("text_lebel_2");
        text_lebel_2->setGeometry(QRect(130, 350, 501, 221));
        text_lebel_2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        message_button = new QPushButton(centralwidget);
        message_button->setObjectName("message_button");
        message_button->setGeometry(QRect(260, 280, 261, 51));
        Usermenu->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Usermenu);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 23));
        Usermenu->setMenuBar(menubar);
        statusbar = new QStatusBar(Usermenu);
        statusbar->setObjectName("statusbar");
        Usermenu->setStatusBar(statusbar);

        retranslateUi(Usermenu);

        QMetaObject::connectSlotsByName(Usermenu);
    } // setupUi

    void retranslateUi(QMainWindow *Usermenu)
    {
        Usermenu->setWindowTitle(QCoreApplication::translate("Usermenu", "Menu u\305\274ytkownika", nullptr));
        add_friend_button->setText(QCoreApplication::translate("Usermenu", "Dodaj znajomego", nullptr));
        view_friend_button->setText(QCoreApplication::translate("Usermenu", "Wy\305\233wietl znajomych", nullptr));
        remove_friend_button->setText(QCoreApplication::translate("Usermenu", "Usu\305\204 znajomego", nullptr));
        text_lebel->setText(QString());
        text_lebel_2->setText(QString());
        message_button->setText(QCoreApplication::translate("Usermenu", "Menu wiadomo\305\233ci", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Usermenu: public Ui_Usermenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMENU_H
