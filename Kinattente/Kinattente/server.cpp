#include "server.h"

Server::Server(QObject *parent) : QObject(parent), m_messageSize{0}// Constructeur
{
    // Gestion du serveur
    m_tcpServer = new QTcpServer(this);
    m_tcpServer->listen(QHostAddress::Any, 50885);
    if(!m_tcpServer->isListening())
    {
        error(200, nullptr, m_tcpServer->errorString());
        qDebug() << "Server started failed :(";
        m_serverListening = false;
    }
    else
    {
        qDebug() << "Server started successfully!";
        connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);
        m_serverListening = true;
    }
}

Server::~Server()// Destructeur
{

}

bool Server::isServerListening()// Méthode publique indiquant si le serveur s'est bien lancé
{
    return m_serverListening;
}

void Server::sendToAll(const QString &message)// Méthode pour envoyer les messages reçus à tous les connectés
{
    Q_ASSERT(!message.isEmpty());
    QTcpSocket const *socket { qobject_cast<QTcpSocket *>(sender()) };
    if (!socket)
        return;

    QByteArray paquet { createPaquet(message) };

    for(auto client : qAsConst(m_clients))
    {
        Q_ASSERT(client);
        client->write(paquet);
    }
}

void Server::newConnection()// Slot en cas de nouvelle connexion client
{
    QTcpSocket *nouveauClient { m_tcpServer->nextPendingConnection() };
    m_clients << nouveauClient;
    qDebug() << "New Client connected!";

    // On informe le nouveau client de l'état des actions en cours
    QSettings const settings;
    QString message { "PendingAction#" + settings.value("PendingAction").toString() + "#" + m_clients.indexOf(nouveauClient) };

    QByteArray paquet { createPaquet(message) };

    nouveauClient->write(paquet);

    connect(nouveauClient, &QTcpSocket::readyRead, this, &Server::receivedData);
    connect(nouveauClient, &QTcpSocket::disconnected, this, &Server::clientDisconnection);
}

void Server::receivedData()// Slot en cas de reception de paquets
{
    QTcpSocket *socket { qobject_cast<QTcpSocket *>(sender()) };
    if (!socket)
        return;

    QDataStream in(socket);
    do
    {
        if (!m_messageSize)
        {
            if (socket->bytesAvailable() < (int)sizeof(quint16))
                 return;
            in >> m_messageSize;
        }

        if (socket->bytesAvailable() < m_messageSize)
            return;

        QString message;
        in >> message;

        QSettings const settings;
        if(message == "MasterWorkplaceVersion")
        {
            message = "MasterWorkplaceVersion#" + settings.value("Version").toString();
            QByteArray paquet { createPaquet(message) };
            socket->write(paquet);
        }
        else
            sendToAll(message);
        m_messageSize = 0;
    }
    while(socket->bytesAvailable() > (int)sizeof(quint16));
}

void Server::clientDisconnection()// Slot en cas de déconnexion client
{
    QTcpSocket *socket { qobject_cast<QTcpSocket *>(sender()) };
    if (!socket)
        return;

    QSettings const settings;
    if(settings.value("PendingAction").toInt() == m_clients.indexOf(socket))
        sendToAll("PendingAction#-1");

    m_clients.removeOne(socket);
    socket->deleteLater();
}
