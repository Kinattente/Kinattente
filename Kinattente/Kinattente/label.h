#ifndef LABEL_H
#define LABEL_H

#include "usualfonctions.h"
#include "labelpushbutton.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QLabel>
#include <QTextEdit>
#include <QVariant>
#include <QDateTime>
#include <QLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

/*
 * Classe qui crée l'étiquette du patient à mettre dans les onglets de la liste d'attente
 * Propose des accesseurs pour les boutons Retirer et Modifier
 */

class Label : public QFrame
{
    Q_OBJECT

    public:
    explicit Label(QSqlQuery const &query, QString const &numero, QWidget *parent = nullptr);// Constructeur
    Label(Label const&) = delete;// Constructeur de copie
    Label& operator=(Label const&) = delete;// Opérateur de copie
    ~Label();// Destructeur
    int id() const;// Accesseur

    signals:
    void withdrawalPatient(int id, QSqlRecord const &sql_doublon = QSqlRecord());
    void editPatient(int id, QSqlRecord const &sql_doublon = QSqlRecord());

    public slots:
    void withdrawalButtonClicked();// Slot pour retirer un patient
    void editButtonClicked();// Slot pour modifier un patient
    void highlighteningMargins();// Slot pour changer la couleur du cadre en clignotement
    void originalMarginsColor();// Slot pour changer la couleur du cadre en fonction de l'urgence

    private:
    QString availabilityFilling(QString const &availabilityCode);// Méthode pour remplir la disponibilite
    QString kineInitials(QString name, QString firstname);// Méthode qui prend le nom du kiné et renvoie ses initiales en majuscules
    int m_id;
    bool m_emergency;
};

#endif // LABEL_H
