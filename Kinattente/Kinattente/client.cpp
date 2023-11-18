#include "client.h"

Client::Client(QObject *parent) : QObject(parent), m_clientConnected { false }, m_messageSize { 0 }// Constructeur
{
    m_socket = new QTcpSocket(this);
}

Client::~Client()// Destructeur
{

}

void Client::connectClient(QString host, quint16 port)// Méthode pour connecter le client
{
    Q_ASSERT(!host.isEmpty());
    Q_ASSERT(port);

    QEventLoop loop;
    connect(m_socket, &QTcpSocket::connected, &loop, &QEventLoop::quit);
    connect(m_socket, &QTcpSocket::connected, this, &Client::connected);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::receivedData);
    m_socket->abort(); // On désactive les connexions précédentes s'il y en a
    m_socket->connectToHost(host, port); // On se connecte au serveur demandé
    if(m_socket->state() != QAbstractSocket::ConnectedState)
        loop.exec();
}

bool Client::isClientConnected()// Méthode publique indiquant si le client est bien connecté
{
    return m_clientConnected;
}

void Client::connected()// Slot pour supprimer la fenêtre modale de connexion qui bloque le programme
{
    m_clientConnected = true;
    sendMessage("MasterWorkplaceVersion");
    qDebug() << "Connected to server successfully!";
}

void Client::sendMessage(const QString &message)// Slot permettant d'envoyer un message au serveur lors d'une actualisation de l'affichage
{
    Q_ASSERT(!message.isEmpty());
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    m_socket->write(paquet);
}

void Client::receivedData()// Slot en cas de reception de paquets
{
    Q_CHECK_PTR(m_socket);

    QDataStream in(m_socket);
    do
    {
        if (!m_messageSize)
        {
            if (m_socket->bytesAvailable() < (int)sizeof(quint16))
                 return;
            in >> m_messageSize;
        }

        if (m_socket->bytesAvailable() < m_messageSize)
            return;

        QString messageRecu;
        in >> messageRecu;

        QSettings const settings;
        if(messageRecu.contains("MasterWorkplaceVersion"))
        {
            messageRecu.replace("MasterWorkplaceVersion#", "");
            if(settings.value("Version").toDouble() < messageRecu.toDouble())
                emit update();
        }
        else
            emit refreshReceived(messageRecu);

        m_messageSize = 0;
    }
    while(m_socket->bytesAvailable() > (int)sizeof(quint16));
}

void Client::socketError(QAbstractSocket::SocketError erreur)// Slot pour afficher les erreurs
{
    QSettings settings;
    m_clientConnected = false;
    qDebug() << "Connection to server failed :(";
    switch(erreur)
    {
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::critical(nullptr, "Erreur de connexion", tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::critical(nullptr, "Erreur de connexion", tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            QMessageBox::critical(nullptr, "Erreur de connexion", tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
            settings.setValue("CloseEvent", "ConnexionError #2");
            qApp->quit();
            break;
        case QAbstractSocket::SocketTimeoutError:
            QMessageBox::critical(nullptr, "Erreur de connexion", tr("<em>ERREUR : un temps trop long s'est écoulé.</em>"));
            settings.setValue("CloseEvent", "ConnexionError #3");
            qApp->quit();
            break;
        default:
            QMessageBox::critical(nullptr, "Erreur de connexion", tr("<em>ERREUR : ") + m_socket->errorString() + tr("</em>"));
            settings.setValue("CloseEvent", "ConnexionError #4");
            qApp->quit();
    }
}
