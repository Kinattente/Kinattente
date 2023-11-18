#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "mainwindow.h"
#include "configassistant.h"
#include "server.h"
#include "client.h"
#include <QObject>
#include <QtSql>// Pour la connexion à la DataBase
#include <QMessageBox>// Pour l'alerte en cas d'échec de connexion

class Connections : public QObject
{
    Q_OBJECT

    public:
    explicit Connections(QObject *parent = nullptr);// Constructeur
    Connections(Connections const&) = delete;// Constructeur de copie
    Connections& operator=(Connections const&) = delete;// Opérateur de copie
    ~Connections();// Destructeur

    private:
    MainWindow *m_mainwindow;
};

#endif // CONNECTIONS_H
