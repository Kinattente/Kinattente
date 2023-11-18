#ifndef UPDATER_H
#define UPDATER_H

#include "usualfonctions.h"
#include <QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QProgressBar>
#include <QEventLoop>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QLabel>
#include <QPushButton>

class Updater : public QDialog
{
    Q_OBJECT

    public:
    Updater(QWidget *parent = nullptr);
    ~Updater();

    private:
    void cancelButtonClicked();// Slot pour annuler le téléchargement
    QProgressBar m_progressBar;
    QNetworkReply *m_reply;
    QNetworkAccessManager m_manager;
    QPushButton m_cancelButton;
    QString m_md5checking;

    private slots:
    void downloadProgression(qint64 bytesReceived, qint64 bytesTotal);
    void write();
};

#endif // UPDATER_H
