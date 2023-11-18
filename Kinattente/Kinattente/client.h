#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QSettings>

class Client : public QObject
{
    Q_OBJECT

    public:
    explicit Client(QObject *parent = nullptr);// Constructeur
    Client(Client const&) = delete;// Constructeur de copie
    Client& operator=(Client const&) = delete;// Opérateur de copie
    ~Client();// Destructeur
    void connectClient(QString host, quint16 port);// Méthode pour connecter le client
    bool isClientConnected();// Méthode publique indiquant si le client est bien connecté

    signals:
    void refreshReceived(QString const &message);// Signal émit lors de la réception d'un message pour actualisation de l'affichage
    void update();// Signal émit quand le client reçoit la notification de la MàJ du pc maître

    public slots:
    void connected();// Slot pour supprimer la fenêtre modale de connexion qui bloque le programme
    void sendMessage(QString const &message);// Slot permettant d'envoyer un message au serveur lors d'une actualisation de l'affichage

    private slots:
    void receivedData();// Slot en cas de reception de paquets
    void socketError(QAbstractSocket::SocketError erreur);// Slot pour afficher les erreurs

    private:
    QTcpSocket *m_socket;
    bool m_clientConnected;
    quint16 m_messageSize;
    QMessageBox *m_connectionDialog;
};

#endif // CLIENT_H
