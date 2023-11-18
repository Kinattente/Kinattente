#ifndef USUALFONCTIONS_H
#define USUALFONCTIONS_H


#include <QWidget>
#include <QString>
#include <QFileInfo>
#include <QDirIterator>
#include <QSqlDatabase>
#include <QHBoxLayout>
#include <QSettings>
#include <QPushButton>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QInputDialog>
#include <QFileDialog>
#include <QSqlQueryModel>
#include <QDate>
#include <QCheckBox>

QString pathToDirectory(QString file);

void removeDatabase(QString connectionName);

bool connectToDatabase(QString ipAddress = QString());

bool pendingAction(QString title = QString(), QString text = QString());

QByteArray createPaquet(QString message);

void logEntry(QString text);

void error(int errorCode, QWidget *parent = nullptr, QString errorText = QString());

bool compareVersion(QString lastVersion);

class LinkedPicture : public QLabel
{
    Q_OBJECT

    public:
    LinkedPicture(QSize size, QString picture, QString link, QWidget *parent = nullptr);// Constructeur
    ~LinkedPicture();// Destructeur
    void setLink(const QString & text);// Méthode pour changer le lien de l'image

    signals:
    void launchUpdate(bool const forcingUpdate = false);// Signal pour lancer la recherche de nouvelle mise à jour

    private:
    void mouseReleaseEvent(QMouseEvent* event) override;

    protected:
    QString m_link;
};

class CustomWidget : public QDialog
{
    Q_OBJECT

    public:
    explicit CustomWidget(QString display, bool checkBox = false, QWidget *parent = nullptr);// Constructeur
    CustomWidget(CustomWidget const&) = delete;// Constructeur de copie
    CustomWidget& operator=(CustomWidget const&) = delete;// Opérateur de copie
    ~CustomWidget();// Destructeur


    private slots:
    void closeButtonClicked();// Méthode pour fermer la fenêtre à propos

    protected:
    LinkedPicture *m_logo;
    QLabel m_title;
    QLabel m_display;
    QCheckBox m_checkBox;
    QPushButton m_closeButton;
    QHBoxLayout m_HLayout;
    QVBoxLayout m_VLayout;
};

class CustomSqlModel : public QSqlQueryModel
{
    Q_OBJECT

    public:
    CustomSqlModel(int index, QObject *parent = nullptr);
    ~CustomSqlModel();// Destructeur
    QVariant data(const QModelIndex &item, int role) const override;
    enum index {Archives, Editor };

    protected:
    index m_index;
};

#endif // USUALFONCTIONS_H
