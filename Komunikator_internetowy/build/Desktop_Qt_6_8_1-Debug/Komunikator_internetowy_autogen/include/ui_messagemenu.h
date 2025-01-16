/********************************************************************************
** Form generated from reading UI file 'messagemenu.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEMENU_H
#define UI_MESSAGEMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Messagemenu
{
public:
    QWidget *centralwidget;
    QPushButton *priv_button;
    QPushButton *group_button;
    QPushButton *create_group_button;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Messagemenu)
    {
        if (Messagemenu->objectName().isEmpty())
            Messagemenu->setObjectName("Messagemenu");
        Messagemenu->resize(800, 600);
        centralwidget = new QWidget(Messagemenu);
        centralwidget->setObjectName("centralwidget");
        priv_button = new QPushButton(centralwidget);
        priv_button->setObjectName("priv_button");
        priv_button->setGeometry(QRect(290, 60, 221, 41));
        group_button = new QPushButton(centralwidget);
        group_button->setObjectName("group_button");
        group_button->setGeometry(QRect(290, 110, 221, 41));
        create_group_button = new QPushButton(centralwidget);
        create_group_button->setObjectName("create_group_button");
        create_group_button->setGeometry(QRect(290, 160, 221, 41));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(150, 0, 501, 61));
        Messagemenu->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Messagemenu);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 23));
        Messagemenu->setMenuBar(menubar);
        statusbar = new QStatusBar(Messagemenu);
        statusbar->setObjectName("statusbar");
        Messagemenu->setStatusBar(statusbar);

        retranslateUi(Messagemenu);

        QMetaObject::connectSlotsByName(Messagemenu);
    } // setupUi

    void retranslateUi(QMainWindow *Messagemenu)
    {
        Messagemenu->setWindowTitle(QCoreApplication::translate("Messagemenu", "Menu wiadomo\305\233ci", nullptr));
        priv_button->setText(QCoreApplication::translate("Messagemenu", "Wy\305\233lij wiadomo\305\233\304\207 prywatn\304\205", nullptr));
        group_button->setText(QCoreApplication::translate("Messagemenu", "Wy\305\233lij wiadomo\305\233\304\207 grupow\304\205", nullptr));
        create_group_button->setText(QCoreApplication::translate("Messagemenu", "Utw\303\263rz grup\304\231", nullptr));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Messagemenu: public Ui_Messagemenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEMENU_H
