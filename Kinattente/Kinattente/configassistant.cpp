#include "configassistant.h"

ConfigAssistant::ConfigAssistant(QWidget *parent) : QWizard(parent)// Constructeur
{
    setWindowTitle("Assistant de configuration");
    setWizardStyle(QWizard::ModernStyle);
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - Intro.png").scaledToWidth(175, Qt::SmoothTransformation));
    setPixmap(QWizard::LogoPixmap, QPixmap("Images/Kinattente.png").scaledToWidth(175, Qt::SmoothTransformation));
    setOptions(QWizard::NoCancelButton|QWizard::NoBackButtonOnLastPage);

    setPage(Page_Intro, new IntroPage());
    setPage(Page_BDD, new BDDPage());
    setPage(Page_MySQLInstall, new MySQLInstallPage());
    setPage(Page_MySQLFinal, new MySQLFinalPage());
    setPage(Page_IPAddress, new IPAddressPage());
    setPage(Page_Save, new SavePage());
    setPage(Page_Final, new FinalPage());

    connect(button(QWizard::FinishButton), &QPushButton::clicked, this, &ConfigAssistant::finished);
}

ConfigAssistant::~ConfigAssistant()// Destructeur
{

}

int ConfigAssistant::nextId() const// Réimplémentation de la fonction nextId()
{
    QSettings settings;
    switch(currentId())
    {
        case Page_Intro:
            if(settings.value("OnlyMySQLInstall", false).toBool())
                return Page_MySQLInstall;
            else
                return Page_BDD;

        case Page_BDD:
            if(field("UniqueWorkplace").toBool())
                return Page_Save;
            else
                if(field("MasterWorkplace").toBool())
                    return Page_MySQLInstall;
                else
                    return Page_IPAddress;

        case Page_MySQLInstall:
            return Page_MySQLFinal;

        case Page_MySQLFinal:
            if(settings.value("OnlyMySQLInstall", false).toBool())
                return Page_Final;
            else
                return Page_Save;

        case Page_IPAddress:
            return Page_Save;

        case Page_Save:
            return Page_Final;

        case Page_Final:
            return -1;

        default:
            return -1;
    }
}

void ConfigAssistant::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent), m_surpriseButtonClicked { false }
{
    setTitle(tr("Introduction"));
    setSubTitle(tr("<br/>Bienvenue dans l'assistant de configuration de votre logiciel Kinattente."));

    QLabel *warningLogo { new QLabel };
    warningLogo->setPixmap(QPixmap("Images/Warning.png"));
    warningLogo->setFixedSize(66, 55);
    warningLogo->setScaledContents(true);
    QLabel *text1 { new QLabel(tr("Vous avez définitivement fait le bon choix, et vous pouvez en être fier !<br/>"
                                  "Finissons tranquillement cette fichue installation et commençons enfin à organiser nos patients en attente !!<br/><br/>")) };
    QLabel *text2 { new QLabel(tr("<strong>ATTENTION !!!</strong> Je ne serais <strong>ABSOLUMENT</strong> pas responsable si vous faites des bêtises avec "
                                  "le logiciel et que vous perdez votre liste d'attente. En théorie, si vous suivez les règles "
                                  "tout se passera bien mais pour un logiciel réalisé gratuitement vous comprendrez que je me dégage de "
                                  "toute responsabilité ! Il n'existe donc aucune garantie entre nous, en revanche si vous "
                                  "avez un quelconque doute je serai ravi de vous aider >>> <a href='https://kinattente.wordpress.com/contact/' >me contacter</a><br/><br/>")) };
    text2->setWordWrap(true);
    QLabel *text3 { new QLabel(tr("Vous ne le saviez sûrement pas (tout comme moi) mais vous êtes sensé informer vos patients que vous "
                                  "collectez leurs données quand vous leur répondez au téléphone ou qu'ils viennent en personne au cabinet. "
                                  "Et moi je me dois de vous le rappeler car ce logiciel vous aide à collecter ces données.")) };
    text3->setWordWrap(true);
    text1->setOpenExternalLinks(true);
    m_surpriseButton.setText(tr("Clique ici pour recevoir une belle surprise !!!!"));
    m_textBrowser.insertHtml(tr("Et voilà bien fait, voici des rappels de la RGPD, tu ne pourras pas dire que ne les as pas vu !!!<br/>"
                             "Une liste d'attente s'inscrit dans une collecte de données patients et doit se conformer au<br/>"
                             "<strong>Règlement Général sur la Protection des Données</strong> :<br/>"
                             "• Le RGPD s’applique aux <strong>traitements informatiques et aussi aux dossiers papier</strong><br/>"
                             "• La transmission des données de santé de vos patients doit être limitée aux seules personnes<br/>"
                             "qui sont autorisées à y accéder au regard de leurs missions<br/>"
                             "• Les données de vos patients <strong>ne peuvent être gardées indéfiniment</strong><br/>"
                             "• <strong>Les patients doivent être informés</strong> du traitement de leurs données mais vous n’avez pas<br/>"
                             "à recueillir leur consentement (ex : sous la forme d’une affiche dans votre salle d’attente)<br/>"
                             "• Il n’y a plus de déclaration à faire auprès de la CNIL mais vous devez <strong>tenir un registre des traitements</strong><br/>"
                             "Pour connaître les règlementations en détails, vous pouvez vous rendre <a href='https://www.cnil.fr/fr/rgpd-et-professionnels-de-sante-liberaux-ce-que-vous-devez-savoir' >ici</a>."));
    m_textBrowser.setOpenExternalLinks(true);
    m_textBrowser.setMaximumHeight(73);
    m_textBrowser.hide();

    QVBoxLayout *layout { new QVBoxLayout };
    QHBoxLayout *warningLayout { new QHBoxLayout };
    setLayout(layout);
        layout->addWidget(text1);
        layout->addLayout(warningLayout);
            warningLayout->addWidget(warningLogo, Qt::AlignVCenter);
            warningLayout->addWidget(text2);
        layout->addWidget(text3);
        layout->addWidget(&m_surpriseButton);
        layout->addWidget(&m_textBrowser);

    connect(&m_surpriseButton, &QPushButton::clicked, this, &IntroPage::surpriseButtonClicked);
}

