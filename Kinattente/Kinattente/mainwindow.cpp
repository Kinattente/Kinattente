/*
 * @file    mainwindow.cpp
 * @author  Weber Arnold <kinattente@gmail.com>
 * @version 3.0
 *
 * @section DESCRIPTION
 *
 * Class that manages the display of the main window
 *
 */

#include "mainwindow.h"

/*
 * Constructeur
 *
 * Charge le fichier CSS
 * Lance la fonction de l'assistant de configuration (pour le premier lancement de Kinattente et peut-être après mise à jour)
 * Réinitialise les variables settings PendingAction (en cas de coupure du logiciel sans remise à zéro) et ClientIndex (change à chaque connexion)
 * Vérifie que l'assistant de configuration s'est bien lancé sinon lance une erreur récupérée dans le main pour fermer Kinattente
 * Lance la fonction de connexions à la BDD et au serveur si multiposte, vérifie qu'elles sont bien ouvertes  sinon lance une erreur récupérée dans le main pour fermer Kinattente
 * Lance  la fonction de création de la barre d'outils
 * Lance  la fonction de création des onglets Archives patients, éditeur de kinés et listes d'attente
 * Met l'onglet listes d'attente en onglet courant
 * Rempli le widget central du QMainWindow
 * Lance la fonction de connexion des boutons "Retirer" et "Modifier" de toutes les étiquettes
 * @param parent widget parent de la classe
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_problemOccurred(false)// Constructeur
{
    configAssistant();
    loadStyleSheet();

    m_settings.setValue("PendingAction", -1);
    m_settings.remove("ClientIndex");
    m_settings.remove("OnlyMySQLInstall");// Au cas ou un transfert s'est mal déroulé, on réinitialise le paramètre
    m_settings.remove("CloseEvent");

    if(!m_settings.value("ConfigAssistant").toBool())
    {
        error(101, this);
        m_problemOccurred = true;
    }
    else
    {
        if(!connections())
            m_problemOccurred = true;
        else
        {
            // Création de la Menu Bar et la Tool Bar
            createActions();

            // Création du QStackedWidget et remplissage de ses pages
            createArchivesEditor(index::Archives);
            createArchivesEditor(index::Editor);
            createWaitingList();
            m_stackedWidget.setCurrentIndex(2);
            setCentralWidget(&m_stackedWidget);

            // Retrait des plus anciens patients
            flushOldestPatients();

            // Connexion des boutons Retirer et Modifier
            createWithdrawalEditButtonsConnections();
        }
    }
}

MainWindow::~MainWindow()// Destructeur
{

}

/*
 * hasProblemOccurred
 *
 * Accesseur renseignant si les connexions à la BDD et au serveur sont opérationnelles
 * @return Bool renseignant l'état des connexions
 */
bool MainWindow::hasProblemOccurred()// Accesseur renseignant si les connexions à la BDD et au serveur sont opérationnelles
{
    return m_problemOccurred;
}

/*
 * configAssistant
 *
 * Méthode pour lancer une instance de la classe ConfigAssistant si l'assistant de configuration n'a jamais complété auparavant
 * Vérifier par le booléen de la variable setting "ConfigAssistant"
 */
void MainWindow::configAssistant()// Méthode pour lancer l'assistant de configuration
{
    if(!m_settings.value("ConfigAssistant", false).toBool())
    {
        ConfigAssistant *ConfigAssistant { new class ConfigAssistant };

        QEventLoop loop;
        connect(ConfigAssistant, &ConfigAssistant::closed, &loop, &QEventLoop::quit);
        connect(ConfigAssistant, &ConfigAssistant::finished, &loop, &QEventLoop::quit);
        ConfigAssistant->show();
        loop.exec();
    }
}

    /*
     * connections
     *
     * Méthode pour lancer les connexions à la BDD et au serveur dans le cas du multiposte
     * Appel la fonction connectToDatabase() et  teste son retour
     * Si monoposte : connecte la transmission des actualisations avec la réception en direct (sans passer par un serveur)
     * Si multiposte : lance le serveur si poste principal, puis se connecte en tant que client au serveur.
     *                 Connecte ensuite l'envoi des actualisations
     * @return Bool renseignant sur la réussite des deux connexions
     */
bool MainWindow::connections()// Méthode pour lancer la connexion à la BDD, le serveur et le client
{
    // Connexion à la DataBase
    m_settings.beginGroup("BDD");

    if(!connectToDatabase())
        return false;

    if(m_settings.value("Driver").toString() == "QSQLITE")
        connect(this, &MainWindow::refreshTransmission, this, &MainWindow::refreshReception);
    else
    {
        if(m_settings.value("MasterWorkplace").toBool())
        {
            m_Server = new Server(this);
            if(!m_Server->isServerListening())
                return false;
        }

        m_Client = new Client(this);
        connect(this, &MainWindow::refreshTransmission, m_Client, &Client::sendMessage);
        connect(m_Client, &Client::refreshReceived, this, &MainWindow::refreshReception);
        connect(m_Client, &Client::update, [this]() {launchUpdate(true); });
        m_Client->connectClient(m_settings.value("IPAddress").toString(), m_settings.value("Port").toInt());
        if(!m_Client->isClientConnected())
        {
            error(102, this);
            return false;
        }
    }
    m_settings.endGroup();
    return true;
}

