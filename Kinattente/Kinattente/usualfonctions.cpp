#include "usualfonctions.h"

QString pathToDirectory(QString file)
{
    QDirIterator progIt("C:/Program Files/MySQL", QStringList() << file, QDir::Files, QDirIterator::Subdirectories);
    while(progIt.hasNext())
        return progIt.next();

    QFileInfoList const ListDrives { QDir::drives() };
    foreach(const QFileInfo& FileInfo, ListDrives)
    {
        QDirIterator mysqlIt(FileInfo.filePath(), QStringList() << file, QDir::Files, QDirIterator::Subdirectories);
        while(mysqlIt.hasNext())
            return mysqlIt.next();
    }
    return QString();
}

void removeDatabase(QString connectionName)
{
        QSqlDatabase::database(connectionName).close();
        QSqlDatabase::database(connectionName) = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
}

bool connectToDatabase(QString ipAddress)
{
    QSettings settings;
        settings.beginGroup("BDD");

        if(ipAddress.isEmpty())
            ipAddress = settings.value("IPAddress").toString();

    QMessageBox *connectionPending { new QMessageBox(QMessageBox::Warning, QObject::tr("Connexion en cours"), QObject::tr("Tentative de connexion..."), QMessageBox::NoButton) };
        connectionPending->show();

    if(QSqlDatabase::contains(settings.value("Driver").toString()))
        removeDatabase(settings.value("Driver").toString());
    QSqlDatabase db { QSqlDatabase::addDatabase(settings.value("Driver").toString(), settings.value("Driver").toString()) };
        db.setDatabaseName(settings.value("DatabaseName").toString());
    if(settings.value("Driver").toString() == "QMYSQL")
    {
        db.setHostName(ipAddress);
        db.setUserName(settings.value("UserName").toString());
        db.setPassword(settings.value("Password").toString());
    }

    QString password;
    bool connectionOk { false };

    do
    {
        if(db.open())
        {
            qDebug() << "Locale Database opened successfully!";
            if(!password.isEmpty())
                settings.setValue("Password", password);
            if(!ipAddress.isEmpty())
                settings.setValue("IPAddress", ipAddress);
            connectionPending->close();
            return true;
        }

        qDebug() << "Locale Database opening failed :(";

        if(settings.value("Driver").toString() == "QSQLITE")
        {
            if(QMessageBox::No == QMessageBox::critical(nullptr, QObject::tr("Connexion à la Base de Données"), QObject::tr("Kinattente n'a pas réussi à se connecter à la base de données.<br/>"
                                                                                                      "Ceci peut être un problème aléatoire, auquel cas cliquez sur <strong>Non</strong> et relancez Kinattente pour le régler.<br/>"
                                                                                                      "Si le problème persiste souhaitez-vous importer une <strong>sauvegarde précédente</strong> ?<br/>"
                                                                                                      "Votre sauvegarde actuelle sera déplacée sur le bureau."),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes))
            {
                connectionPending->close();
                return false;
            }

            QString const saveFilePath { QFileDialog::getOpenFileName(nullptr, QObject::tr("Ouvrir un fichier"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "Sauvegarde (*.sqlite)") };

            if(saveFilePath.isEmpty())
            {
                connectionPending->close();
                return false;
            }

            if(QFile::exists(settings.value("BDD/DatabaseName").toString()))
            {
                if(!QFile::copy(settings.value("BDD/DatabaseName").toString(), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/data" + QDateTime::currentDateTime().toString() + ".sqlite"))
                {
                    error(800, nullptr);
                    connectionPending->close();
                    return false;
                }

                if(!QFile::remove(settings.value("BDD/DatabaseName").toString()))
                {
                    error(801, nullptr);
                    connectionPending->close();
                    return false;
                }
            }

            if(!QFile::copy(saveFilePath, settings.value("BDD/DatabaseName").toString()))
            {
                error(802, nullptr);
                connectionPending->close();
                return false;
            }
        }

        if(db.lastError().text().contains("Unknown database"))
        {
            error(803, nullptr, db.lastError().text());
            connectionPending->close();
            return false;
        }

        if(settings.value("MasterWorkplace").toBool() && pathToDirectory("mysqldump.exe").isEmpty())
        {
            do
            {
                QString const inputText { QObject::tr("Le logiciel n'a pas pu se connecter à la base de données MySQL contenant toutes les données de la liste d'attente.<br/>"
                                       "Vérifiez que <strong>MySQL est toujours installé</strong>, sinon réinstallez-le en suivant le tutoriel et reportez le <strong>mot de passe</strong> choisis :<br/>"
                                       "https://kinattente.wordpress.com/videos/#Installation<br/>") + db.lastError().text() };
                password = QInputDialog::getText(nullptr, QObject::tr("Erreur Base de Données"), inputText, QLineEdit::Password, QString(), &connectionOk);

                if(connectionOk && password.isEmpty())
                    error(804, nullptr);
                else
                    db.setPassword(password);

            } while (connectionOk && password.isEmpty());
            if(!connectionOk)
            {
                connectionPending->close();
                return false;
            }
        }
        else
        {
            do
            {
                QString const inputText { QObject::tr("Le logiciel n'a pas pu se connecter à la base de données MySQL contenant toutes les données de la liste d'attente.<br/>"
                                       "Vérifiez que <strong>Kinattente est ouvert sur le poste principal</strong> et que les deux postes sont <strong>reliés sur le même réseau local</strong>.<br/>"
                                       "Vérifiez que <strong>l'adresse IP locale de l'ordinateur Principal</strong> est toujours :<strong>") + settings.value("IPAddress").toString() + QObject::tr("</strong><br/>"
                                       "Vous pourrez la trouver dans Kinattente sur l'ordinateur Principal en vous rendant dans l'onglet \"Outils -> Options\"<br/>"
                                       "Si celle-ci a changé, veuillez la reporter ici :<br/>") + db.lastError().text() };
                ipAddress = QInputDialog::getText(nullptr, QObject::tr("Erreur Base de Données"), inputText, QLineEdit::Normal, ipAddress, &connectionOk);

                if(connectionOk && !ipAddress.contains(QRegExp(R"(^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$)")))
                    error(805, nullptr);
                else
                    db.setHostName(ipAddress);
            } while (connectionOk && !ipAddress.contains(QRegExp(R"(^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$)")));
            if(!connectionOk)
            {
                connectionPending->close();
                return false;
            }
        }
    } while (connectionOk);
    connectionPending->close();
    return false;
}

bool pendingAction(QString title, QString text)
{
    if(title.isEmpty())
        title = QObject::tr("Action impossible");
    if(text.isEmpty())
        text = QObject::tr("Une action est en cours sur un autre poste, veuillez patienter.");
    QSettings const settings;
    if(settings.value("PendingAction").toInt() != -1)
    {
        QMessageBox::critical(nullptr, title, text);
        return true;
    }
    else
        return false;
}

void logEntry(QString text)
{
    QFileInfo logFileInfo("LogFile.txt");
    QFile logFile("LogFile.txt");
    logFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux1(&logFile);
    QStringList logText;
    while(!flux1.atEnd())
        logText << flux1.readLine();
    logFile.close();
    logText.append(text + " " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    if(logFileInfo.size() > 10 * 1024 * 1024)
        logText.removeFirst();

    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux2(&logFile);
    for(int i { 0 }; i < logText.size(); i++)
        flux2 << logText[i] << Qt::endl;
    logFile.close();
}

void error(int errorCode, QWidget *parent, QString errorText)
{
    QSettings settings;
    switch (errorCode)
    {
        case 100:
        QMessageBox::warning(parent, QObject::tr("Chargement Feuille de Style échoué"), QObject::tr("Le fichier qui doit rendre ton extraordinaire logiciel Kinattente tout joli n'a pas été trouvé... C'est balo :/") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 101:
        QMessageBox::critical(parent, QObject::tr("Assistant de configuration inachevé"), QObject::tr("La configuration de Kinattente n'a pas pu se terminer, tu pourras la recommencer en relançant le logiciel.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 102:
        QMessageBox::critical(parent, QObject::tr("Connexion au serveur ratée"), QObject::tr("Le logiciel n'a pas pu se connecter au serveur, Kinattente doit être ouvert sur le poste principal !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 103:
        QMessageBox::warning(parent, QObject::tr("Suppression des Patients dépassant le délai de conservation légal échouée"), QObject::tr("Les patients dépassant la durée maximale de conservation de leurs données n'ont pas pu être supprimés.<br/>"
                                                 "Avez-vous bien lancé Kinattente en mode Administrateur ?<br/>"
                                                 "Error #") + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 104:
        QMessageBox::warning(parent, QObject::tr("Sélection des données patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des données patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 105:
        QMessageBox::warning(parent, QObject::tr("Archivage Patient échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'archivage du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 106:
        QMessageBox::warning(parent, QObject::tr("Archivage patient échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'archivage du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 107:
        QMessageBox::warning(parent, QObject::tr("Sélection des données patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des données patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 108:
        QMessageBox::warning(parent, QObject::tr("Désarchivage patient échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de désarchivage du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 109:
        QMessageBox::warning(parent, QObject::tr("Désarchivage patient échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de désarchivage du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 110:
        QMessageBox::warning(parent, QObject::tr("Suppression Kiné échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression du Kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 111:
        QMessageBox::warning(parent, QObject::tr("Suppression Kiné échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression du Kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 112:
        QMessageBox::warning(parent, QObject::tr("Suppression Kiné échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression du Kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 113:
        QMessageBox::warning(parent, QObject::tr("Sauvegarde de la Base de Données échouée"), QObject::tr("La sauvegarde automatique n'a pu être effectuée, le chemin d'accès était introuvable :<br/><strong>\"") + settings.value("BDD/SavePath").toString() + QObject::tr("\"</strong><br/>Vous pouvez le modifier dans \"Outils->Options\".") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 114:
        QMessageBox::critical(parent, QObject::tr("Sauvegarde de la Base de Données échouée"), QObject::tr("La sauvegarde automatique a échouée !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 115:
        QMessageBox::warning(parent, QObject::tr("Sauvegarde de la Base de Données échouée"), QObject::tr("La suppression de l'ancienne sauvegarde a échouée.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 116:
        QMessageBox::critical(parent, QObject::tr("Renommage de l'ancienne Base de Données échoué"), QObject::tr("Le renommage de l'ancienne sauvegarde a échouée !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 117:
        QMessageBox::critical(parent, QObject::tr("Sauvegarde de la Base de Données échouée"), QObject::tr("La sauvegarde automatique a échouée !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 118:
        QMessageBox::warning(parent, QObject::tr("Sauvegarde de la Base de Données échouée"), QObject::tr("La suppression de l'ancienne sauvegarde a échouée !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 119:
        QMessageBox::critical(parent, QObject::tr("Fichier de sauvegarde introuvable"), QObject::tr("Le fichier de sauvegarde créé n'a pas été trouvé !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 120:
        QMessageBox::critical(parent, QObject::tr("Erreur sauvegarde BDD échoué"), QObject::tr("Une erreur est survenue lors de l'ouverture en écrite du fichier de sauvegarde !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 121:
        QMessageBox::warning(parent, QObject::tr("Base de Données érronée"), QObject::tr("Vous ne pouvez pas importer la base actuelle.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 122:
        QMessageBox::warning(parent, QObject::tr("Suppression de l'ancienne Base de Données échouée"), QObject::tr("La suppression de l'ancienne Base de Données a échoué.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 123:
        QMessageBox::critical(parent, QObject::tr("Renommage de l'ancienne Base de Données échoué"), QObject::tr("Le renommage de l'ancienne Base de Données a échoué !") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 124:
        QMessageBox::critical(parent, QObject::tr("Importation de la Base de Données échouée"), QObject::tr("L'importation a échoué ! Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 125:
        QMessageBox::warning(parent, QObject::tr("Suppression de l'ancienne Base de Données échouée"), QObject::tr("La suppression de l'ancienne Base de Données a échoué.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 126:
        QMessageBox::information(parent, QObject::tr("Connexion SQLite échouée"), QObject::tr("Echec de connexion SQLite. Etes-vous sur que le fichier n'est pas corrompu ?") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 127:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête SQLite de récupération des données Kinés, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 128:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de transfert des données Kinés, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 129:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête SQLite de récupération des données Villes, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 130:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de transfert des données Villes, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 131:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête SQLite de récupération des données Patients actifs, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 132:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de transfert des données Patients actifs, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 133:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête SQLite de récupération des données Patients archivés, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 134:
        QMessageBox::warning(parent, QObject::tr("Transfert de Base de données échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de transfert des données Patients archivés, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 135:
        QMessageBox::warning(parent, QObject::tr("Erreur de récupération des données"), QObject::tr("Une erreur est survenue lors de la vérification de la dernière version de Kinattente. Sans doute n'êtes vous pas connecté à internet.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 136:
        QMessageBox::warning(parent, QObject::tr("Exportation de la BDD échouée"), QObject::tr("La sauvegarde de vos données a échouée.<br/>Afin de sécuriser vos données, la mise à jour est impossible sans sauvegarde au préalable.<br/>Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 200:
        QMessageBox::critical(parent, QObject::tr("Erreur démarrage Serveur"), QObject::tr("Une erreur est survenue lors du démarrage du serveur, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 300:
        QMessageBox::critical(parent, QObject::tr("Sélection kinés actifs échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de recherche des kinés actifs, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 301:
        QMessageBox::critical(parent, QObject::tr("Sélection anciens kinés échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de recherche d'anciens kinés, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 302:
        QMessageBox::critical(parent, QObject::tr("Sélection kinés actifs et non actifs échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de recherche des kinés actifs et non actifs, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 303:
        QMessageBox::critical(parent, QObject::tr("Sélection patients pour attribution du numéro de liste d'attente échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'attribution des numéro de liste d'attente, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 304:
        QMessageBox::critical(parent, QObject::tr("Sélection des étiquette de l'onglet échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de récupération des données des étiquette de l'onglet en cours, veuillez relancer le logiciel.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 400:
        QMessageBox::warning(parent, QObject::tr("Sélection Données Patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de récupération des données du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 401:
        QMessageBox::warning(parent, QObject::tr("Sélection Kinés actifs échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des kinés actifs, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 402:
        QMessageBox::warning(parent, QObject::tr("Sélection Villes échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des villes, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 403:
        QMessageBox::warning(parent, QObject::tr("Taille limite atteinte"), QObject::tr("Le nom de la ville ne peut pas excéder 40 caractères.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 404:
        QMessageBox::warning(parent, QObject::tr("Sélection Ville Doublon échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection de ville doublon, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 405:
        QMessageBox::warning(parent, QObject::tr("Ville doublon"), QObject::tr("Cette ville existe déjà dans la liste.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 406:
        QMessageBox::warning(parent, QObject::tr("Insertion Nouvelle Ville échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'insertion de la nouvelle ville, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 407:
        QMessageBox::warning(parent, QObject::tr("Sélection Ville par Défaut échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection de la ville par défaut, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 408:
        QMessageBox::warning(parent, QObject::tr("Suppression Ville échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression de la ville, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 409:
        QMessageBox::warning(parent, QObject::tr("Supression Ville échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression de la ville, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 410:
        QMessageBox::warning(parent, QObject::tr("Supression Ville échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de suppression de la ville, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 411:
        QMessageBox::warning(parent, QObject::tr("Date Incorrecte"), QObject::tr("La date d'appel doit être au format AAAA-MM-JJ.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 412:
        QMessageBox::warning(parent, QObject::tr("Téléphone 1 incorrect"), QObject::tr("Le numéro de téléphone 1 n'est pas complet.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 413:
        QMessageBox::warning(parent, QObject::tr("Téléphone 2 incorrect"), QObject::tr("Le numéro de téléphone 2 n'est pas complet.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 414:
        QMessageBox::warning(parent, QObject::tr("Formulaire incomplet"), QObject::tr("Le nom du patient ou un numéro de téléphone doivent au moins être renseignés complètement.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 415:
        QMessageBox::warning(parent, QObject::tr("Modification Patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de mise à jour du patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 416:
        QMessageBox::warning(parent, QObject::tr("Sélection de doublon échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de recherche de patient doublon, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 417:
        QMessageBox::warning(parent, QObject::tr("Insertion Nouveau Patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'insertion du nouveau patient, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 500:
        QMessageBox::warning(parent, QObject::tr("Sélection Données Patient échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des données du kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 501:
        QMessageBox::warning(parent, QObject::tr("Sélection Couleurs Kinés échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection des couleurs des kinés, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 502:
        QMessageBox::critical(parent, QObject::tr("Couleur Réservée"), QObject::tr("La couleur est indisponible, veuillez changer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 503:
        QMessageBox::warning(parent, QObject::tr("Insertion Nouveau Kiné échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'insertion du nouveau kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 504:
        QMessageBox::warning(parent, QObject::tr("Modification Kiné échoué"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de mise à jour du kiné, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 600:
        QMessageBox::warning(parent, QObject::tr("Sélection Ville par Défaut échouée"), QObject::tr("Une erreur a eu lieu lors de la sélection de la ville par défaut, veuillez relancer les options.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 601:
        QMessageBox::critical(parent, QObject::tr("Adresse IP invalide"), QObject::tr("Vous ne pouvez pas changer d'onglet tant que l'adresse IP n'est pas valide.<br/>Si vous souhaitez annuler la modification, cliquez sur annuler.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 602:
        QMessageBox::critical(parent, QObject::tr("Ville par défaut invalide"), QObject::tr("Vous ne pouvez pas changer d'onglet tant que le champ de la ville par défaut est vide.<br/>Si vous souhaitez annuler la modification, cliquez sur annuler.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 603:
        QMessageBox::warning(parent, QObject::tr("Sélection Ville Doublon échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de sélection de ville doublon, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 604:
        QMessageBox::warning(parent, QObject::tr("Modification Ville par défaut échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL d'actualisation du nom de la ville par défaut, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 605:
        QMessageBox::warning(parent, QObject::tr("Changement de la durée de conservation des données patients échouée"), QObject::tr("Une erreur a eu lieu lors de la requête MySQL de changement de la durée de conservation des données patients, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 700:
        QMessageBox::warning(parent, QObject::tr("Erreur de téléchargement"), QObject::tr("Une erreur est survenue lors de la lecture de la page internet du lien de la mise à jour. Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 701:
        QMessageBox::warning(parent, QObject::tr("Téléchargement impossible"), QObject::tr("Il n'y a pas de fichier à télécharger, la mise à jour est annulée."));
        break;

        case 702:
        QMessageBox::warning(parent, QObject::tr("Téléchargement échoué"), QObject::tr("Le fichier téléchargé n'est pas conforme, la mise à jour a échoué, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 703:
        QMessageBox::warning(parent, QObject::tr("Lancement Mise à Jour échouée"), QObject::tr("Le lancement de la mise à jour a échoué, veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 800:
        QMessageBox::warning(parent, QObject::tr("Copie Ancienne Base de Données échouée"), QObject::tr("La copie de l'ancienne Base de Données sur le bureau a échoué. Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 801:
        QMessageBox::warning(parent, QObject::tr("Suppression Ancienne Base de Données échouée"), QObject::tr("La suppression de l'ancienne Base de Données sur le bureau a échoué. Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 802:
        QMessageBox::warning(parent, QObject::tr("Importation Base de Données échouée"), QObject::tr("L'importation a échoué. Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 803:
        QMessageBox::warning(parent, QObject::tr("Database Inconnue"), QObject::tr("MySQL n'est pas installé sur le poste principal<br/>Veuillez commencer par installer correctement le poste principal.") + "<br/>Error #" + QString::number(errorCode) + "<br/>" + errorText);
        break;

        case 804:
        QMessageBox::warning(parent, QObject::tr("Mot de Passe Incorrect"), QObject::tr("Le mot de passe ne peut être nul ! Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 805:
        QMessageBox::warning(parent, QObject::tr("Adresse IP Invalide"), QObject::tr("L'adresse IP saisie n'est pas conforme ! Veuillez recommencer.") + "<br/>Error #" + QString::number(errorCode));
        break;

        case 901:
        QMessageBox::critical(parent, QObject::tr("Création de la Base de Données échouée"), QObject::tr("La création de la Base de Données a échoué ! Cliquez à nouveau sur \"<strong>Un seul poste</strong>\". Si le problème persiste relancer Kinattente.") + "<br/>Error #" + QString::number(errorCode));
        break;

        default:
        ;
    }
    logEntry(QString::number(errorCode));
}

bool compareVersion(QString lastVersion)
{
    QSettings settings;
    if(!lastVersion.contains(QRegExp(R"(^\d+(\.\d+)+$)")))
        return false;
    QStringList const lastVersionNumbers { lastVersion.split(".") };
    QStringList const kinattenteVersionNumbers { settings.value("Version").toString().split(".") };

    for(int i { 0 }; i < lastVersionNumbers.size(); i++)
        if(lastVersionNumbers[i] > kinattenteVersionNumbers[i])
            return true;
    return false;
}

QByteArray createPaquet(QString message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));
    return paquet;
}

LinkedPicture::LinkedPicture(QSize size, QString picture, QString link, QWidget *parent) : QLabel(parent), m_link(link)// Constructeur
{
    QPixmap const logoPixmap { QPixmap(picture) };
    setPixmap(logoPixmap);
    setScaledContents(true);
    setFixedSize(size);
    setCursor(Qt::PointingHandCursor);
}

LinkedPicture::~LinkedPicture()// Destructeur
{

}

void LinkedPicture::setLink(const QString & text)// Méthode pour changer le lien de l'image
{
    m_link = text;
}

void LinkedPicture::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if(m_link == "Update")
        emit launchUpdate();
    else
        QDesktopServices::openUrl(QUrl(m_link));
}

CustomWidget::CustomWidget(QString display, bool checkBox, QWidget *parent) : QDialog(parent)// Constructeur
{
    m_logo = new LinkedPicture(QSize(150, 150), "Images/Logo.png", "https://kinattente.wordpress.com/", this);
    QPixmap const logoPixmap { QPixmap("Images/Kinattente.png") };
        m_title.setPixmap(logoPixmap);
        m_title.setScaledContents(true);
        m_title.setFixedSize(200, 50);

    m_display.setText(display);
    m_display.setOpenExternalLinks(true);

    m_checkBox.setText("Cochez cette case pour ne plus voir apparaître cette fenêtre au démarrage");
    if(!checkBox)
        m_checkBox.hide();

    m_closeButton.setText(tr("Fermer"));

    setLayout(&m_HLayout);
    m_HLayout.addWidget(m_logo);
    m_HLayout.addLayout(&m_VLayout);
        m_VLayout.addWidget(&m_title);
        m_VLayout.addWidget(&m_display);
        m_VLayout.addWidget(&m_checkBox);
        m_VLayout.addWidget(&m_closeButton, 0, Qt::AlignRight);

    connect(&m_closeButton, &QPushButton::clicked, this, &CustomWidget::closeButtonClicked);
}

CustomWidget::~CustomWidget()// Destructeur
{

}

void CustomWidget::closeButtonClicked()// Méthode pour fermer la fenêtre à propos
{
    QSettings settings;
    if(m_checkBox.isChecked())
        settings.setValue("lastVersionScreen", false);
    close();
}

CustomSqlModel::CustomSqlModel(int index, QObject *parent) : QSqlQueryModel(parent), m_index(static_cast<enum index>(index))
{

}

CustomSqlModel::~CustomSqlModel()// Destructeur
{

}

QVariant CustomSqlModel::data(const QModelIndex &index, int role) const
{
    QVariant value { QSqlQueryModel::data(index, role) };
    if (value.isValid() && role == Qt::DisplayRole)
    {
        if(m_index == Editor)
        {
            if (index.column() == 2)
                return QString();
        }

        if (index.column() == 0)
            return value.toString().replace("Office", tr("Cabinet")).replace("Home", tr("Domicile"));
        if (index.column() == 1)
        {
            QList<QString> const genderList { "", tr("Mr"), tr("Mme"), tr("Enft") };
            return genderList[value.toString().leftRef(1).toInt()] + value.toString().remove(0, 1);
        }
        if (index.column() == 3)
            return value.toDouble() > 1 || value.toDouble() == 0 ? value.toString() + " ans" : QString::number(static_cast<int>(value.toDouble() * 100)) + " mois";
        if (index.column() == 12)
            return value.toDate().toString();
    }
    if (role == Qt::BackgroundRole && index.column() == 2 && m_index == Editor)
        return QVariant::fromValue(QColor(QSqlQueryModel::data(index, Qt::DisplayRole).toString()));
    return value;
}