void IntroPage::surpriseButtonClicked()
{
    m_textBrowser.show();
    m_textBrowser.moveCursor(QTextCursor::Start);
    m_surpriseButtonClicked = true;
    emit completeChanged();
}

bool IntroPage::isComplete() const
{
    if(m_surpriseButtonClicked)
        return true;
    else
        return false;
}

BDDPage::BDDPage(QWidget *parent) : QWizardPage(parent), m_sqliteAlreadyCopied { false }
{
    setTitle(tr("Choix du nombre de postes"));
    setSubTitle(tr("<br/>Sur combien de postes souhaitez-vous que Kinattente soit installé ?"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - BDD.png").scaledToWidth(175, Qt::SmoothTransformation));

    m_uniqueWorkplace.setText(tr("Un seul poste"));
        m_uniqueDescription.setText(tr("<i>Configuration trèèèèès rapide, passage au multiposte possible ultérieurement.</i><br/><br/>"));
        m_mysqlPathDescription.hide();
        m_mysqlPathDescription.setWordWrap(true);

    m_multiWorkplace.setText(tr("Plusieurs Postes"));
        m_multiDescription.setText(tr("<i>Plus long car nécessite l'installation de la Base de données MySQL sur le poste principal.</i><br/><br/>"));
        m_masterWorkplace.setText(tr("Ordinateur Principal"));
        m_masterWorkplace.hide();
            m_masterDescription.setText(tr("<i>Vous allez installer MySQL sur <strong>ce poste</strong>.</i><br/><br/>"));
            m_masterDescription.hide();
        m_slaveWorkplace.setText(tr("Ordinateur Secondaire"));
        m_slaveWorkplace.hide();
            m_slaveDescription.setText(tr("<i>Vous avez <strong><u>déjà</u></strong> installé Kinattente et Mysql sur un autre ordinateur.</i><br/><br/>"));
            m_slaveDescription.hide();

    m_numberGroup.addButton(&m_uniqueWorkplace);
    m_numberGroup.addButton(&m_multiWorkplace);

    m_typeGroup.addButton(&m_masterWorkplace);
    m_typeGroup.addButton(&m_slaveWorkplace);

    QLabel *helpText { new QLabel(tr("Je vous suggère <strong>FORTEMENT</strong> de suivre le tutoriel vidéo : <a href='https://kinattente.wordpress.com/videos/#Installation'>Installation de Kinattente</a>")) };
    helpText->setOpenExternalLinks(true);

    QVBoxLayout *layout { new QVBoxLayout };
    QHBoxLayout *workplaceHLayout { new QHBoxLayout };
    QVBoxLayout *workplaceVLayout { new QVBoxLayout };
        setLayout(layout);
            layout->addWidget(&m_uniqueWorkplace);
            layout->addWidget(&m_uniqueDescription);
            layout->addWidget(&m_mysqlPathDescription);
            layout->addWidget(&m_multiWorkplace);
            layout->addWidget(&m_multiDescription);
            layout->addLayout(workplaceHLayout);
                workplaceHLayout->addStretch(1);
                workplaceHLayout->addLayout(workplaceVLayout, 20);
                    workplaceVLayout->addWidget(&m_masterWorkplace);
                    workplaceVLayout->addWidget(&m_masterDescription);
                    workplaceVLayout->addWidget(&m_slaveWorkplace);
                    workplaceVLayout->addWidget(&m_slaveDescription);
            layout->addWidget(helpText);

    registerField("UniqueWorkplace*", &m_uniqueWorkplace);
    registerField("MultiWorkplace*", &m_multiWorkplace);
    registerField("MasterWorkplace*", &m_masterWorkplace);
    registerField("SlaveWorkplace*", &m_slaveWorkplace);

    connect(&m_uniqueWorkplace, &QRadioButton::clicked, this, &BDDPage::WorkplaceChecked);
    connect(&m_multiWorkplace, &QRadioButton::clicked, this, &BDDPage::WorkplaceChecked);
}

void BDDPage::WorkplaceChecked(bool checked)
{
    Q_UNUSED(checked);
    if(m_uniqueWorkplace.isChecked())
    {
        m_multiWorkplace.setText(tr("Plusieurs Postes"));
        m_uniqueWorkplace.setStyleSheet("color:black;");
        m_uniqueDescription.setStyleSheet("color:black;");
        m_multiWorkplace.setStyleSheet("color:grey;");
        m_multiDescription.setStyleSheet("color:grey;");
        m_typeGroup.setExclusive(false);
        m_masterWorkplace.setChecked(false);
        m_slaveWorkplace.setChecked(false);
        m_typeGroup.setExclusive(true);
        m_masterDescription.hide();
        m_masterWorkplace.hide();
        m_slaveDescription.hide();
        m_slaveWorkplace.hide();

        if(QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data.sqlite") && !m_sqliteAlreadyCopied)
        {
            m_mysqlPathDescription.show();
            m_mysqlPathDescription.setText(tr("<strong style='color:red'>Kinattente a retrouvé une base de données déjà existante, elle sera utilisée.</strong><br/><br/>"));
        }
        emit completeChanged();
        return;
    }

    m_multiWorkplace.setText(tr("Plusieurs Postes, celui-ci sera :"));
    m_multiWorkplace.setStyleSheet("color:black;");
    m_multiDescription.setStyleSheet("color:black;");
    m_uniqueWorkplace.setStyleSheet("color:grey;");
    m_uniqueDescription.setStyleSheet("color:grey;");
    m_mysqlPathDescription.hide();
    m_masterDescription.show();
    m_masterWorkplace.show();
    m_slaveDescription.show();
    m_slaveWorkplace.show();
    emit completeChanged();
}

void BDDPage::cleanupPage()
{
    m_numberGroup.setExclusive(false);
    m_typeGroup.setExclusive(false);
    m_uniqueWorkplace.setChecked(false);
    m_multiWorkplace.setChecked(false);
    m_masterWorkplace.setChecked(false);
    m_slaveWorkplace.setChecked(false);
    m_numberGroup.setExclusive(true);
    m_typeGroup.setExclusive(true);
    m_uniqueWorkplace.setStyleSheet("color:black;");
    m_uniqueDescription.setStyleSheet("color:black;");
    m_multiWorkplace.setText(tr("Plusieurs Postes"));
    m_multiWorkplace.setStyleSheet("color:black;");
    m_multiDescription.setStyleSheet("color:black;");
    m_masterDescription.hide();
    m_masterWorkplace.hide();
    m_slaveDescription.hide();
    m_slaveWorkplace.hide();
    m_mysqlPathDescription.hide();
}

bool BDDPage::isComplete() const
{
    if(m_uniqueWorkplace.isChecked())
        return true;
    else
        if(m_masterWorkplace.isChecked() || m_slaveWorkplace.isChecked())
            return true;
        else
            return false;
}

bool BDDPage::validatePage()
{
    if(!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data.sqlite"))
    {
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        if(!QFile::copy("data.sqlite", QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data.sqlite"))
        {
            error(901, nullptr);
            m_mysqlPathDescription.show();
            m_mysqlPathDescription.setText(tr("<strong style='color:red'>Cliquez à nouveau sur \"Un seul poste\" pour relancer la création de la base de données et pouvoir continuer</strong><br/><br/>"));
            return false;
        }
        m_sqliteAlreadyCopied = true;
    }
    return true;
}

MySQLInstallPage::MySQLInstallPage(QWidget *parent) : QWizardPage(parent), m_vcRedistInstallOk(false)
{
    setTitle(tr("Installation de MySQL"));
    setSubTitle(tr("<br/>Bienvenue dans l'assistant d'installation du serveur MySQL."));
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - MySQL.png").scaledToWidth(175, Qt::SmoothTransformation));

    QLabel *text1 { new QLabel(tr("<strong>MySQL</strong> est un logiciel à part entière qui va stocker localement (c'est à dire sur cet ordinateur uniquement) les informations patients "
                               "et permettre de les lire sur tous les ordinateurs de votre cabinet EN MEME TEMPS !<br/><br/>"
                               "Commencez par installer les <strong>librairies</strong> nécessaires au bon fonctionnement de Kinattente :")) };
        text1->setWordWrap(true);

    QPushButton *vc_redistInstallationButton { new QPushButton("Installer les librairies Visual C++ 2015-2019") };

    QLabel *text2 { new QLabel(tr("En cliquant sur le bouton <strong>Installer MySQL</strong> ci-dessous, vous allez être redirigé vers la page de<br/>"
                                  "téléchargement du logiciel. <strong>Suivez les étapes de son installation en <a href='https://kinattente.wordpress.com/videos/#Installation'>vidéo</a> !</strong>")) };
        text2->setWordWrap(true);

    m_mysqlInstallationButton.setText("Installer MySQL");
    m_mysqlInstallationButton.setEnabled(false);

    QLabel *text3 { new QLabel(tr("Si vous constatez que la vidéo n'est plus à jour et que vous n'arrivez plus à installer MySQL <strong><a href='https://kinattente.wordpress.com/contact/' >contactez-moi !</a></strong>")) };
        text3->setWordWrap(true);

    QVBoxLayout *layout { new QVBoxLayout };
    setLayout(layout);
        layout->addWidget(text1);
        layout->addWidget(vc_redistInstallationButton);
        layout->addWidget(text2);
        layout->addWidget(&m_mysqlInstallationButton);
        layout->addWidget(text3);

    connect(vc_redistInstallationButton, &QPushButton::clicked, this, &MySQLInstallPage::launchVCRedistInstall);
    connect(vc_redistInstallationButton, &QPushButton::clicked, this, &MySQLInstallPage::completeChanged);
    connect(&m_mysqlInstallationButton, &QPushButton::clicked, this, &MySQLInstallPage::launchMySQLInstall);
    connect(&m_mysqlInstallationButton, &QPushButton::clicked, this, &MySQLInstallPage::completeChanged);
}

void MySQLInstallPage::launchVCRedistInstall()
{
    QProcess VC_redistProcess;
        VC_redistProcess.setProgram("VC_redist.x64.exe");
        VC_redistProcess.start();
        VC_redistProcess.waitForFinished();
    m_mysqlInstallationButton.setEnabled(true);
    m_vcRedistInstallOk = true;
}

void MySQLInstallPage::launchMySQLInstall()
{
    QDesktopServices::openUrl(QUrl("https://dev.mysql.com/downloads/mysql/"));
}

void MySQLInstallPage::cleanupPage()
{

}

bool MySQLInstallPage::isComplete() const
{
    if((m_vcRedistInstallOk))
        return true;
    else
        return false;
}

MySQLFinalPage::MySQLFinalPage(QWidget *parent) : QWizardPage(parent), m_mysqlDatabaseChecked(false)
{
    setTitle("Installation Complete");
    setSubTitle(tr("<br/>Installation de MySQL terminée."));
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - MySQL.png").scaledToWidth(175, Qt::SmoothTransformation));

        m_mysqlCheckUpButton.setText("Vérifier MySQL");
        m_password.setEchoMode(QLineEdit::Password);
        m_mysqlCheckUpProgress.setReadOnly(true);
        m_mysqlCheckUpProgress.ensureCursorVisible();
        QFontMetrics fontmectrics (m_mysqlCheckUpProgress.font()) ;
        m_mysqlCheckUpProgress.setFixedHeight(fontmectrics.lineSpacing() * 13);

    QFormLayout *layout { new QFormLayout };
    setLayout(layout);
        layout->addWidget(new QLabel(tr("Reportez le <strong>Mot de passe</strong> que vous avez choisi dans MySQL :")));
        layout->addRow("Mot de &passe :", &m_password);
        layout->addWidget(new QLabel(tr("Vérifiez que l'installation s'est bien passée en cliquant ici :")));
        layout->addWidget(&m_mysqlCheckUpButton);
        layout->addWidget(&m_mysqlCheckUpProgress);

    registerField("MysqlBinDirPath", &m_mysqlBinDirPath);
    registerField("Password", &m_password);

    connect(&m_mysqlCheckUpButton, &QPushButton::clicked, this, &MySQLFinalPage::mysqlCheckUpClicked);
    connect(&m_mysqlCheckUpProgress, &QTextEdit::textChanged, &m_mysqlCheckUpProgress, [this]() {m_mysqlCheckUpProgress.verticalScrollBar()->setValue(m_mysqlCheckUpProgress.verticalScrollBar()->maximum()); });
}

void MySQLFinalPage::mysqlCheckUpClicked()
{
    // Recherche du répertoire de MySQL pour confirmer son installation
    QSettings const settings;
    QStringList backup;
        backup << "--host=127.0.0.1" << "--user=root" << "--password=" + m_password.text();

    m_mysqlCheckUpProgress.insertHtml(tr("Recherche du chemin d'installation MySQL... (cela peut prendre quelques minutes)"));
    QCoreApplication::processEvents();
    m_binDir = pathToDirectory("mysqldump.exe").replace("/mysqldump.exe", "");

    if(m_binDir.isEmpty())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de localisation de MySQL sur votre ordinateur. Avez-vous bien installé le serveur MySQL ?</i>"));
        QCoreApplication::processEvents();
        return;
    }

    m_mysqlBinDirPath.setText(m_binDir);// Enregistrement du chemin de mysqldump.exe
    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Chemin d'installation MySQL trouvé...</strong><br/><i>") + m_binDir + "</i>");
    QCoreApplication::processEvents();

    // Connexion à la BDD "mysql"
    QSqlDatabase dbMySQL { QSqlDatabase::addDatabase("QMYSQL", "mysql") };
        dbMySQL.setDatabaseName("mysql");
        dbMySQL.setHostName("127.0.0.1");
        dbMySQL.setUserName("root");
        dbMySQL.setPassword(m_password.text());

    if(!dbMySQL.open())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de connexion à la Database \"mysql\". Êtes-vous sûr de votre mot de passe ?</i><br/>") + dbMySQL.lastError().text());
        QCoreApplication::processEvents();
        return;
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Connexion à la Database \"mysql\" réussie...</strong>"));
    QCoreApplication::processEvents();

    // Vérification de l'existance du l'utilisateur "userskine"
    QSqlQuery sql_mysql(dbMySQL);
    sql_mysql.prepare("SELECT * FROM user WHERE User = 'userskine'");
    if(!sql_mysql.exec())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Une erreur a eu lieu lors de la requête MySQL de recherche de l'existance de l'utilisateur spécifique, veuillez recommencer.</i>") + sql_mysql.lastError().text());
        return;
    }

    if(sql_mysql.size() == 1)
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Utilisateur \"userskine\" trouvé...</strong>"));
    else
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Utilisateur \"userskine\" non trouvé...</strong>"));
        // Création du l'utilisateur "userskine"
        QProcess createUserProcess;
            createUserProcess.setProgram(m_binDir + "/mysql");
            createUserProcess.setStandardInputFile(QDir::currentPath() + "/CreateUser.sql");
            createUserProcess.setArguments(backup);
            createUserProcess.start();
            createUserProcess.waitForFinished();

        m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Fichier \"CreateUser.sql\" lancé...</strong>"));
        QCoreApplication::processEvents();

        // Vérification de l'existance du l'utilisateur "userskine" nouvellement créé
        QSqlQuery sql_mysql(dbMySQL);
        sql_mysql.prepare("SELECT * FROM user WHERE User = 'userskine'");
        if(!sql_mysql.exec())
        {
            m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Une erreur a eu lieu lors de la requête MySQL de recherche de l'existance de l'utilisateur spécifique, veuillez recommencer.</i>") + sql_mysql.lastError().text());
            return;
        }
        else
        {
            if(sql_mysql.size() != 1)
            {
                m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Utilisateur \"userskine\" non trouvé...</i>"));
                return;
            }
            m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Utilisateur \"userskine\" trouvé...</strong>"));
        }
    }
    removeDatabase("mysql");

    // Connexion à la BDD "information_schema"
    QSqlDatabase dbInfoSchema = QSqlDatabase::addDatabase("QMYSQL", "information_schema");
        dbInfoSchema.setDatabaseName("information_schema");
        dbInfoSchema.setHostName("127.0.0.1");
        dbInfoSchema.setUserName("root");
        dbInfoSchema.setPassword(m_password.text());

    if(!dbInfoSchema.open())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de connexion à la Database \"information_schema\". Êtes-vous sûr de votre mot de passe ?<br/></i>") + dbInfoSchema.lastError().text());
        QCoreApplication::processEvents();
        return;
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Connexion à la Database \"information_schema\" réussie...</strong>"));
    QCoreApplication::processEvents();

    // Recherche de la DataBase Kinattente
    QSqlQuery sql_infoSchema(dbInfoSchema);
    sql_infoSchema.prepare("SELECT * FROM TABLES WHERE TABLE_SCHEMA = 'kinattente'");

    if(!sql_infoSchema.exec())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Une erreur a eu lieu lors de la requête MySQL de recherche de l'existance des tables, veuillez recommencer.<br/></i>") + sql_infoSchema.lastError().text());
        return;
    }

    if(sql_infoSchema.size() != 4 || settings.value("OnlyMySQLInstall", false).toBool())
    {
        // Création de la DataBase Kinattente et de ses tables
        QProcess fillUpKinattenteProcess;
            fillUpKinattenteProcess.setProgram(m_binDir + "/mysql");
            fillUpKinattenteProcess.setStandardInputFile(QDir::currentPath() + "/FillUpKinattente.sql");
            fillUpKinattenteProcess.setArguments(backup);
            fillUpKinattenteProcess.start();
            fillUpKinattenteProcess.waitForFinished();

        m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Fichier \"FillUpKinattente.sql\" lancé...</strong>"));
        QCoreApplication::processEvents();
    }
    removeDatabase("information_schema");

    // Connexion à la BDD "Kinattente"
    QSqlDatabase dbKinattente = QSqlDatabase::addDatabase("QMYSQL", "QMYSQL");
        dbKinattente.setDatabaseName("Kinattente");
        dbKinattente.setHostName("127.0.0.1");
        dbKinattente.setUserName("userskine");
        dbKinattente.setPassword("localnetwork");

    if(!dbKinattente.open())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de connexion à la database \"Kinattente\". Êtes-vous sûr de votre mot de passe ?<br/></i>") + dbKinattente.lastError().text());
        QCoreApplication::processEvents();
        return;
    }

    qDebug() << "Database MySQL opened successfully!";
    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Connexion à la Database \"Kinattente\" réussie...</strong>"));
    QCoreApplication::processEvents();

    // Remplissage des tables de Kinattente si transfert SQLite -> MySQL
    if(settings.value("OnlyMySQLInstall", false).toBool())
        m_mysqlDatabaseChecked = sqLiteMySQLTransfert(dbKinattente);
    else
    {
        m_mysqlDatabaseChecked = true;
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Installation Complète !</i>"));
        QCoreApplication::processEvents();
    }

    emit completeChanged();
}

