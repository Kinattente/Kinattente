#ifndef OPTIONS_H
#define OPTIONS_H

#include "configassistant.h"
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFileDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDirIterator>
#include <QStandardItemModel>
#include <QStackedLayout>
#include <QGraphicsDropShadowEffect>

class Options : public QDialog
{
    Q_OBJECT

    public:
    Options(QWidget *parent = nullptr);// Constructeur
    Options(Options const&) = delete;// Constructeur de copie
    Options& operator=(Options const&) = delete;// Opérateur de copie
    ~Options();// Destructeur

    signals:
    void closed();
    void ipChanged();
    void css();
    void exportBDD(bool choosePath);
    void refreshTransmission(QString const message);// Signal pour émission d'une MàJ de la BDD


    private slots:
    void optionsChange(const QModelIndex &index);// Slot pour changer les options à afficher
    void ipInvalid(const QModelIndex &index);// Slot pour bloquer le changement de page option si l'adresse ip n'est pas valide
    void cityInvalid(const QModelIndex &index);// Slot pour bloquer le changement de page option si la ville par défaut n'est pas valide
    void IPAddressLineEditEdited(const QString & text);// Slot pour modifier l'adresse IP
    void saveButtonClicked();// Slot pour activer / désactiver la sauvegarder automatique
    void savePathButtonClicked();// Slot pour changer l'emplacement de la sauvegarde
    void cssButtonClicked();// Slot pour ajouter un fichier css
    void cssCurrentIndexChanged(int index);// Slot pour changer de fichier css
    void cityLineEditEdited(const QString &text);// Slot pour changer le nom de la ville par défaut
    void bootCheckBoxClicked(int state);// Slot pour ajouter / retirer Kinattente au démarrage
    void dataSpinBoxChanged(int i);// Slot pour changer la durée de conservation des données patients
    void multiWorkplaceButtonClicked();// Slot pour créer un fichier SQL à partir de SQLITE
    void acceptButtonClicked();// Slot pour valider tous les changements et quitter les options
    void cancelButtonClicked();// Slot pour quitter les options sans changement
    void applyButtonClicked();// Slot pour valider tous les changements

    private:
    QStandardItemModel m_listModel;
    QListView m_listView;

    QStackedLayout m_stackedLayout;
    QVBoxLayout m_ipAddressLayout;
    QVBoxLayout m_saveLayout;
    QVBoxLayout m_cssLayout;
    QVBoxLayout m_cityLayout;
    QVBoxLayout m_bootLayout;
    QVBoxLayout m_dataLayout;
    QVBoxLayout m_multiWorkplaceLayout;

    bool m_applyIPAddress;
    bool m_applySaveOption;
    bool m_applySavePath;
    bool m_applyCSS;
    bool m_applyCity;
    bool m_applyBoot;
    bool m_applyData;

    QLineEdit m_IPAddressLineEdit;
    QString m_IPAddress;
    QPushButton m_saveButton;
    QLineEdit m_savePath;
    QComboBox m_cssFileList;
    QLineEdit m_cityLineEdit;
    QLabel m_cityDisplay;
    QCheckBox m_bootCheckBox;
    QSpinBox m_dataSpinBox;
    QLabel m_dataDisplay;

    QPushButton m_acceptButton;
    QPushButton m_cancelButton;
    QPushButton m_applyButton;
};

#endif // OPTIONS_H
