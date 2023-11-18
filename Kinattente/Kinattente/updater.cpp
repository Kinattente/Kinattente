#include "updater.h"

Updater::Updater(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *mainLayout { new QVBoxLayout };
    QLabel *text { new QLabel("Téléchargement de la mise à jour en cours...<br/>Toute interruption annulera simplement la mise à jour sans conséquence ni sur vos données, ni sur le logiciel.") };
    QLabel *fichier { new QLabel("<i>Updater.exe</i>") };
    m_cancelButton.setText(tr("Annuler"));
    setLayout(mainLayout);
    mainLayout->addWidget(text);
    mainLayout->addWidget(&m_progressBar);
    mainLayout->addWidget(fichier);
    mainLayout->addWidget(&m_cancelButton, 0, Qt::AlignRight);

    QNetworkReply *reply { m_manager.get(QNetworkRequest(QUrl("https://kinattente.wordpress.com/data/"))) };
    Q_ASSERT(reply);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, [this] { QMessageBox::information(this, tr("Temps d'attente dépassé"), tr("Le téléchargement de la mise à jour a été arrêté en raison d'un temps de téléchargement trop long. Veuillez recommencer."));});
    timer.start(60000);
    loop.exec();
    timer.stop();

    if(reply->error() != QNetworkReply::NoError)
        error(700, nullptr, reply->errorString());
    else
    {
        QString data(reply->readAll());
        reply->deleteLater();

        QString downloadLink { data };
            downloadLink.replace(QRegExp(R"(^.*Lien de téléchargement updater : \#([^\#]*)\#.*$)"), R"(\1)");

        m_md5checking = data.replace(QRegExp(R"(^.*Fichier md5 correspondant : \#([^\#]*)\#.*$)"), R"(\1)");

        if(downloadLink.isEmpty())
        {
            error(701, this);
            QTimer::singleShot(200, this, &Updater::close);
            return;
        }

        m_reply = m_manager.get(QNetworkRequest(QUrl(downloadLink)));
        connect(m_reply, &QNetworkReply::downloadProgress, this, &Updater::downloadProgression);
        connect(m_reply, &QNetworkReply::finished, this, &Updater::write);
        connect(&m_cancelButton, &QPushButton::clicked, this, &Updater::cancelButtonClicked);
    }
}

Updater::~Updater()
{

}

void Updater::cancelButtonClicked()// Slot pour annuler le téléchargement
{
    m_reply->deleteLater();
    m_reply->disconnect();
    close();
}

void Updater::downloadProgression(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal != -1)
    {
        m_progressBar.setRange(0, bytesTotal);
        m_progressBar.setValue(bytesReceived);
    }
}

void Updater::write()
{
    m_reply->deleteLater();

    QByteArray const data { m_reply->readAll() };
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(data);
    QByteArray hah { md5.result() };
    QString const md5Text { hah.toHex() };

    if(md5Text != m_md5checking)
    {
        error(702, this);
        close();
        return;
    }

    QFile lastversion("updater.exe");

    if ( lastversion.open(QIODevice::WriteOnly) )
    {
        lastversion.write(data);
        lastversion.close();
        QMessageBox::information(this, tr("Téléchargement de la mise à jour"), tr("Téléchargement de la mise à jour terminée !"));

        QProcess *kinattente { new QProcess(this) };
        kinattente->setProgram("updater.exe");
        if(!kinattente->startDetached())
            error(703, this);
        kinattente->waitForStarted();
        QSettings settings;
        settings.setValue("CloseEvent", "Update");
        qApp->quit();
    }
    close();
}