bool MySQLFinalPage::sqLiteMySQLTransfert(QSqlDatabase dbKinattente)
{
    // Connexion à SQLite
    QSettings settings;
    QSqlDatabase dbSQLite { QSqlDatabase::addDatabase("QSQLITE", "QSQLITE") };
        dbSQLite.setDatabaseName(settings.value("BDD/DatabaseName").toString());

    if(!dbSQLite.open())
    {
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de connexion à la Database \"SQLite\".</i>") + dbSQLite.lastError().text());
        QCoreApplication::processEvents();
        return false;
    }

    qDebug() << "Database SQLite opened successfully!";
    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Connexion à la Database \"SQLite\" réussie...</strong>"));
    QCoreApplication::processEvents();

    QSqlQuery sql_Export(dbSQLite);
    QSqlQuery sql_Import(dbKinattente);
    QSqlDatabase::database("QMYSQL").transaction();
    int i { 1 };
    QMap<int, QString> kines_id;
    QMap<int, QString> cities_id;

    // Transfert des données Kinés
    sql_Export.prepare("SELECT name, firstname, color, active, active_string FROM Kines ORDER BY id");
    if(!sql_Export.exec())
    {
        QSqlDatabase::database("QMYSQL").rollback();
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de récupération des données Kinés. Veuillez recommencer</i>.<br/>") + sql_Export.lastError().text());
        QCoreApplication::processEvents();
        return false;
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
                QSqlDatabase::database("QMYSQL").rollback();
                m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de transfert des données Kinés. Veuillez recommencer</i>.<br/>") + sql_Import.lastError().text());
                QCoreApplication::processEvents();
                return false;
            }
        }
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Transfert des données Kinés réussi...</strong>"));
    QCoreApplication::processEvents();

    // Transfert des données Villes
    i = 1;
    sql_Export.prepare("SELECT name FROM Cities ORDER BY id");
    if(!sql_Export.exec())
    {
        QSqlDatabase::database("QMYSQL").rollback();
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de récupération des données Villes. Veuillez recommencer</i>.<br/>") + sql_Export.lastError().text());
        QCoreApplication::processEvents();
        return false;
    }

    while(sql_Export.next())
    {
        cities_id.insert(i++, sql_Export.value(0).toString());
        if(i == 2)
        {
            sql_Import.prepare("UPDATE Cities SET name = :name WHERE id = 1");
            sql_Import.bindValue(":name", sql_Export.value(0).toString());
        }
        else
        {
            sql_Import.prepare("INSERT INTO Cities (name) VALUES (:name)");
            sql_Import.bindValue(":name", sql_Export.value(0).toString());
        }
        if(!sql_Import.exec())
        {
            QSqlDatabase::database("QMYSQL").rollback();
            m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de transfert des données Villes. Veuillez recommencer</i>.<br/>") + sql_Import.lastError().text());
            QCoreApplication::processEvents();
            return false;
        }
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Transfert des données Villes réussi...</strong>"));
    QCoreApplication::processEvents();

    // Transfert des données Patients Actifs
    sql_Export.prepare("SELECT p.location, p.gender, p.age, p.name, p.firstname, p.phone1, p.phone2, p.email, p.doctor, k.name, k.firstname, p.availability, p.address, c.name, p.pathology, p.no_emergency, p.call_date FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id ORDER BY p.id");
    if(!sql_Export.exec())
    {
        QSqlDatabase::database("QMYSQL").rollback();
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de récupération des données Patients actifs. Veuillez recommencer</i>.<br/>") + sql_Export.lastError().text());
        QCoreApplication::processEvents();
        return false;
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
            QSqlDatabase::database("QMYSQL").rollback();
            m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de transfert des données Patients actifs. Veuillez recommencer</i>.<br/>") + sql_Import.lastError().text());
            QCoreApplication::processEvents();
            return false;
        }
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Transfert des données Patients Actifs réussi...</strong>"));
    QCoreApplication::processEvents();

    // Transfert des données Patients Archivés
    sql_Export.prepare("SELECT p.location, p.gender, p.age, p.name, p.firstname, p.phone1, p.phone2, p.email, p.doctor, k.name, k.firstname, p.availability, p.address, c.name, p.pathology, p.emergency, p.call_date, p.archiving_date FROM Archived_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id ORDER BY p.id");
    if(!sql_Export.exec())
    {
        QSqlDatabase::database("QMYSQL").rollback();
        m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de récupération des données Patients archivés. Veuillez recommencer</i>.<br/>") + sql_Export.lastError().text());
        QCoreApplication::processEvents();
        return false;
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
            QSqlDatabase::database("QMYSQL").rollback();
            m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Echec de transfert de données Patients archivés. Veuillez recommencer</i>.<br/>") + sql_Import.lastError().text());
            return false;
        }
    }

    m_mysqlCheckUpProgress.insertHtml(tr("<br/><strong>Transfert des données Patients Archivés réussi...</strong>"));
    QCoreApplication::processEvents();

    QSqlDatabase::database("QMYSQL").commit();
    removeDatabase("QSQLITE");
    removeDatabase("QMYSQL");
    m_mysqlCheckUpProgress.insertHtml(tr("<br/><i>Installation Complète !</i>"));
    QCoreApplication::processEvents();
    m_password.setEnabled(false);
    m_mysqlCheckUpButton.setEnabled(false);
    return true;
}

