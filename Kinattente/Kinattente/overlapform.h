#ifndef OVERLAPFORM_H
#define OVERLAPFORM_H

#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDateTime>

/*
 * Classe qui ouvre un QDialog pour proposer une correspondance avec un doublon
 * Propose des accesseurs pour les boutons Oui et Non et la rÃƒÂ©ponse si le patient existe dÃƒÂ©jÃƒ 
 */

class OverlapForm : public QDialog
{
    Q_OBJECT

    public:
    OverlapForm(QHash<QString, QVariant> patientData, QSqlQuery query, QWidget *parent = nullptr);// Constructeur
    OverlapForm(OverlapForm const&) = delete;// Constructeur de copie
    OverlapForm& operator=(OverlapForm const&) = delete;// OpÃƒÂ©rateur de copie
    ~OverlapForm();// Destructeur
    QString state() const;// Accesseur

    public slots:
    void yesButtonClicked();// Slot si le patient existe bien déjà dans la BDD
    void noButtonClicked();// Slot si le patient n'existe pas dans la BDD
    void cancelButtonClicked();// Slot pour annuler

    protected:
    QString m_state;
    QPushButton m_yesButton;
    QPushButton m_noButton;
    QPushButton m_cancelButton;
};

#endif // OVERLAPFORM_H
