#ifndef SERVER_H
#define SERVER_H

#include "usualfonctions.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QObject>
#include <QSettings>

class Server : public QObject
{
    Q_OBJECT

    public:
    explicit Server(QObject *parent = nullptr);// Constructeur
    Server(Server const&) = delete;// Constructeur de copie
    Server& operator=(Server const&) = delete;// Opérateur de copie
    ~Server();// Destructeur
    bool isServerListening();// Méthode publique indiquant si le serveur s'est bien lancé

    public slots:
    void sendToAll(const QString &message);// Méthode pour envoyer les messages reçus à tous les connectés (sauf l'émetteur)

    private slots:
    void newConnection();// Slot en cas de nouvelle connexion client
    void receivedData();// Slot en cas de reception de paquets
    void clientDisconnection();// Slot en cas de déconnexion client

    private:
    QTcpServer *m_tcpServer;
    bool m_serverListening;
    QList<QTcpSocket *> m_clients;
    quint16 m_messageSize;
};

#endif // SERVER_H