void MySQLFinalPage::cleanupPage()
{
    m_mysqlDatabaseChecked = false;
    m_password.setEnabled(true);
    m_mysqlCheckUpButton.setEnabled(true);
    m_mysqlCheckUpProgress.setHtml("");
}

bool MySQLFinalPage::isComplete() const
{
    if(m_mysqlDatabaseChecked)
        return true;
    else
        return false;
}

IPAddressPage::IPAddressPage(QWidget *parent) : QWizardPage(parent), m_connectionOk(false)
{
    setTitle("Adresse IP locale");
    setSubTitle("<br/>Remplissez l'adresse IP locale du poste Principal");
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - IPAddress.png").scaledToWidth(175, Qt::SmoothTransformation));
    QLineEdit *IPAddress { new QLineEdit };
    IPAddress->setInputMask("900.900.900.900;_");
    m_connectionTestButton.setText("Tester la connexion au poste principal");
    m_connectionTestLabel.setWordWrap(true);
    QFormLayout *layout { new QFormLayout };
    setLayout(layout);
        layout->addRow(new QLabel("<i>\"Parlez ami et entrer !\" - Gandalf</i><br/>"
                                  "<strong>L'adresse IP</strong> du poste Principal vous a été donné à la fin de <strong>l'Assistant de configuration</strong>.<br/>"
                                  "Si vous ne l'avez plus, pas de panique ! Vous le trouverez dans <strong>\"Outils->Options\"</strong> sur le poste Principal.<br/>"
                                  "Si vous n'avez pas encore installé Kinattente sur le poste Principal, faites-le !<br/><br/>"));
        layout->addRow("&Adresse IP Locale :", IPAddress);
        layout->addRow(new QLabel("<i>Il est tout à fait normal qu'il puisse rester des espaces vides. Faites attention au placement des points !</i>"));
        layout->addRow(&m_connectionTestButton);
        layout->addRow(&m_connectionTestLabel);
    registerField("IPAddress*", IPAddress);

    connect(&m_connectionTestButton, &QPushButton::clicked, this, &IPAddressPage::connectionTestClicked);
}