void MainWindow::createActions()// Méthode pour créer la Menu Bar  et la Tool Bar
{
    QMenu *fileMenu { menuBar()->addMenu(tr("&Fichier")) };
    QToolBar *toolBar { addToolBar(tr("Actions")) };
        toolBar->setFloatable(false);
        toolBar->setMovable(false);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QIcon const addPatientIcon { QIcon("Images/AddPatient.png") };
    QAction *addPatientAction { new QAction(addPatientIcon, tr("Ajouter un &patient..."), this) };
    addPatientAction->setShortcut(QKeySequence("Ctrl+P"));
    addPatientAction->setStatusTip(tr("Ajouter un patient à la liste d'attente"));
    connect(addPatientAction, &QAction::triggered, [this]() {openPatientForm(); });
    fileMenu->addAction(addPatientAction);
    toolBar->addAction(addPatientAction);

    QIcon const addKineIcon { QIcon("Images/AddKine.png") };
    QAction *addKineAction { new QAction(addKineIcon, tr("Ajouter un &kiné..."), this) };
    addKineAction->setShortcut(QKeySequence("Ctrl+K"));
    addKineAction->setStatusTip(tr("Ajouter un nouveau kiné"));
    connect(addKineAction, &QAction::triggered, this, &MainWindow::openKineForm);
    fileMenu->addAction(addKineAction);
    toolBar->addAction(addKineAction);

    fileMenu->addSeparator();

    QIcon const openPatientsArchivesIcon { QIcon("Images/PatientsArchives.png") };
    QAction *openPatientsArchivesAction { new QAction(openPatientsArchivesIcon, tr("&Archives patients..."), this) };
    openPatientsArchivesAction->setShortcut(QKeySequence("Ctrl+A"));
    openPatientsArchivesAction->setStatusTip(tr("Accéder aux archives patients"));
    int index {0};
    connect(openPatientsArchivesAction, &QAction::triggered, [this, index]() {changePage(index); });
    m_toolButtons.append(new QToolButton);
    m_toolButtons.last()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolButtons.last()->setDefaultAction(openPatientsArchivesAction);

    QIcon const openKinesEditorIcon { QIcon("Images/KinesEditor.png") };
    QAction *openKinesEditorAction { new QAction(openKinesEditorIcon, tr("&Editer Kinés..."), this) };
    openKinesEditorAction->setShortcut(QKeySequence("Ctrl+E"));
    openKinesEditorAction->setStatusTip(tr("Gérer la liste des Kinés"));
    index = 1;
    connect(openKinesEditorAction, &QAction::triggered, [this, index]() {changePage(index); });
    m_toolButtons.append(new QToolButton);
    m_toolButtons.last()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolButtons.last()->setDefaultAction(openKinesEditorAction);

    QIcon const openWaitingListIcon { QIcon("Images/WaitingList.png") };
    QAction *openWaitingListAction { new QAction(openWaitingListIcon, tr("&Listes d'attentes..."), this) };
    openWaitingListAction->setShortcut(QKeySequence("Ctrl+L"));
    openWaitingListAction->setStatusTip(tr("Accéder aux listes d'attentes"));
    index = 2;
    connect(openWaitingListAction, &QAction::triggered, [this, index]() {changePage(index); });
    fileMenu->addAction(openWaitingListAction);
    fileMenu->addAction(openPatientsArchivesAction);
    fileMenu->addAction(openKinesEditorAction);
    m_toolButtons.append(new QToolButton);
    m_toolButtons.last()->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolButtons.last()->setDefaultAction(openWaitingListAction);
    m_toolButtons.last()->setStyleSheet("QToolButton { background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255)); color: white }"
                                        "QToolButton:hover { background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255)) }");
    toolBar->addWidget(m_toolButtons[2]);
    toolBar->addWidget(m_toolButtons[0]);
    toolBar->addWidget(m_toolButtons[1]);

    fileMenu->addSeparator();

    QAction *quitAction { new QAction(tr("&Quitter"), this) };
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    quitAction->setStatusTip(tr("Quitter Kinattente"));
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    fileMenu->addAction(quitAction);

    QMenu *menuOutils { menuBar()->addMenu(tr("&Outils")) };

    QIcon const optionsIcon { QIcon("Images/Options.png") };
    QAction *optionsAction { new QAction(optionsIcon, tr("Options..."), this) };
    optionsAction->setShortcut(QKeySequence("Ctrl+O"));
    optionsAction->setStatusTip(tr("Options"));
    connect(optionsAction, &QAction::triggered, this, &MainWindow::openOptions);
    menuOutils->addAction(optionsAction);

    QIcon const exportIcon { QIcon("Images/Export.png") };
    QAction *exportAction { new QAction(exportIcon, tr("&Exporter la Base de Données..."), this) };
    exportAction->setShortcut(QKeySequence("Ctrl+X"));
    exportAction->setStatusTip(tr("Exporter la BDD"));
    connect(exportAction, &QAction::triggered, [this] () {exportBDD(true); });
    menuOutils->addAction(exportAction);

    QIcon const importIcon { QIcon("Images/Import.png") };
    QAction *importAction { new QAction(importIcon, tr("&Importer la Base de Données..."), this) };
    importAction->setShortcut(QKeySequence("Ctrl+I"));
    importAction->setStatusTip(tr("Importer la BDD"));
    connect(importAction, &QAction::triggered, this, &MainWindow::importBDD);
    menuOutils->addAction(importAction);

    QMenu *helpMenu { menuBar()->addMenu(tr("&Aide")) };

    QAction *updateAction { new QAction(tr("Vérifier les &mises à jour..."), this) };
    updateAction->setShortcut(QKeySequence("Ctrl+M"));
    updateAction->setStatusTip(tr("Vérifier les mises à jour"));
    connect(updateAction, &QAction::triggered, this, &MainWindow::launchUpdate);
    helpMenu->addAction(updateAction);

    QAction *faqAction { new QAction(tr("&FAQ..."), this) };
    faqAction->setShortcut(QKeySequence("Ctrl+F"));
    faqAction->setStatusTip(tr("Foire aux questions"));
    connect(faqAction, &QAction::triggered, this, &MainWindow::faq);
    helpMenu->addAction(faqAction);

    QAction *aboutAction { new QAction(tr("À &Propos de Kinattente..."), this) };
    aboutAction->setShortcut(QKeySequence("Ctrl+D"));
    aboutAction->setStatusTip(tr("À Propos de Kinattente"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::launchAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createArchivesEditor(const index index)// Méthode pour créer la page contenant les archives patients
{
    // Création des objets
    QWidget *widget { new QWidget };
    QVBoxLayout *mainLayout { new QVBoxLayout };
    QHBoxLayout *buttonsLayout { new QHBoxLayout };
    m_AEModel.append(new CustomSqlModel(index));
    m_AEView.append(new QTableView);
    QFormLayout *AEForm { new QFormLayout };
    m_AELineEdit.append(new QLineEdit);
    m_AELabel.append(new QLabel);
    m_AEModelProxy.append(new QSortFilterProxyModel);
    QPushButton *AEButton { new QPushButton(index ? tr("Modifier le kiné") : tr("Désarchiver")) };

    // Reparentage / Intrication
    m_stackedWidget.addWidget(widget);
        widget->setLayout(mainLayout);
            mainLayout->addWidget(m_AEView[index]);
                m_AEModelProxy[index]->setSourceModel(m_AEModel[index]);
                    m_AEView[index]->setModel(m_AEModelProxy[index]);
            mainLayout->addLayout(AEForm);
                AEForm->addRow(tr("&Recherche :"), m_AELineEdit[index]);
            mainLayout->addWidget(m_AELabel[index], 1, Qt::AlignHCenter);
            mainLayout->addLayout(buttonsLayout);
            buttonsLayout->addWidget(AEButton, 1, (index ? Qt::AlignRight : Qt::AlignHCenter));
            if(index)
            {
                QPushButton *supprKineButton { new QPushButton(tr("Supprimer")) };
                buttonsLayout->addWidget(supprKineButton, 1, Qt::AlignLeft);
                connect(supprKineButton, &QPushButton::clicked, this, &MainWindow::supprKineButtonClicked);
            }

    m_AEView[index]->setSortingEnabled(true);

    QList<QStringList> headerTitle;
        headerTitle.append(QStringList() << tr("Lieu") << tr("Nom") << tr("Prénom") << tr("Age") << tr("Téléphone 1") << tr("Téléphone 2") << tr("Email") << tr("Kiné") << tr("Rue") << tr("Ville") << tr("Pathologie") << tr("Urgent") << tr("Date d'appel") << tr("ID"));
        headerTitle.append(QStringList() << tr("Nom") << tr("Prénom") << tr("Couleur") << tr("Actif") << tr("ID"));
    QStringList queries { QStringList() << (m_settings.value("BDD/Driver").toString() == "QSQLITE" ?
        "SELECT p.location, p.gender || ' ' || p.name AS p_name,        p.firstname, p.age, p.phone1, p.phone2, p.email, k.name || ' ' || k.firstname AS k_name,        p.address, c.name, p.pathology, p.emergency, p.call_date, p.id FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id" :
        "SELECT p.location, CONCAT_WS(' ', p.gender, p.name) AS p_name, p.firstname, p.age, p.phone1, p.phone2, p.email, CONCAT_WS(' ', k.name, k.firstname) AS k_name, p.address, c.name, p.pathology, p.emergency, p.call_date, p.id FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id")
                                        << "SELECT name, firstname, color, active_string, id FROM Kines WHERE name != 'AUCUN'" };
    m_AEModel[index]->setQuery(queries[index], QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    for(int i { 0 }; i < headerTitle[index].size(); i++)
        m_AEModel[index]->setHeaderData(i, Qt::Horizontal, headerTitle[index][i]);
    m_AEModelProxy[index]->sort((index ? 0 : 12), (index ? Qt::AscendingOrder : Qt::DescendingOrder));

    m_AEView[index]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// Les colonnes prennent toute la largeur de la page et se répartissent
    m_AEView[index]->setSelectionBehavior(QAbstractItemView::SelectRows);// On ne peut sélectionner que des lignes
    m_AEView[index]->setSelectionMode(QAbstractItemView::SingleSelection);
    m_AEView[index]->selectRow(0);
    m_AEView[index]->selectionModel()->setCurrentIndex(m_AEView[index]->indexAt(QPoint(0, 0)), QItemSelectionModel::NoUpdate);
    m_AELabel[index]->setFrameShadow(QFrame::Raised);
    m_AELabel[index]->setFrameShape(QFrame::StyledPanel);
    m_AELabel[index]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_AELabel[index]->setStyleSheet("padding: 10px");
    m_AELabel[index]->setText("<center><strong>" + m_AEModelProxy[index]->data(m_AEView[index]->currentIndex()).toString() + "</strong></center>");
    m_AEModelProxy[index]->setFilterKeyColumn(-1);
    m_AEModelProxy[index]->setFilterCaseSensitivity(Qt::CaseInsensitive);
    AEButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(m_AELineEdit[index], &QLineEdit::textEdited, m_AEModelProxy[index], &QSortFilterProxyModel::setFilterFixedString);
    connect(m_AEView[index], &QTableView::clicked, this, &MainWindow::clicSelection);
    connect(AEButton, &QPushButton::clicked, this, &MainWindow::clicAEButton);
}

void MainWindow::createWaitingList()// Méthode pour créer la page contenant les listes d'attente
{
    // Création du Widget, des Layouts et des QLabel
    QWidget *widget { new QWidget };
    QVBoxLayout *mainLayout { new QVBoxLayout };
    QHBoxLayout *WLLayout { new QHBoxLayout };
    QHBoxLayout *footerLayout { new QHBoxLayout };
    QList<QString> WLTitleText {tr("Cabinet"), tr("Domicile")};
    LinkedPicture *Logo { new LinkedPicture(QSize(50, 50), "Images/Logo.png", "https://kinattente.wordpress.com/") };

    // Intrication des Layouts
    m_stackedWidget.addWidget(widget);
        widget->setLayout(mainLayout);
        mainLayout->addLayout(WLLayout);
        mainLayout->addLayout(footerLayout);
            footerLayout->addLayout(&m_updateLayout, 100);
            footerLayout->setAlignment(&m_updateLayout, Qt::AlignHCenter);
                m_updateLayout.addWidget(&m_version, 0, Qt::AlignHCenter);
            footerLayout->addWidget(Logo, 0, Qt::AlignRight);

    // Vérifier l'existance d'une nouvelle mise à jour
    m_version.setText(tr("Version : ") + m_settings.value("Version").toString() + " (32Bits) - Kinattente est à jour");
    m_updatePicture = new LinkedPicture(QSize(15, 15), "Images/NoUpdate.png", "Update") ;
    m_updateLayout.addWidget(m_updatePicture, 0, Qt::AlignHCenter);

    QTimer::singleShot(200, [this] {refreshUpdate(true);});

    for(int i { 0 }; i < 2; i++)
    {
        m_WLVerticalLayoutList.append(new QVBoxLayout);
        m_WLList.append(new WaitingList(QString::number(i)));
        WLLayout->addLayout(m_WLVerticalLayoutList.last());
        m_WLTitleList.append(new WLTitle(WLTitleText[i], m_WLList.last()->waitingTimesInformations(0)));
            m_WLVerticalLayoutList.last()->addWidget(m_WLTitleList.last(), 0, Qt::AlignLeft);
            m_WLVerticalLayoutList.last()->addWidget(m_WLList.last());
        connect(m_WLList[i]->tabBar(), &QTabBar::currentChanged, [this, i](int index) {m_WLTitleList[i]->refreshTitle(m_WLList[i]->waitingTimesInformations(index)); });
    }
}

void MainWindow::flushOldestPatients()// Méthode pour retirer du logiciel les patients les plus anciens
{
    if(pendingAction())
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    QSqlQuery sql_EraseData(QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    sql_EraseData.prepare("DELETE FROM archived_patients WHERE call_date < :call_date");
    sql_EraseData.bindValue(":call_date", QDateTime::currentDateTime().addYears(- m_settings.value("DataDuration").toInt()).toString("yyyy-MM-dd 00:00:00"));

    if(!sql_EraseData.exec())
        error(103, this, sql_EraseData.lastError().text());
    else
        emit refreshTransmission("ArchivesEditor#Archives");
    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::refreshWaitingList(const enum actualisationType actualisationType, const int id_Patient, const enum waitingList destinationWLType, const enum waitingList originWLType)// Méthode pour actualiser les listes d'attente
{
    WaitingList &originWaitingList { *m_WLList[originWLType] };
    QStringList const researchField { originWaitingList.researchFields()};// Cette liste sera envoyé en paramètre à la recréation de la liste d'attente, elle contient les textes de recherche
    int tabIndex { originWaitingList.currentIndex() };
    int patientIndexPosition { 0 };// Position nécessaire pour scroller après l'actualisation

    if (actualisationType != KineEdit)// L'actualisation ne provient pas d'un ajout / édition de kiné
    {
        if(actualisationType == Withdrawal)
            patientIndexPosition = originWaitingList.patientIndex(id_Patient, tabIndex) - 1;// Le patient est retiré de la LA, on récupère donc la position du précédant dans la LA. Cas particulier s'il était premier => -1
        if(actualisationType == PatientEdit && originWLType != destinationWLType)// Changement de liste d'attente donc suppression de l'étiquette dans l'ancienne liste et MàJ du titre
        {
            originWaitingList.withdrawalLabel(id_Patient);
            m_WLTitleList[originWLType]->refreshTitle(originWaitingList.waitingTimesInformations(tabIndex));// Changement des délais d'attente de la LA d'origine pour les changements de LA
        }

        // Mise à jour des listes d'attente
        delete m_WLList[destinationWLType];// Suppression de la Liste d'Attente "d'arrivée"
        m_WLList.removeAt(destinationWLType);
        m_WLList.insert(destinationWLType, new WaitingList(QString::number(destinationWLType), researchField, this));// Création et insertion de la nouvelle Liste d'Attente avec remplissage du QlineEdit
        WaitingList &destinationWaitingList { *m_WLList[destinationWLType] };
        m_WLVerticalLayoutList[destinationWLType]->addWidget(&destinationWaitingList);
        connect(destinationWaitingList.tabBar(), &QTabBar::currentChanged, [this, destinationWLType, &destinationWaitingList](int index) {m_WLTitleList[destinationWLType]->refreshTitle(destinationWaitingList.waitingTimesInformations(index)); });

        if(actualisationType == Addition || originWLType != destinationWLType || actualisationType == Unarchiving)
            tabIndex = 0;// Onglet "Tous" par défaut pour les situations n'ayant pas d'onglet prédéfini
        if(actualisationType != Withdrawal)
        {
            destinationWaitingList.setCurrentIndex(tabIndex);
            patientIndexPosition = destinationWaitingList.patientIndex(id_Patient, tabIndex);
            changePage(2);// Affichage des Listes d'Attentes si on est dans les archives
        }

        m_WLTitleList[destinationWLType]->refreshTitle(destinationWaitingList.waitingTimesInformations(destinationWaitingList.currentIndex()));
        qApp->processEvents();// Mise à jour des events pour que la géométrie des widgets soit fixée et pouvoir ainsi dérouler le QScrollArea

        // Scroll et clignotement
        if(patientIndexPosition != -1)// Exclusion des patients ayant quitté l'onglet affiché par retrait ou changement de kiné. Valeur -1 obtenue par indexOf() dans patientIndex() ou par attribution d'un -1 au début de la fonction
        {
            destinationWaitingList.scrollListToSelectedPatient(tabIndex, patientIndexPosition);
            if(actualisationType != Withdrawal)
            {
                QList<QList<Label*>*> const labels { destinationWaitingList.labels() };
                (*labels[tabIndex])[patientIndexPosition]->highlighteningMargins();
                cursor().setPos(cursor().pos().x() + 1, cursor().pos().y() + 1);// Je ne sais pas pourquoi mais le clignotement ne se fait que si la souris bouge, alors je la fait bouger
                for(int i { 100 }; i < 1200; i += 100)
                {
                    if((i / 100) % 2 != 0)
                        QTimer::singleShot(i, (*labels[tabIndex])[patientIndexPosition], &Label::originalMarginsColor);
                    else
                        QTimer::singleShot(i, (*labels[tabIndex])[patientIndexPosition], &Label::highlighteningMargins);
                }
            }
        }
    }
    else
    {// Ajout / édition de kiné
        // Mise à jour des 2 listes d'attente
        for(int i { 0 }; i < 2; i++)
        {
            delete m_WLList[i];
            m_WLList.removeAt(i);
            m_WLList.insert(i, new WaitingList(QString::number(i)));
            m_WLVerticalLayoutList[i]->addWidget(m_WLList[i]);
            m_WLTitleList[i]->refreshTitle(m_WLList[i]->waitingTimesInformations(0));// Changement des délais d'attente du titre en fonction de l'onglet affiché
            connect(m_WLList[i]->tabBar(), &QTabBar::currentChanged, [this, i](int index) {m_WLTitleList[i]->refreshTitle(m_WLList[i]->waitingTimesInformations(index)); });
        }
    }

    // Connexion des boutons Retirer et Modifier
    createWithdrawalEditButtonsConnections();
}

void MainWindow::refreshArchivesEditor(const enum index index)// Méthode pour actualiser les archives patients et l'éditeur de kinés
{
    QStringList const queries { QStringList() << (m_settings.value("BDD/Driver").toString() == "QSQLITE" ?
        "SELECT p.location, p.gender || ' ' || p.name AS p_name,        p.firstname, p.age, p.phone1, p.phone2, p.email, k.name || ' ' || k.firstname AS k_name,        p.address, c.name, p.pathology, p.emergency, p.call_date, p.id FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id" :
        "SELECT p.location, CONCAT_WS(' ', p.gender, p.name) AS p_name, p.firstname, p.age, p.phone1, p.phone2, p.email, CONCAT_WS(' ', k.name, k.firstname) AS k_name, p.address, c.name, p.pathology, p.emergency, p.call_date, p.id FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id")
                                        << "SELECT name, firstname, color, active_string, id FROM Kines WHERE name != 'AUCUN'" };
    m_AEModel[index]->setQuery(queries[index], QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    m_AEView[index]->selectRow(0);
    m_AEView[index]->selectionModel()->setCurrentIndex(m_AEView[index]->indexAt(QPoint(0, 0)), QItemSelectionModel::NoUpdate);
    m_AELabel[index]->setText("<center><strong>" + m_AEModelProxy[index]->data(m_AEView[index]->currentIndex()).toString() + "</strong></center>");
}

void MainWindow::changePage(const int index)// Slot pour changer de page
{
    for(int i { 0 }; i < 3; i++)
    {
        if(index == i)
            m_toolButtons[i]->setStyleSheet("QToolButton { background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255)); color: white }"
                                                "QToolButton:hover { background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255)) }");
        else
            m_toolButtons[i]->setStyleSheet("QToolButton { background-color: rgb(255,255,255) }"
                                                "QToolButton:hover { background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255)) }");
    }

    m_stackedWidget.setCurrentIndex(index);
    if(index != 2)
        m_AELineEdit[index]->setFocus();
}

void MainWindow::createWithdrawalEditButtonsConnections()// Slot pour créer les connexions des boutons retirer et modifier des etiquettes et la connexion entre le champ de recherche et l'acutalisation de la liste correspondante
{
    for(auto const WaitingList : qAsConst(m_WLList))// Parcourt les 2 listes d'attentes
    {
        WaitingList->disconnect();
        connect(WaitingList, &WaitingList::refresh, this, &MainWindow::createWithdrawalEditButtonsConnections);
        for(auto const onglet : WaitingList->labels())// Parcourt chaque onglet de la liste d'attente parcouru
        {
            for(auto const label : *onglet)// Parcourt chaque étiquette de l'onglet pourcouru
            {
                label->disconnect();
                connect(label, &Label::withdrawalPatient, this, &MainWindow::withdrawalPatient);
                connect(label, &Label::editPatient, this, &MainWindow::openPatientForm);
            }
        }
    }
}

void MainWindow::openPatientForm(const int id)// Slot pour ouvrir le formulaire d'ajout de patient
{
    if(pendingAction())
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    PatientForm *PatientForm { new class PatientForm(id, this) };
        PatientForm->setWindowTitle(tr("Formulaire Patient"));
        PatientForm->setMinimumSize(100,100);// Pour éviter le "setGeometry: Unable to set geometry"
    connect(PatientForm, &PatientForm::refreshTransmission, this, &MainWindow::refreshTransmission);// Ajout / Suppression de Ville
    PatientForm->exec();

    if(!PatientForm->accessor().isEmpty())
        emit refreshTransmission("WaitingList#" + PatientForm->accessor()["actualisationType"] + "#" + PatientForm->accessor()["id"] + "#" + PatientForm->accessor()["WaitingList"] + "#" + PatientForm->accessor()["OldWaitingList"]);// Envoi de l'info d'actualisation aux autres pc

    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::openKineForm(const int id)// Slot pour ouvrir le formulaire d'ajout de kiné
{
    if(pendingAction())
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    KineForm *KineForm { new class KineForm(id, this) };
        KineForm->setWindowTitle("Formulaire Kiné");
        KineForm->setMinimumSize(50,50);// Pour éviter le "setGeometry: Unable to set geometry"
    KineForm->exec();

    if(KineForm->result())
    {
        emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
        emit refreshTransmission("ArchivesEditor#Editor");
    }

    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::withdrawalPatient(int id)// Slot pour retirer un patient de la liste
{
    if(pendingAction())
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    QSqlQuery sql_withdrawalPatient(QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    sql_withdrawalPatient.prepare("SELECT location, gender, name, firstname FROM Active_Patients WHERE id = :id");
    sql_withdrawalPatient.bindValue(":id", id);
    if(!sql_withdrawalPatient.exec())
    {
        error(104, this, sql_withdrawalPatient.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    sql_withdrawalPatient.first();
    QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };
    bool const location { sql_withdrawalPatient.value(0).toBool() };
    QString const gender { genderList[sql_withdrawalPatient.value(1).toInt()] };
    QString const patientName { sql_withdrawalPatient.value(2).toString() + " " + sql_withdrawalPatient.value(3).toString() };
    if(QMessageBox::question(this, tr("Archivage"), tr("Voulez-vous vraiment retirer <strong>") + gender + " " + patientName + tr("</strong> de la liste d'attente ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlDatabase::database().transaction();
    sql_withdrawalPatient.prepare("INSERT INTO Archived_Patients (id, location, gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, emergency, call_date, archiving_date)"
                               "SELECT NULL, replace(replace(location, 0, 'Office'), 1, 'Home'), gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, replace(replace(no_emergency, 0, 'Oui'), 1, 'Non'), call_date, CURRENT_TIMESTAMP FROM Active_Patients WHERE id = :id");
    sql_withdrawalPatient.bindValue(":id", id);
    if(!sql_withdrawalPatient.exec())
    {
        QSqlDatabase::database().rollback();
        error(105, this, sql_withdrawalPatient.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    sql_withdrawalPatient.prepare("DELETE FROM Active_Patients WHERE id = :id");
    sql_withdrawalPatient.bindValue(":id", id);
    if(!sql_withdrawalPatient.exec())
    {
        QSqlDatabase::database().rollback();
        error(106, this, sql_withdrawalPatient.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlDatabase::database().commit();
    QMessageBox::information(this, tr("Archivage"), tr("Le patient <strong>") + patientName + tr("</strong> a été archivé avec succès !"));
    emit refreshTransmission("ArchivesEditor#Archives");
    emit refreshTransmission("WaitingList#Withdrawal#" + QString::number(id) + "#" + (location ? "Home#Home" : "Office#Office"));
    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::clicSelection(const QModelIndex &index)// Slot pour afficher la sélection
{
    const int currentIndex { m_stackedWidget.currentIndex() };
    QVariant const currentData { m_AEModelProxy[currentIndex]->data(index) };
    m_AELabel[currentIndex]->setText("<center><strong>" + currentData.toString() + "</strong></center>");
}

void MainWindow::clicAEButton()// Slot pour désarchiver la sélection ou modifier un kiné
{
    if(pendingAction() || m_AEView[m_stackedWidget.currentIndex()]->rowAt(0) == -1)
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    QItemSelectionModel const *selectionModel { m_AEView[m_stackedWidget.currentIndex()]->selectionModel() };
    QModelIndexList const selectedIndexes { selectionModel->selectedIndexes() };
    QList<QVariant> currentDataList;
    for(auto const &indexValeur : qAsConst(selectedIndexes))
        currentDataList.append(m_AEModelProxy[m_stackedWidget.currentIndex()]->data(indexValeur));

    if(m_stackedWidget.currentIndex())
    {
        openKineForm(currentDataList[4].toInt());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlQuery sql_patientUnarchiving(QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    sql_patientUnarchiving.prepare("SELECT location, gender, name, firstname FROM Archived_Patients WHERE id = :id");
    sql_patientUnarchiving.bindValue(":id", currentDataList[13].toInt());
    if(!sql_patientUnarchiving.exec())
    {
        emit refreshTransmission("PendingAction#-1");
        error(107, this, sql_patientUnarchiving.lastError().text());
        return;
    }

    sql_patientUnarchiving.first();
    QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };
    QString location { sql_patientUnarchiving.value(0).toString() };
    QString gender { genderList[sql_patientUnarchiving.value(1).toInt()] };
    QString patientName { sql_patientUnarchiving.value(2).toString() + " " + sql_patientUnarchiving.value(3).toString() };
    if(QMessageBox::question(this, tr("Désarchivage"), tr("Voulez-vous vraiment désarchiver le patient <strong>") + gender + " " + patientName + tr("</strong> de la liste d'attente ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlDatabase::database().transaction();
    sql_patientUnarchiving.prepare("INSERT INTO Active_Patients (id, location, gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, no_emergency, call_date)"
                               "SELECT NULL, replace(replace(location, 'Office', 0), 'Home', 1), gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, replace(replace(emergency, 'Oui', 0), 'Non', 1), call_date FROM Archived_Patients WHERE id = :id");
    sql_patientUnarchiving.bindValue(":id", currentDataList[13].toInt());
    if(!sql_patientUnarchiving.exec())
    {
        QSqlDatabase::database().rollback();
        error(108, this, sql_patientUnarchiving.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QString const id { sql_patientUnarchiving.lastInsertId().toString() };
    sql_patientUnarchiving.prepare("DELETE FROM Archived_Patients WHERE id = :id");
    sql_patientUnarchiving.bindValue(":id", currentDataList[13].toInt());
    if(!sql_patientUnarchiving.exec())
    {
        QSqlDatabase::database().rollback();
        error(109, this, sql_patientUnarchiving.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlDatabase::database().commit();
    QMessageBox::information(this, tr("Désarchivage"), tr("Le patient <strong>") + patientName + tr("</strong> a été désarchivé avec succès !"));
    emit refreshTransmission("WaitingList#Unarchiving#" + id + "#" + (location == "Home" ? "Home#Office" : "Office#Office"));
    emit refreshTransmission("ArchivesEditor#Archives");
    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::supprKineButtonClicked()// Slot pour supprimer un kiné
{
    if(pendingAction() || m_AEView[m_stackedWidget.currentIndex()]->rowAt(0) == -1)
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    QItemSelectionModel const *selectionModel { m_AEView[m_stackedWidget.currentIndex()]->selectionModel() };
    QModelIndexList const selectedIndexes { selectionModel->selectedIndexes() };
    QList<QVariant> currentDataList;
    for(auto const &modelIndex : selectedIndexes)
        currentDataList.append(m_AEModelProxy[m_stackedWidget.currentIndex()]->data(modelIndex));

    if(QMessageBox::question(this, tr("Suppression Kiné"), tr("Etes-vous sur de vouloir supprimer le kiné <strong>") + currentDataList[0].toString() + " " + currentDataList[1].toString() + "</strong> ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlQuery sql_KineSuppr(QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
    QSqlDatabase::database().transaction();
    sql_KineSuppr.prepare("UPDATE Active_Patients SET kine_id = (SELECT id FROM Kines WHERE name = 'AUCUN') WHERE kine_id = :kine_id");
    sql_KineSuppr.bindValue(":kine_id", currentDataList[4].toInt());
    if(!sql_KineSuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(110, this, sql_KineSuppr.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    sql_KineSuppr.prepare("UPDATE Archived_Patients SET kine_id = (SELECT id FROM Kines WHERE name = 'AUCUN') WHERE kine_id = :kine_id");
    sql_KineSuppr.bindValue(":kine_id", currentDataList[4].toInt());
    if(!sql_KineSuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(111, this, sql_KineSuppr.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    sql_KineSuppr.prepare("DELETE FROM Kines WHERE id = :kine_id");
    sql_KineSuppr.bindValue(":kine_id", currentDataList[4].toInt());
    if(!sql_KineSuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(112, this, sql_KineSuppr.lastError().text());
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QSqlDatabase::database().commit();
    QMessageBox::information(this, tr("Suppression Effectuée"), tr("Le kiné <strong>") + currentDataList[0].toString() + " " + currentDataList[1].toString() + tr("</strong> a bien été supprimé."));
    emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
    emit refreshTransmission("ArchivesEditor#Archives");
    emit refreshTransmission("ArchivesEditor#Editor");
    emit refreshTransmission("PendingAction#-1");
}

void MainWindow::openOptions()// Slot pour ouvrir les options
{
    Options *Options { new class Options(this) };
        Options->setWindowTitle(tr("Options - Kinattente"));
    connect(Options, &Options::closed, [this]() {m_settings.setValue("CloseEvent", "Option");close(); });
    connect(Options, &Options::ipChanged, this, &MainWindow::ipChanged);
    connect(Options, &Options::css, this, &MainWindow::loadStyleSheet);
    connect(Options, &Options::exportBDD, this, &MainWindow::exportBDD);
    connect(Options, &Options::refreshTransmission, this, &MainWindow::refreshTransmission);
        Options->exec();
}

void MainWindow::ipChanged()// Slot pour reconnecter à la Database quand l'IP a changé
{
    if(!connectToDatabase())
    {
        m_settings.setValue("CloseEvent", "IpChanged");
        close();
        return;
    }

    emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
    emit refreshTransmission("ArchivesEditor#Editor");
    emit refreshTransmission("ArchivesEditor#Archives");
}

bool MainWindow::exportBDD(bool mandatoryBackup)// Slot pour sauvegarder la BDD
{
    if((!QDir(m_settings.value("BDD/SavePath").toString()).exists() || m_settings.value("BDD/SavePath").toString().isEmpty()) && !mandatoryBackup)
    {
        error(113, this);
        return false;
    }

    QString const fileName { (m_settings.value("BDD/Driver").toString() == "QSQLITE" ? "/data.sqlite" : "/data.sql") };
    QString savePath;
    if(!QDir(m_settings.value("BDD/SavePath").toString()).exists() || m_settings.value("BDD/SavePath").toString().isEmpty() || mandatoryBackup)
    {
        QString const dir { m_settings.value("BDD/SavePath").toString().isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) : m_settings.value("BDD/SavePath").toString() };
        QString const extension { (m_settings.value("BDD/Driver").toString() == "QSQLITE" ? "Sauvegarde (*.sqlite)" : "Sauvegarde (*.sql)") };
        savePath = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), dir, extension);
        if(savePath.isEmpty())
            return false;

    }
    else
        savePath = m_settings.value("BDD/SavePath").toString() + fileName;

    if(m_settings.value("BDD/Driver").toString() == "QSQLITE")
    {
        if(!QFile::exists(savePath))
        {
            if(!QFile::copy(m_settings.value("BDD/DatabaseName").toString(), savePath))
            {
                error(114, this);
                return false;
            }
            else
            {
                QMessageBox::information(this, tr("Exportation terminée"), tr("<br/><strong>Exportation de la base de données réussie !</strong>"));
                return true;
            }
        }

        QString oldSavePath { savePath };
        oldSavePath.replace(".sqlite", "_old_save.sqlite");
        if(QFile::exists(oldSavePath))
            if(!QFile::remove(oldSavePath))
            {
                error(115, this);
                return false;
            }

        if(!QFile::rename(savePath, oldSavePath))
        {
            error(116, this);
            return false;
        }

        if(!QFile::copy(m_settings.value("BDD/DatabaseName").toString(), savePath) && QFileInfo(savePath).size() != 0)
        {
            error(117, this);
            return false;
        }

        QMessageBox::information(this, tr("Exportation terminée"), tr("<br/><strong>Exportation de la base de données réussie !</strong>"));
        if(QFile::exists(oldSavePath))
        {
            if(!QFile::remove(oldSavePath))
            {
                error(118, this);
                return false;
            }
        }
        return true;
    }

    // BDD/Driver == "QMYSQL"
    QStringList backup;
        backup << "--host=" + m_settings.value("BDD/IPAddress").toString() << "--user=root" << "--password=" + m_settings.value("BDD/PasswordRootMySQL").toString() << "--opt" << m_settings.value("BDD/DatabaseName").toString();
    QProcess myProcess;
        myProcess.setProgram(m_settings.value("BDD/MysqlBinDirPath").toString() + "/mysqldump");
        myProcess.setStandardOutputFile(savePath);
        myProcess.setArguments(backup);
        myProcess.start();
        myProcess.waitForFinished();

    if(!QFile::exists(savePath))
    {
        error(119, this);
        return false;
    }

    // Il faut rajouter "USE Kinattente" au fichier
    QFile saveFile(savePath);
        saveFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux1(&saveFile);
    QString saveText { flux1.readAll() };
        saveFile.close();
        saveText.prepend("USE Kinattente;\n");

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        error(120, this);
        return false;
    }

    QTextStream flux2(&saveFile);
        flux2 << saveText;
    saveFile.close();
    QMessageBox::information(this, tr("Exportation terminée"), tr("<br/><strong>Exportation de la base de données réussie !</strong>"));
    return true;
}

void MainWindow::importBDD()// Slot pour restaurer la BDD
{
    if(pendingAction())
        return;

    emit refreshTransmission("PendingAction#" + m_settings.value("ClientIndex").toString());

    if(QMessageBox::question(this, tr("Importer une sauvegarde existante"), tr("<strong>Voulez-vous vraiment importer la sauvegarde existante d'une base de données ?</strong><br/>"
                                                                               "Celle-ci écrasera les données actuellement présentes dans le logiciel.<br/>"
                                                                               "Je vous recommande avant d'importer quoi que ce soit d'aller faire une exportation de la base de données actuelle !"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    QString const extension { (m_settings.value("BDD/Driver").toString() == "QSQLITE" ? "Sauvegarde (*.sqlite)" : "Sauvegarde (*.sqlite *.sql)") };
    QString const saveFilePath { QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), extension) };

    if(saveFilePath.isEmpty())
    {
        emit refreshTransmission("PendingAction#-1");
        return;
    }

    if(m_settings.value("BDD/Driver").toString() == "QSQLITE")
    {
        if(saveFilePath == "data.sqlite" || saveFilePath == m_settings.value("BDD/DatabaseName").toString())
        {
            error(121, this);
            emit refreshTransmission("PendingAction#-1");
            return;
        }

        QString oldSavePath { m_settings.value("BDD/DatabaseName").toString().replace(".sqlite", "_old_save.sqlite") };
        if(QFile::exists(m_settings.value("BDD/DatabaseName").toString()))
        {
            if(QFile::exists(oldSavePath))
                if(!QFile::remove(oldSavePath))
                {
                    error(122, this);
                    emit refreshTransmission("PendingAction#-1");
                    return;
                }

            removeDatabase(m_settings.value("BDD/Driver").toString());
            if(!QFile::rename(m_settings.value("BDD/DatabaseName").toString(), oldSavePath))
            {
                error(123, this);
                emit refreshTransmission("PendingAction#-1");
                return;
            }
        }

        if(!QFile::copy(saveFilePath, m_settings.value("BDD/DatabaseName").toString()))
        {
            error(124, this);
            emit refreshTransmission("PendingAction#-1");
            return;
        }

        if(QFile::exists(oldSavePath))
        {
            if(!QFile::remove(oldSavePath))
            {
                error(125, this);
                emit refreshTransmission("PendingAction#-1");
                return;
            }
        }

        connectToDatabase();
    }
    else
    {
        // BDD/Driver == "QMYSQL"
        QStringList backup;
            backup << "--host=" + m_settings.value("BDD/IPAddress").toString() << "--user=root" << "--password=" + m_settings.value("BDD/PasswordRootMySQL").toString() << m_settings.value("BDD/DatabaseName").toString();
        if(saveFilePath.contains(QRegExp("sql$")))
        {// Importation sur MySQL d'un fichier sql
            QProcess myProcess;
                myProcess.setProgram(m_settings.value("BDD/MysqlBinDirPath").toString() + "/mysql");
                myProcess.setStandardInputFile(saveFilePath);
                myProcess.setArguments(backup);
                myProcess.start();
                myProcess.waitForFinished();
        }
        else
        {// Importation sur MySQL d'un fichier sqlite
            QProcess myProcess;
                myProcess.setProgram(m_settings.value("BDD/MysqlBinDirPath").toString() + "/mysql");
                myProcess.setStandardInputFile(QDir::currentPath() + "/FillUpKinattente.sql");
                myProcess.setArguments(backup);
                myProcess.start();
                myProcess.waitForFinished();

            QMessageBox::information(this, tr("Création Kinattente"), tr("<br/><strong>Création de la Database Kinattente réussie...</strong>"));

            QSqlDatabase dbSQLite { QSqlDatabase::addDatabase("QSQLITE", "QSQLITE") };
                dbSQLite.setDatabaseName(saveFilePath);

            if(!dbSQLite.open())
            {
                error(126, this);
                emit refreshTransmission("PendingAction#-1");
                return;
            }

            qDebug() << "Database SQLite opened successfully!";
            QMessageBox::information(this, tr("Connexion SQLite"), tr("<br/><strong>Connexion à la Database SQLite réussie...</strong>"));

            QSqlQuery sql_Export(dbSQLite);
            QSqlQuery sql_Import(QSqlDatabase::database(m_settings.value("BDD/Driver").toString()));
            QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).transaction();

            int i { 1 };
            QMap<int, QString> kines_id;
            QMap<int, QString> cities_id;

            sql_Export.prepare("SELECT name, firstname, color, active, active_string FROM Kines ORDER BY id");
            if(!sql_Export.exec())
            {
                QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                error(127, this, sql_Export.lastError().text());
                emit refreshTransmission("PendingAction#-1");
                return;
            }

            while (sql_Export.next())
            {
                kines_id.insert(i++, sql_Export.value(0).toString() + " " + sql_Export.value(1).toString());
                if(sql_Export.value(0).toString() != "AUCUN")
                {
                    sql_Import.prepare("INSERT INTO Kines (name, firstname, color, active, active_string) VALUES (:name, :firstname, :color, :active, :active_string)");
                    sql_Import.bindValue(":name", sql_Export.value(0).toString());
                    sql_Import.bindValue(":firstname", sql_Export.value(1).toString());
                    sql_Import.bindValue(":color", sql_Export.value(2).toString());
                    sql_Import.bindValue(":active", sql_Export.value(3).toBool());
                    sql_Import.bindValue(":active_string", sql_Export.value(4).toString());
                    if(!sql_Import.exec())
                    {
                        QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                        error(128, this, sql_Import.lastError().text());
                        emit refreshTransmission("PendingAction#-1");
                        return;
                    }
                }
            }

            QMessageBox::information(this, tr("Transfert des données Kinés"), tr("<br/><strong>Transfert des données Kinés réussi...</strong>"));

            i = 1;
            sql_Export.prepare("SELECT name FROM Cities ORDER BY id");
            if(!sql_Export.exec())
            {
                QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                error(129, this, sql_Export.lastError().text());
                emit refreshTransmission("PendingAction#-1");
                return;
            }

            while(sql_Export.next())
            {
                cities_id.insert(i++, sql_Export.value(0).toString());
                if(sql_Export.value(0).toString() != " ")
                {
                    sql_Import.prepare("INSERT INTO Cities (name) VALUES (:name)");
                    sql_Import.bindValue(":name", sql_Export.value(0).toString());
                    if(!sql_Import.exec())
                    {
                        QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                        error(130, this, sql_Import.lastError().text());
                        emit refreshTransmission("PendingAction#-1");
                        return;
                    }
                }
            }

            QMessageBox::information(this, tr("Transfert des données Villes"), tr("<br/><strong>Transfert des données Villes réussi...</strong>"));

            sql_Export.prepare("SELECT p.location, p.gender, p.age, p.name, p.firstname, p.phone1, p.phone2, p.email, p.doctor, k.name, k.firstname, p.availability, p.address, c.name, p.pathology, p.no_emergency, p.call_date FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id ORDER BY p.id");
            if(!sql_Export.exec())
            {
                QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                error(131, this, sql_Export.lastError().text());
                emit refreshTransmission("PendingAction#-1");
                return;
            }

            while (sql_Export.next())
            {
                cities_id.insert(i++, sql_Export.value(0).toString());
                sql_Import.prepare("INSERT INTO Active_Patients (location, gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, no_emergency, call_date) VALUES (:location, :gender, :age, :name, :firstname, :phone1, :phone2, :email, :doctor, :kine_id, :availability, :address, :city_id, :pathology, :no_emergency, :call_date)");
                sql_Import.bindValue(":location", sql_Export.value(0).toInt());
                sql_Import.bindValue(":gender", sql_Export.value(1).toInt());
                sql_Import.bindValue(":age", sql_Export.value(2).toInt());
                sql_Import.bindValue(":name", sql_Export.value(3).toString());
                sql_Import.bindValue(":firstname", sql_Export.value(4).toString());
                sql_Import.bindValue(":phone1", sql_Export.value(5).toString());
                sql_Import.bindValue(":phone2", sql_Export.value(6).toString());
                sql_Import.bindValue(":email", sql_Export.value(7).toString());
                sql_Import.bindValue(":doctor", sql_Export.value(8).toString());
                sql_Import.bindValue(":kine_id", kines_id.key(sql_Export.value(9).toString() + " " + sql_Export.value(10).toString()));
                sql_Import.bindValue(":availability", sql_Export.value(11).toString());
                sql_Import.bindValue(":address", sql_Export.value(12).toString());
                sql_Import.bindValue(":city_id", cities_id.key(sql_Export.value(13).toString()));
                sql_Import.bindValue(":pathology", sql_Export.value(14).toString());
                sql_Import.bindValue(":no_emergency", sql_Export.value(15).toInt());
                sql_Import.bindValue(":call_date", sql_Export.value(16).toDateTime().toString("yyyy-MM-dd 00:00:00"));
                if(!sql_Import.exec())
                {
                    QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                    error(132, this, sql_Import.lastError().text());
                    emit refreshTransmission("PendingAction#-1");
                    return;
                }
            }

            QMessageBox::information(this, tr("Transfert des données Patients Actifs"), tr("<br/><strong>Transfert des données Patients Actifs réussi...</strong>"));

            sql_Export.prepare("SELECT p.location, p.gender, p.age, p.name, p.firstname, p.phone1, p.phone2, p.email, p.doctor, k.name, k.firstname, p.availability, p.address, c.name, p.pathology, p.emergency, p.call_date, p.archiving_date FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id ORDER BY p.id");
            if(!sql_Export.exec())
            {
                QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                error(133, this, sql_Export.lastError().text());
                emit refreshTransmission("PendingAction#-1");
                return;
            }

            while (sql_Export.next())
            {
                cities_id.insert(i++, sql_Export.value(0).toString());
                sql_Import.prepare("INSERT INTO Archived_Patients (location, gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, emergency, call_date, archiving_date) VALUES (:location, :gender, :age, :name, :firstname, :phone1, :phone2, :email, :doctor, :kine_id, :availability, :address, :city_id, :pathology, :emergency, :call_date, :archiving_date)");
                sql_Import.bindValue(":location", sql_Export.value(0).toInt());
                sql_Import.bindValue(":gender", sql_Export.value(1).toInt());
                sql_Import.bindValue(":age", sql_Export.value(2).toInt());
                sql_Import.bindValue(":name", sql_Export.value(3).toString());
                sql_Import.bindValue(":firstname", sql_Export.value(4).toString());
                sql_Import.bindValue(":phone1", sql_Export.value(5).toString());
                sql_Import.bindValue(":phone2", sql_Export.value(6).toString());
                sql_Import.bindValue(":email", sql_Export.value(7).toString());
                sql_Import.bindValue(":doctor", sql_Export.value(8).toString());
                sql_Import.bindValue(":kine_id", kines_id.key(sql_Export.value(9).toString() + " " + sql_Export.value(10).toString()));
                sql_Import.bindValue(":availability", sql_Export.value(11).toString());
                sql_Import.bindValue(":address", sql_Export.value(12).toString());
                sql_Import.bindValue(":city_id", cities_id.key(sql_Export.value(13).toString()));
                sql_Import.bindValue(":pathology", sql_Export.value(14).toString());
                sql_Import.bindValue(":emergency", sql_Export.value(15).toInt());
                sql_Import.bindValue(":call_date", sql_Export.value(16).toDateTime().toString("yyyy-MM-dd 00:00:00"));
                sql_Import.bindValue(":archiving_date", sql_Export.value(17).toDateTime().toString("yyyy-MM-dd 00:00:00"));
                if(!sql_Import.exec())
                {
                    QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).rollback();
                    error(134, this, sql_Import.lastError().text());
                    emit refreshTransmission("PendingAction#-1");
                    return;
                }
            }

            QMessageBox::information(this, tr("Transfert des données Patients Archivés"), tr("<br/><strong>Transfert des données Patients Archivés réussi...</strong>"));
            QSqlDatabase::database(m_settings.value("BDD/Driver").toString()).commit();
            removeDatabase("QSQLITE");
        }
    }

QMessageBox::information(this, tr("Importation terminée"), tr("<br/><strong>Importation de base de données réussie !</strong>"));
emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
emit refreshTransmission("ArchivesEditor#Editor");
emit refreshTransmission("ArchivesEditor#Archives");
emit refreshTransmission("PendingAction#-1");
}

void MainWindow::faq()// Slot pour ouvrir la FAQ du site internet
{
    QDesktopServices::openUrl(QUrl("https://kinattente.wordpress.com/faq"));
}

void MainWindow::launchAbout()// Slot pour ouvrir la fenêtre à propos
{
    QString display;
        display.append("Version : " + m_settings.value("Version").toString() + " (32Bits)<br/><br/><br/><br/>");
        display.append("Kinattente est un logiciel libre conçu par un kiné passionné qui vous veut du bien.<br/><br/>");
        display.append("Vous souhaitez aider ? Ou en savoir plus ? rendez-vous sur le site internet de Kinattente : <br/><br/>");
        display.append("<a href='https://kinattente.wordpress.com/'>https://kinattente.wordpress.com/</a>");
    CustomWidget *About { new class CustomWidget(display) };
    About->setWindowTitle(tr("À Propos de Kinattente"));
    About->setMinimumSize(100,100);// Pour éviter le "setGeometry: Unable to set geometry"
    About->exec();
}

QString MainWindow::downloadInfoFromWebsite(QRegExp pattern)// Slot pour récupérer une info sur le site internet
{
    m_reply = m_manager.get(QNetworkRequest(QUrl("https://kinattente.wordpress.com/data/")));
    Q_ASSERT(m_reply);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, [this] { QMessageBox::information(this, tr("Temps d'attente dépassé"), tr("Le logiciel n'a pas pu se connecter afin vérifier la dernière version de Kinattente. Sans doute n'êtes vous pas conencté à internet."));});
    timer.start(10000);
    loop.exec();

    if(m_reply->error() != QNetworkReply::NoError)
    {
        error(135, this, m_reply->errorString());
        return QString();
    }

    QString data(m_reply->readAll());
    m_reply->deleteLater();
    return data.replace(pattern, R"(\1)");
}

void MainWindow::refreshUpdate(bool softwareOpening)// Slot pour actualiser l'affichage d'une mise à jour potentielle
{
    QSettings settings;
    QString const lastVersion(downloadInfoFromWebsite(QRegExp(R"(^.*Dernière version de Kinattente  : \#(\d+(\.\d+)+)\#.*$)")));
    if(lastVersion != settings.value("LastVersion").toString())
    {
        settings.setValue("LastVersion", lastVersion);
        settings.setValue("lastVersionScreen", true);
    }

    if (compareVersion(lastVersion))
    {
        m_version.setText(tr("Version : ") + m_settings.value("Version").toString() + " (32Bits) - Une mise à jour est disponible !");

        delete m_updatePicture;
        m_updatePicture = new LinkedPicture(QSize(20, 20), "Images/Update.png", "Update");
        m_updateLayout.addWidget(m_updatePicture, 0, Qt::AlignHCenter);
        connect(m_updatePicture, &LinkedPicture::launchUpdate, this, &MainWindow::launchUpdate);

        if(softwareOpening && settings.value("lastVersionScreen").toBool())
        {
            CustomWidget *Update { new class CustomWidget(downloadInfoFromWebsite(QRegExp(R"(^.*Message avertissement MàJ : \#([^\#]*)\#.*$)")), true) } ;
            Update->setWindowTitle(tr("Nouvelle mise à jour de Kinattente : ") + lastVersion);
            Update->setMinimumSize(100,100);// Pour éviter le "setGeometry: Unable to set geometry"
            Update->exec();
        }
    }
}

void MainWindow::launchUpdate(const bool forcingUpdate)// Slot pour mettre à jour le logiciel
{
    QString const lastVersion(downloadInfoFromWebsite(QRegExp(R"(^.*Dernière version de Kinattente  : \#(\d+(\.\d+)+)\#.*$)")));
    if(lastVersion.isEmpty())
        return;

    if(!compareVersion(lastVersion))
    {
        QMessageBox::information(this, tr("Mise à jour"), tr("Votre logiciel Kinattente est à jour !"));
        return;
    }

    if(!forcingUpdate && QMessageBox::question(this, tr("Mise à jour"), tr("Souhaitez-vous mettre à jour Kinattente ?<br/>"
                                                                          "Un export de la base de données est obligatoire pour restaurer vos données en cas d'échec potentiel et va vous être proposé."), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    if(forcingUpdate)
        QMessageBox::information(this, tr("Mise à jour de Kinattente"), tr("L'un <strong>des postes</strong> a été mis à jour dans une version supérieure à celle de ce poste.<br/>"
                                                                           "La mise à jour va démarrer, un export de la base de données est obligatoire pour restaurer vos données en cas d'échec potentiel et va vous être proposé."));
    if(!exportBDD(true))
    {
        error(136, this);
        if(forcingUpdate)
        {
            m_settings.setValue("CloseEvent", "UpdateFail");
            close();
        }
        return;
    }

    Updater *Updater { new class Updater };
    Updater->exec();
    return;
}

void MainWindow::refreshReception(QString const message)// Slot pour réception d'une MàJ de la BDD
{
    if(message.isEmpty())
        return;

    int refreshCode { 0 };
    QStringList const list { message.split('#') };
    if(list[0] == "WaitingList")
    {
        if(list[1] == "Addition")
            refreshCode = 0;
        if(list[1] == "Withdrawal")
            refreshCode = 1;
        if(list[1] == "PatientEdit")
            refreshCode = 2;
        if(list[1] == "Unarchiving")
            refreshCode = 3;
        if(list[1] == "KineEdit")
            refreshCode = 4;
        refreshWaitingList(static_cast<actualisationType>(refreshCode), list[2].toInt(), (list[3] == "Office" ? Office : Home), (list[4] == "Office" ? Office : Home));
    }

    if(list[0] == "ArchivesEditor")
    {
        if(list[1] == "Archives")
            refreshCode = 0;
        if(list[1] == "Editor")
            refreshCode = 1;
        refreshArchivesEditor(static_cast<index>(refreshCode));
    }

    if(list[0] == "PendingAction")
    {
        m_settings.setValue("PendingAction", list[1]);
        if(list.size() > 2)
            m_settings.setValue("ClientIndex", list[2]);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)// keyPressEvent pour valider le bouton Archives / Edition avec "Entrée"
{
    if((event->key() == Qt::Key_Enter || event->key() == 16777220) && (m_stackedWidget.currentIndex() != 2))
    clicAEButton();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSqlDatabase::removeDatabase(m_settings.value("BDD/Driver").toString());
    if(m_settings.value("BDD/SaveOption", false).toBool())
        exportBDD();
    logEntry("Close : " + m_settings.value("CloseEvent", "quit()").toString());
    event->accept();
}

void MainWindow::loadStyleSheet()
{
    QFile file(m_settings.value("CSS", "Aqua.qss").toString());
    if(file.open(QFile::ReadOnly))
        qApp->setStyleSheet(QString::fromLatin1(file.readAll()));
    else
        error(100, this);
}
