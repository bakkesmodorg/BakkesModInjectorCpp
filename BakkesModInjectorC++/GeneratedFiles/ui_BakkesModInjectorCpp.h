/********************************************************************************
** Form generated from reading UI file 'BakkesModInjectorCpp.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BAKKESMODINJECTORCPP_H
#define UI_BAKKESMODINJECTORCPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BakkesModInjectorCppClass
{
public:
    QAction *actionOpen_BakkesMod_folder;
    QAction *actionInstall_Python_support;
    QAction *actionReinstall;
    QAction *actionExit;
    QAction *actionBakkesMod_Twitter;
    QAction *actionBakkesMod_com;
    QAction *actionBakkesMod_Discord;
    QAction *actionCheck_injection;
    QAction *actionEnable_safe_mode;
    QAction *actionRun_on_startup;
    QAction *actionHide_when_minimized;
    QAction *actionMinimize_on_start;
    QAction *actionTroubleshooting;
    QAction *actionManually_select_BakkesMod_folder;
    QAction *actionSet_injection_timeout;
    QAction *actionDisable_warnings;
    QAction *actionCheck_for_updates;
    QAction *actionPatreon;
    QWidget *centralWidget;
    QLabel *label;
    QProgressBar *progressBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuLinks;
    QMenu *menuHelp;
    QMenu *menuSettings;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BakkesModInjectorCppClass)
    {
        if (BakkesModInjectorCppClass->objectName().isEmpty())
            BakkesModInjectorCppClass->setObjectName(QString::fromUtf8("BakkesModInjectorCppClass"));
        BakkesModInjectorCppClass->resize(310, 85);
        actionOpen_BakkesMod_folder = new QAction(BakkesModInjectorCppClass);
        actionOpen_BakkesMod_folder->setObjectName(QString::fromUtf8("actionOpen_BakkesMod_folder"));
        actionInstall_Python_support = new QAction(BakkesModInjectorCppClass);
        actionInstall_Python_support->setObjectName(QString::fromUtf8("actionInstall_Python_support"));
        actionReinstall = new QAction(BakkesModInjectorCppClass);
        actionReinstall->setObjectName(QString::fromUtf8("actionReinstall"));
        actionExit = new QAction(BakkesModInjectorCppClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionBakkesMod_Twitter = new QAction(BakkesModInjectorCppClass);
        actionBakkesMod_Twitter->setObjectName(QString::fromUtf8("actionBakkesMod_Twitter"));
        actionBakkesMod_com = new QAction(BakkesModInjectorCppClass);
        actionBakkesMod_com->setObjectName(QString::fromUtf8("actionBakkesMod_com"));
        actionBakkesMod_Discord = new QAction(BakkesModInjectorCppClass);
        actionBakkesMod_Discord->setObjectName(QString::fromUtf8("actionBakkesMod_Discord"));
        actionCheck_injection = new QAction(BakkesModInjectorCppClass);
        actionCheck_injection->setObjectName(QString::fromUtf8("actionCheck_injection"));
        actionEnable_safe_mode = new QAction(BakkesModInjectorCppClass);
        actionEnable_safe_mode->setObjectName(QString::fromUtf8("actionEnable_safe_mode"));
        actionEnable_safe_mode->setCheckable(true);
        actionRun_on_startup = new QAction(BakkesModInjectorCppClass);
        actionRun_on_startup->setObjectName(QString::fromUtf8("actionRun_on_startup"));
        actionRun_on_startup->setCheckable(true);
        actionHide_when_minimized = new QAction(BakkesModInjectorCppClass);
        actionHide_when_minimized->setObjectName(QString::fromUtf8("actionHide_when_minimized"));
        actionHide_when_minimized->setCheckable(true);
        actionMinimize_on_start = new QAction(BakkesModInjectorCppClass);
        actionMinimize_on_start->setObjectName(QString::fromUtf8("actionMinimize_on_start"));
        actionMinimize_on_start->setCheckable(true);
        actionTroubleshooting = new QAction(BakkesModInjectorCppClass);
        actionTroubleshooting->setObjectName(QString::fromUtf8("actionTroubleshooting"));
        actionManually_select_BakkesMod_folder = new QAction(BakkesModInjectorCppClass);
        actionManually_select_BakkesMod_folder->setObjectName(QString::fromUtf8("actionManually_select_BakkesMod_folder"));
        actionSet_injection_timeout = new QAction(BakkesModInjectorCppClass);
        actionSet_injection_timeout->setObjectName(QString::fromUtf8("actionSet_injection_timeout"));
        actionDisable_warnings = new QAction(BakkesModInjectorCppClass);
        actionDisable_warnings->setObjectName(QString::fromUtf8("actionDisable_warnings"));
        actionDisable_warnings->setCheckable(true);
        actionCheck_for_updates = new QAction(BakkesModInjectorCppClass);
        actionCheck_for_updates->setObjectName(QString::fromUtf8("actionCheck_for_updates"));
        actionPatreon = new QAction(BakkesModInjectorCppClass);
        actionPatreon->setObjectName(QString::fromUtf8("actionPatreon"));
        centralWidget = new QWidget(BakkesModInjectorCppClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 251, 41));
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(100, 10, 211, 23));
        progressBar->setValue(0);
        BakkesModInjectorCppClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BakkesModInjectorCppClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 310, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuLinks = new QMenu(menuBar);
        menuLinks->setObjectName(QString::fromUtf8("menuLinks"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        BakkesModInjectorCppClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BakkesModInjectorCppClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        BakkesModInjectorCppClass->addToolBar(Qt::BottomToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BakkesModInjectorCppClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        BakkesModInjectorCppClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuLinks->menuAction());
        menuBar->addAction(menuSettings->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen_BakkesMod_folder);
        menuFile->addAction(actionCheck_for_updates);
        menuFile->addAction(actionReinstall);
        menuFile->addAction(actionExit);
        menuLinks->addAction(actionBakkesMod_com);
        menuLinks->addAction(actionBakkesMod_Twitter);
        menuLinks->addAction(actionBakkesMod_Discord);
        menuLinks->addAction(actionPatreon);
        menuHelp->addAction(actionCheck_injection);
        menuHelp->addAction(actionTroubleshooting);
        menuSettings->addAction(actionEnable_safe_mode);
        menuSettings->addAction(actionRun_on_startup);
        menuSettings->addAction(actionHide_when_minimized);
        menuSettings->addAction(actionMinimize_on_start);
        menuSettings->addAction(actionManually_select_BakkesMod_folder);
        menuSettings->addAction(actionSet_injection_timeout);
        menuSettings->addAction(actionDisable_warnings);

        retranslateUi(BakkesModInjectorCppClass);
        QObject::connect(actionOpen_BakkesMod_folder, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnOpenBakkesModFolderClicked()));
        QObject::connect(actionCheck_injection, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnCheckInjection()));
        QObject::connect(actionEnable_safe_mode, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnCheckSafeMode()));
        QObject::connect(actionRun_on_startup, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnRunOnStartup()));
        QObject::connect(actionHide_when_minimized, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnHideOnMinimize()));
        QObject::connect(actionMinimize_on_start, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnMinimizeOnStart()));
        QObject::connect(actionExit, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnExitClick()));
        QObject::connect(actionReinstall, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnReinstallClick()));
        QObject::connect(actionInstall_Python_support, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnPythonInstallClick()));
        QObject::connect(actionBakkesMod_Discord, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OpenDiscord()));
        QObject::connect(actionBakkesMod_Twitter, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OpenTwitter()));
        QObject::connect(actionBakkesMod_com, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OpenWebsite()));
        QObject::connect(actionTroubleshooting, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OpenTroubleshootPage()));
        QObject::connect(actionManually_select_BakkesMod_folder, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnSelectBakkesModFolder()));
        QObject::connect(actionSet_injection_timeout, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnSetInjectionTimeout()));
        QObject::connect(actionDisable_warnings, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnDisableWarnings()));
        QObject::connect(actionCheck_for_updates, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnCheckForUpdates()));
        QObject::connect(actionPatreon, SIGNAL(triggered()), BakkesModInjectorCppClass, SLOT(OnPatreonClick()));

        QMetaObject::connectSlotsByName(BakkesModInjectorCppClass);
    } // setupUi

    void retranslateUi(QMainWindow *BakkesModInjectorCppClass)
    {
        BakkesModInjectorCppClass->setWindowTitle(QCoreApplication::translate("BakkesModInjectorCppClass", "BakkesModInjectorCpp", nullptr));
        actionOpen_BakkesMod_folder->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Open BakkesMod folder", nullptr));
        actionInstall_Python_support->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Install Python support", nullptr));
        actionReinstall->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Reinstall", nullptr));
        actionExit->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Exit", nullptr));
        actionBakkesMod_Twitter->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "BakkesMod Twitter", nullptr));
        actionBakkesMod_com->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "BakkesMod.com", nullptr));
        actionBakkesMod_Discord->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "BakkesMod Discord", nullptr));
        actionCheck_injection->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Check injection", nullptr));
        actionEnable_safe_mode->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Enable safe mode", nullptr));
        actionRun_on_startup->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Run on startup", nullptr));
        actionHide_when_minimized->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Hide when minimized", nullptr));
        actionMinimize_on_start->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Minimize on start", nullptr));
        actionTroubleshooting->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Troubleshooting", nullptr));
        actionManually_select_BakkesMod_folder->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Manually select BakkesMod folder", nullptr));
        actionSet_injection_timeout->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Set injection timeout (150)", nullptr));
        actionDisable_warnings->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Disable warnings", nullptr));
        actionCheck_for_updates->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Check for updates", nullptr));
        actionPatreon->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Patreon", nullptr));
        label->setText(QCoreApplication::translate("BakkesModInjectorCppClass", "Booting", nullptr));
        menuFile->setTitle(QCoreApplication::translate("BakkesModInjectorCppClass", "File", nullptr));
        menuLinks->setTitle(QCoreApplication::translate("BakkesModInjectorCppClass", "Links", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("BakkesModInjectorCppClass", "Help", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("BakkesModInjectorCppClass", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BakkesModInjectorCppClass: public Ui_BakkesModInjectorCppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BAKKESMODINJECTORCPP_H