void IPAddressPage::connectionTestClicked()
{
    if(field("IPAddress").toString().contains(QRegExp("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}")))
    {
        if(connectToDatabase(field("IPAddress").toString()))
        {
            m_connectionOk = true;
            m_connectionTestButton.setEnabled(false);
            m_connectionTestLabel.setText(tr("<i>Connexion réussie !</i>"));
            emit completeChanged();
            return;
        }
        m_connectionTestLabel.setText(tr("<i>La connexion a échouée, vérifier que l'adresse IP soit la bonne, que Kinattente soit ouvert sur le poste principal et que les deux postes soient reliés sur le même réseau local.</i>"));
    }
    else
        m_connectionTestLabel.setText(tr("<i>L'adresse IP doit être correctement complétée.</i>"));
}

void IPAddressPage::cleanupPage()
{
    m_connectionTestButton.setEnabled(true);
    m_connectionOk = false;
}

bool IPAddressPage::isComplete() const
{
    if(m_connectionOk)
        return true;
    else
        return false;
}

SavePage::SavePage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Choix de la sauvegarde automatique");
    setSubTitle("<br/>Souhaitez-vous effectuer une sauvegarde automatique à chaque fermeture du logiciel ?");
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - Save.png").scaledToWidth(175, Qt::SmoothTransformation));
    m_noSave.setText("Aucune sauvegarde");
    m_noSaveLabelText.setText("<i>Vous effectuerez seul vos sauvegardes quand vous le souhaiterez</i><br/><br/>");
    m_autoSave.setText("Sauvegarde automatique");
    m_autoSaveLabelText.setText("<i>La sauvegarde s'effectuera automatiquement à la fermeture de Kinattente si le chemin spécifié est accessible.</i><br/><br/>");
        m_saveGroup.addButton(&m_noSave);
        m_saveGroup.addButton(&m_autoSave);
        m_savePathLabelText.setText("Chemin de sauvegarde :");
        m_savePathButton.setText("...");
        m_savePathLineEdit.setReadOnly(true);
        m_savePathLabelText.hide();
        m_savePathButton.hide();
        m_savePathLineEdit.hide();

    m_launchCheckbox.setText(tr("Lancer Kinattente au démarrage"));

    QVBoxLayout *layout { new QVBoxLayout };
    QHBoxLayout *savePathLayout { new QHBoxLayout };
        setLayout(layout);
        layout->addWidget(&m_noSave);
        layout->addWidget(&m_noSaveLabelText);
        layout->addWidget(&m_autoSave);
        layout->addWidget(&m_autoSaveLabelText);
        layout->addWidget(&m_savePathLabelText);
        layout->addLayout(savePathLayout);
            savePathLayout->addWidget(&m_savePathLineEdit);
            savePathLayout->addWidget(&m_savePathButton);
        layout->addWidget(&m_launchCheckbox);

    registerField("NoSave", &m_noSave);
    registerField("SaveOption", &m_autoSave);
    registerField("SavePath", &m_savePathLineEdit);
    registerField("StartUpLaunch", &m_launchCheckbox);

    connect(&m_noSave, &QRadioButton::toggled, this, &SavePage::saveChecked);
    connect(&m_autoSave, &QRadioButton::toggled, this, &SavePage::saveChecked);
    connect(&m_savePathButton, &QPushButton::clicked, this, &SavePage::savePathClicked);
    connect(&m_noSave, &QRadioButton::toggled, this, &SavePage::completeChanged);
    connect(&m_autoSave, &QRadioButton::toggled, this, &SavePage::completeChanged);
    connect(&m_savePathLineEdit, &QLineEdit::textChanged, this, &SavePage::completeChanged);
}

