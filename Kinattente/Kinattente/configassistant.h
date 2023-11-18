#ifndef CONFIGASSISTANT_H
#define CONFIGASSISTANT_H

#include "usualfonctions.h"
#include <QWizard>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QVariant>
#include <QSettings>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtSql>
#include <QProcess>
#include <QCloseEvent>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QDesktopServices>
#include <QTextBrowser>

class ConfigAssistant : public QWizard
{
    Q_OBJECT

    enum {Page_Intro, Page_BDD, Page_MySQLInstall, Page_MySQLFinal, Page_IPAddress, Page_Save, Page_Final};

    public:
    explicit ConfigAssistant(QWidget *parent = nullptr);// Constructeur
    ConfigAssistant(ConfigAssistant const&) = delete;// Constructeur de copie
    ConfigAssistant& operator=(ConfigAssistant const&) = delete;// Opérateur de copie
    ~ConfigAssistant();// Destructeur
    int nextId() const override;// Réimplémentation de la fonction nextId()

    signals:
    void closed();
    void finished();// Signal signalant que l'assistant de configuration est terminé

    private:
    void closeEvent(QCloseEvent *event) override;
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

    public:
    IntroPage(QWidget *parent = nullptr);

    private slots:
    void surpriseButtonClicked();

    private:
    bool isComplete() const override;
    bool m_surpriseButtonClicked;
    QTextBrowser m_textBrowser;
    QPushButton m_surpriseButton;
};

class BDDPage : public QWizardPage
{
    Q_OBJECT

    public:
    BDDPage(QWidget *parent = nullptr);

    private slots:
    void WorkplaceChecked(bool checked);

    private:
    bool validatePage() override;
    void cleanupPage() override;
    bool isComplete() const override;
    bool m_sqliteAlreadyCopied;
    QButtonGroup m_numberGroup;
    QRadioButton m_uniqueWorkplace;
    QRadioButton m_multiWorkplace;
    QButtonGroup m_typeGroup;
    QRadioButton m_masterWorkplace;
    QRadioButton m_slaveWorkplace;
    QLabel m_mysqlPathDescription;
    QLabel m_masterDescription;
    QLabel m_slaveDescription;
    QLabel m_uniqueDescription;
    QLabel m_multiDescription;
};

class MySQLInstallPage : public QWizardPage
{
    Q_OBJECT

    public:
    MySQLInstallPage(QWidget *parent = nullptr);

    private slots:
    void launchMySQLInstall();
    void launchVCRedistInstall();

    private:
    void cleanupPage() override;
    bool isComplete() const override;
    QPushButton m_mysqlInstallationButton;
    bool m_vcRedistInstallOk;
};

class MySQLFinalPage : public QWizardPage
{
    Q_OBJECT

    public:
    MySQLFinalPage(QWidget *parent = nullptr);

    private slots:
    void mysqlCheckUpClicked();

    private:
    void cleanupPage() override;
    bool isComplete() const override;
    bool sqLiteMySQLTransfert(QSqlDatabase dbKinattente);
    QPushButton m_mysqlCheckUpButton;
    QLineEdit m_mysqlBinDirPath;
    QLineEdit m_password;
    QTextEdit m_mysqlCheckUpProgress;
    QString m_binDir;
    bool m_mysqlDatabaseChecked;
};

class IPAddressPage : public QWizardPage
{
    public:
    IPAddressPage(QWidget *parent = nullptr);

    private:
    void cleanupPage() override;
    bool isComplete() const override;
    void connectionTestClicked();
    bool m_connectionOk;
    QPushButton m_connectionTestButton;
    QLabel m_connectionTestLabel;
};

class SavePage : public QWizardPage
{
    Q_OBJECT

    public:
    SavePage(QWidget *parent = nullptr);

    signals:
    void textChanged();

    private slots:
    void saveChecked(bool checked);
    void savePathClicked();

    private:
    void cleanupPage() override;
    bool isComplete() const override;
    QRadioButton m_noSave;
    QRadioButton m_autoSave;
    QPushButton m_savePathButton;
    QCheckBox m_launchCheckbox;
    QLineEdit m_savePathLineEdit;
    QLabel m_savePathLabelText;
    QLabel m_noSaveLabelText;
    QLabel m_autoSaveLabelText;
    QButtonGroup m_saveGroup;
};

class FinalPage : public QWizardPage
{
    public:
    FinalPage(QWidget *parent = nullptr);

    private:
    void initializePage() override;
    bool validatePage() override;
    QLabel m_IPAddressText;
    QLineEdit m_IPAddress;
};

#endif // CONFIGASSISTANT_H
