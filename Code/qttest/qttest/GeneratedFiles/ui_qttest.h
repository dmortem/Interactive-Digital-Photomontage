/********************************************************************************
** Form generated from reading UI file 'qttest.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTEST_H
#define UI_QTTEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qttestClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qttestClass)
    {
        if (qttestClass->objectName().isEmpty())
            qttestClass->setObjectName(QStringLiteral("qttestClass"));
        qttestClass->resize(600, 400);
        menuBar = new QMenuBar(qttestClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        qttestClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qttestClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        qttestClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(qttestClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        qttestClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(qttestClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        qttestClass->setStatusBar(statusBar);

        retranslateUi(qttestClass);

        QMetaObject::connectSlotsByName(qttestClass);
    } // setupUi

    void retranslateUi(QMainWindow *qttestClass)
    {
        qttestClass->setWindowTitle(QApplication::translate("qttestClass", "qttest", 0));
    } // retranslateUi

};

namespace Ui {
    class qttestClass: public Ui_qttestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTEST_H