void SavePage::saveChecked(bool checked)
{
    Q_UNUSED(checked);
    if(m_noSave.isChecked())
    {
        m_noSave.setStyleSheet("color:black;");
        m_autoSave.setStyleSheet("color:grey;");
        m_noSaveLabelText.setStyleSheet("color:black;");
        m_autoSaveLabelText.setStyleSheet("color:grey;");
        m_savePathLabelText.hide();
        m_savePathButton.hide();
        m_savePathLineEdit.hide();
        return;
    }

    m_noSave.setStyleSheet("color:grey;");
    m_autoSave.setStyleSheet("color:black;");
    m_noSaveLabelText.setStyleSheet("color:grey;");
    m_autoSaveLabelText.setStyleSheet("color:black;");
    m_savePathLabelText.show();
    m_savePathButton.show();
    m_savePathLineEdit.show();
}

void SavePage::savePathClicked()
{
    m_savePathLineEdit.setText(QFileDialog::getExistingDirectory(nullptr, "Chemin de sauvegarde", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), QFileDialog::ShowDirsOnly));
    emit completeChanged();
}

void SavePage::cleanupPage()
{
    m_saveGroup.setExclusive(false);
    m_noSave.setChecked(false);
    m_autoSave.setChecked(false);
    m_saveGroup.setExclusive(true);
    m_noSave.setStyleSheet("color:black;");
    m_autoSave.setStyleSheet("color:black;");
    m_noSaveLabelText.setStyleSheet("color:black;");
    m_autoSaveLabelText.setStyleSheet("color:black;");
    m_savePathLabelText.hide();
    m_savePathButton.hide();
    m_savePathLineEdit.hide();
}

