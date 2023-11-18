#include "connections.h"

QStringList g_identifiants;// Contient les lignes du fichier connexion.txt

Connections::Connections(QObject *parent) : QObject(parent)// Constructeur
{
    // Assistant de configuration à la première ouverture du logiciel Kinattente
    QSettings settings;
    if(!settings.contains("Assistant"))
    {
        Assistant *AssistantWindow { new Assistant };
        AssistantWindow->show();
        QEventLoop loop;
        connect(AssistantWindow, &Assistant::finished, &loop, &QEventLoop::quit);
        loop.exec();
        delete AssistantWindow;
        settings.setValue("Assistant", true);
    }

    // Connexion à la DataBase
    settings.beginGroup("BDD");
    QSqlDatabase db = QSqlDatabase::addDatabase(settings.value("Database").toString());
    db.setDatabaseName(settings.value("DatabaseName").toString());
    if(settings.value("Database").toString() == "QODBC")
    {
        db.setHostName(settings.value("HostName").toString());
        db.setUserName(settings.value("UserName").toString());
        db.setPassword("MSPUBELAIR");
    }
    settings.endGroup();

    if(!db.open())
        QMessageBox::critical(nullptr, QObject::tr("Erreur Base de Données"), "Le logiciel n'a pas pu se connecteur au serveur contenant toutes les données de la liste d'attente.<br/>Veuillez recommencer.<br/>" + db.lastError().text());
    else
    {
        // Test de connexion à un serveur existant distant ou alors lancement du serveur et connexion en tant que client local
        m_mainwindow = new MainWindow;
        if(settings.value("BDD/IPAdress").toString() == "127.0.0.1")
        {
            Server *Server { new Server(this) };
            if(Server->etatServer())
            {
                // Affichage de la fenêtre principale
                m_mainwindow->showMaximized();

                Client *client { new Client(this) };
                client->connexion(settings.value("BDD/IPAdress").toString(), settings.value("BDD/Port").toInt());
                connect(client, &Client::fermerProgramme, qApp, &QApplication::quit);
                connect(m_mainwindow, &MainWindow::actualisationEmission, client, &Client::envoyerMessage);
                connect(client, &Client::actualisationReception, m_mainwindow, &MainWindow::actualisationReception);
            }
        }
        else
        {
            // Affichage de la fenêtre principale
            m_mainwindow->showMaximized();
            qApp->quit();
            Client *client { new Client(this) };
            client->connexion(settings.value("BDD/IPAdress").toString(), settings.value("BDD/Port").toInt());
            connect(client, &Client::fermerProgramme, qApp, &QApplication::quit);
            connect(m_mainwindow, &MainWindow::actualisationEmission, client, &Client::envoyerMessage);
            connect(client, &Client::actualisationReception, m_mainwindow, &MainWindow::actualisationReception);
        }
    }
}

Connections::~Connections()// Destructeur
{

}

