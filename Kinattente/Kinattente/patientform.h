#ifndef PATIENTFORM_H
#define PATIENTFORM_H

#include "overlapform.h"
#include "usualfonctions.h"
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTimeEdit>
#include <QTextEdit>
#include <QSqlField>
#include <QFormLayout>
#include <QTimer>
#include <QInputDialog>
#include <QSettings>
#include <QSqlError>
#include <QGraphicsDropShadowEffect>
#include <QSpinBox>

/*
 * Classe qui ouvre un formulaire pour ajouter un patient sur la liste d'attente
 * Propose des accesseurs pur toutes les données remplies dans le formulaire
 */

class PatientForm : public QDialog
{
    Q_OBJECT

    public:
    PatientForm(int const &id = 0, QWidget *parent = nullptr);// Constructeur surchargé pour modification de données
    ~PatientForm();// Destructeur
    QMap<QString, QString> accessor() const;// Accesseur
    QHash<QString, QVariant> patientData();// Accesseur

    signals:
    void refreshTransmission(QString const message);// Signal pour émission d'une MàJ de la BDD

    private slots:
    void alternationLocationButtons(bool alternance);// Slot pour masquer / démasquer de l'adresse du patient et alternance bouton Cabinet / Domicile
    void alternationYearMonthButtons();// Slot pour alterner ans et mois sur le bouton associé
    void nameFieldFormatting(const QString & text);// Slot pour formater correctement le champ du nom
    void firstnameFieldFormatting(const QString & text);// Slot pour formater correctement le champ du prénom
    void phone1FieldLightening(const QString & text);// Slot pour surligner le champ téléphone 1
    void phone2FieldLightening(const QString & text);// Slot pour surligner le champ téléphone 2
    void emailFieldLightening(const QString & text);// Slot pour surligner le champ email
    void doctorFieldFormatting(const QString & text);// Slot pour formater correctement le médecin
    void enableWeek(int state);// Slot pour griser / dégriser les jours de la semaine
    void enableDay(int state);// Slot pour griser / dégriser le matin et l'après-midi
    void enableHalfDay(int state);// Slot pour griser / dégriser les CheckBox du matin et de l'après-midi
    void enableHours(int state);// Slot pour griser / dégriser les horaires de disponibilité
    void enableOther(int state);// Slot pour griser / dégriser le champ personnalisé de disponibilité
    void pathologyFieldFormatting();// Slot pour formater correctement les champs de téléphone
    void calldateFieldLightening(const QString & text);// Slot pour surligner le champ date d'appel
    void addressChanged(const QString & text);// Slot pour actualiser le lien Google Maps
    void cityAddButtonClicked();// Slot pour ajouter une nouvelle ville
    void cityWithdrawButtonClicked();// Slot pour retirer une ville
    void acceptButtonClicked();// Slot pour fermer le formulaire après vérification du bon remplissage des champs

    protected:
    void kineListFilling(QString const &name, QString const &firstname);// Méthode pour remplir la QComboBox avec les infos kinés
    void cityListFilling(QString const &name);// Méthode pour remplir la QComboBox avec les villes
    void availabilityFilling(QString const &availability);// Méthode pour remplir la disponibilité

    int id() const;// Accesseur
    bool location() const;// Accesseur
    int gender() const;// Accesseur
    double age() const;// Accesseur
    QString name() const;// Accesseur
    QString firstname() const;// Accesseur
    QString phone1() const;// Accesseur
    QString phone2() const;// Accesseur
    QString email() const;// Accesseur
    QString doctor() const;// Accesseur
    int kine() const;// Accesseur
    QString availability() const;// Accesseur
    QString address() const;// Accesseur
    int city() const;// Accesseur
    QString pathology() const;// Accesseur
    bool no_emergency() const;// Accesseur
    QString callDate() const;// Accesseur

    int m_id;
    bool m_originLocation;
    QString m_originCallDate;
    QMap<QString, QString> m_accessor;
    QPushButton m_officeButton;
    QPushButton m_homeButton;

    QList<QRadioButton*> m_gender;
    QSpinBox m_age;
    QPushButton m_yearmonth;
    QLineEdit m_name;
    QLineEdit m_firstname;
    QLineEdit m_phone1;
    QLineEdit m_phone2;
    QLineEdit m_email;
    QLineEdit m_doctor;
    QComboBox m_kine;
    QMap<int, QString> m_kineMap;

    QCheckBox m_week;
    QCheckBox m_monday;
    QCheckBox m_tuesday;
    QCheckBox m_wednesday;
    QCheckBox m_thursday;
    QCheckBox m_friday;
    QCheckBox m_day;
    QCheckBox m_morning;
    QCheckBox m_morningHoursCB;
    QTimeEdit m_morningHoursTE;
    QCheckBox m_afternoon;
    QCheckBox m_afternoonHoursCB;
    QTimeEdit m_afternoonHoursTE;
    QCheckBox m_other;
    QLineEdit m_otherLE;

    QWidget *m_addressSection;
    QLabel m_addressLabel;
    QLineEdit m_address;
    LinkedPicture m_addressPicture;
    QComboBox m_city;
    QPushButton cityAddButton;
    QPushButton cityWithdrawButton;
    QMap<int, QString> m_cityMap;

    QTextEdit m_pathology;
    QLabel m_charactersLeftLabel;

    QCheckBox m_emergency;

    QCheckBox m_callDateCB;
    QLineEdit m_callDateLE;

    QPushButton m_acceptButton;

    QFormLayout *m_mainLayout;
};

#endif // PATIENTFORM_H