bool SavePage::isComplete() const
{
    if(m_noSave.isChecked() || (m_autoSave.isChecked() && !m_savePathLineEdit.text().isEmpty() && QDir(m_savePathLineEdit.text()).exists()))
        return true;
    else
        return false;
}

FinalPage::FinalPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Configuration terminée");
    setSubTitle("<br/>Vous avez terminé la configuration de Kinattente, qu'attendez-vous ?");
    setPixmap(QWizard::WatermarkPixmap, QPixmap("Images/Earth - Final.png").scaledToWidth(175, Qt::SmoothTransformation));
    setFinalPage(true);

    m_IPAddressText.setText("Voici votre adresse IP Locale, elle vous sera demandée lors de l'installation de Kinattente sur les autres postes :");

    m_IPAddress.setReadOnly(true);
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             m_IPAddress.setText(address.toString());

    QVBoxLayout *layout { new QVBoxLayout };
        setLayout(layout);
            layout->addWidget(&m_IPAddressText);
            layout->addWidget(&m_IPAddress);
            layout->addWidget(new QLabel("N'oubliez pas d'aller enregistrer votre ville maintenant dans l'onglet \"Outils -> Options -> Ville par Défaut\""));
}

void FinalPage::initializePage()
{
    QSettings settings;
    if(settings.value("OnlyMySQLInstall").toBool())
        return;

    settings.setValue("BDD/Port", 50885);
    if(field("UniqueWorkplace").toBool())
    {
        settings.setValue("BDD/DatabaseName", QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data.sqlite");
        settings.setValue("BDD/Driver", "QSQLITE");
        settings.setValue("BDD/IPAddress", "127.0.0.1");
    }
    else
    {
        settings.setValue("BDD/Driver", "QMYSQL");
        settings.setValue("BDD/DatabaseName", "Kinattente");
        settings.setValue("BDD/UserName", "userskine");
        settings.setValue("BDD/Password", "localnetwork");
        settings.setValue("BDD/MasterWorkplace", field("MasterWorkplace").toBool());

        if(field("MasterWorkplace").toBool())
        {
            settings.setValue("BDD/IPAddress", "127.0.0.1");
            settings.setValue("BDD/MysqlBinDirPath", field("MysqlBinDirPath").toString());
            settings.setValue("BDD/PasswordRootMySQL", field("Password").toString());
        }
        else
            settings.setValue("BDD/IPAddress", field("IPAddress").toString());
    }

    settings.setValue("BDD/SaveOption", field("SaveOption").toBool());

    if(field("SaveOption").toBool())
        settings.setValue("BDD/SavePath", field("SavePath").toString());

    settings.setValue("StartUpLaunch", field("StartUpLaunch").toBool());

    if(settings.value("BDD/MasterWorkplace").toBool())
    {
        m_IPAddressText.show();
        m_IPAddress.show();
        return;
    }

    m_IPAddressText.hide();
    m_IPAddress.hide();
}

bool FinalPage::validatePage()
{
    QSettings settings;
    if(!settings.value("OnlyMySQLInstall").toBool())
    {
        if(settings.value("StartUpLaunch").toBool())
        {
            QSettings settings_startup("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
            QString const path { QCoreApplication::applicationFilePath().replace('/', '\\') };
            settings_startup.setValue("Kinattente",path);
        }
        settings.setValue("Version", "3.0");
        settings.setValue("LastVersion", "3.0");
        settings.setValue("ConfigAssistant", true);
        settings.setValue("CSS", "Aqua.qss");
        settings.setValue("DataDuration", 20);
    }
    settings.remove("OnlyMySQLInstall");// Lors d'un transfert SQLite -> MySQL, tout s'est bien passé donc on confirme
    return true;
}

