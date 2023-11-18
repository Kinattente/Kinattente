#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configassistant.h"
#include "server.h"
#include "client.h"
#include "patientform.h"
#include "kineform.h"
#include "waitinglist.h"
#include "wltitle.h"
#include "usualfonctions.h"
#include "options.h"
#include "updater.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QDesktopWidget>
#include <QSqlQueryModel>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QProcess>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QToolButton>
#include <QTabBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    explicit MainWindow(QWidget *parent = nullptr);// Constructeur
    MainWindow(MainWindow const&) = delete;// Constructeur de copie
    MainWindow& operator=(MainWindow const&) = delete;// Opérateur de copie
    ~MainWindow();// Destructeur
    bool hasProblemOccurred();// Accesseur renseignant si les connexions à la BDD et au serveur sont opérationnelles
    enum actualisationType {Addition = 0, Withdrawal, PatientEdit, Unarchiving, KineEdit };
    enum waitingList {Office, Home };
    enum index {Archives, Editor };

    void refreshWaitingList(const enum actualisationType actualisationType, const int id_Patient = 0, const enum waitingList WLType = Office, const enum waitingList oldWLType = Office);// Méthode pour actualiser les listes d'attente
    void refreshArchivesEditor(const index index);// Méthode pour actualiser les archives patients et l'éditeur de kinés

    signals:
    void refreshTransmission(QString const message);// Signal pour émission d'une MàJ de la BDD

    public slots:
    void refreshReception(QString const message = "");// Slot pour réception d'une MàJ de la BDD

    private slots:
    void changePage(int const index);// Slot pour changer de page
    void createWithdrawalEditButtonsConnections();// Slot pour créer les connexions des boutons retirer et modifier des etiquettes
    void openPatientForm(int const id = 0);// Slot pour ouvrir le formulaire d'ajout de patient
    void openKineForm(int const id = 0);// Slot pour ouvrir le formulaire d'ajout de kiné
    void withdrawalPatient(int id);// Slot pour retirer un patient de la liste
    void clicSelection(const QModelIndex &index);// Slot pour afficher la sélection
    void clicAEButton();// Slot pour désarchiver la sélection ou modifier un kiné
    void supprKineButtonClicked();// Slot pour supprimer un kiné
    void openOptions();// Slot pour ouvrir les options
    void ipChanged();// Slot pour reconnecter à la Database quand l'IP a changé
    bool exportBDD(bool mandatoryBackup = false);// Slot pour exporter la BDD
    void importBDD();// Slot pour importer la BDD
    void faq();// Slot pour ouvrir la FAQ du site internet
    void launchAbout();// Slot pour ouvrir la fenêtre à propos
    QString downloadInfoFromWebsite(QRegExp pattern);// Slot pour récupérer une info sur le site internet
    void refreshUpdate(bool softwareOpening = 0);// Slot pour actualiser l'affichage d'une mise à jour potentielle
    void launchUpdate(bool const forcingUpdate = false);// Slot pour mettre à jour le logiciel
    //bool close(QString text);

    private:
    void configAssistant();// Méthode pour lancer l'assistant de configuration
    bool connections();// Méthode pour lancer la connexion à la BDD, le serveur et le client
    void createActions();// Méthode pour créer la Menu Bar et la Tool Bar
    void createArchivesEditor(const index index);// Méthode pour créer la page contenant les archives patients et l'éditeur de kinés
    void createWaitingList();// Méthode pour créer la page contenant les listes d'attente
    void flushOldestPatients();// Méthode pour retirer du logiciel les patients les plus anciens
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void loadStyleSheet();

    // Affichage des listes d'attentes
    Server *m_Server;
    Client *m_Client;
    bool m_problemOccurred;
    QSettings m_settings;
    QStackedWidget m_stackedWidget;
    QList<QToolButton*> m_toolButtons;
        QList<QSqlQueryModel*> m_AEModel;
        QList<QTableView*> m_AEView;
            QList<QSortFilterProxyModel*> m_AEModelProxy;
        QList<QLineEdit*> m_AELineEdit;
        QList<QLabel*> m_AELabel;
        QList<WLTitle*> m_WLTitleList;
        QList<WaitingList*> m_WLList;
        QHBoxLayout m_updateLayout;
        QLabel m_version;
        LinkedPicture *m_updatePicture;
            QList<QVBoxLayout*> m_WLVerticalLayoutList;
    QNetworkAccessManager m_manager;
    QNetworkReply *m_reply;
};

#endif // MAINWINDOW_H
