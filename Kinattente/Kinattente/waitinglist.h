#ifndef WAITINGLIST_H
#define WAITINGLIST_H

#include "label.h"
#include <QTabWidget>
#include <QLineEdit>
#include <QScrollArea>
#include <QMessageBox>
#include <QTimer>
#include <QSqlError>
#include <QSettings>

/*
 * Classe qui crée les onglets des listes d'attente
 * Propose des accesseurs pour les étiquettes et les listes déroulantes
 */

class WaitingList : public QTabWidget
{
    Q_OBJECT

    public:
    explicit WaitingList(QString const &location, QStringList const &researchField = QStringList(), QWidget *parent = nullptr);// Constructeur
    WaitingList(WaitingList const&) = delete;// Constructeur de copie
    WaitingList& operator=(WaitingList const&) = delete;// Opérateur de copie
    ~WaitingList();// Destructeur
    void scrollListToSelectedPatient(int tabIndex, int patientIndexPosition);// Méthode qui prend l'id d'un patient et l'index d'un onglet dans lequel faire défiler le scroll pour rendre le patient choisi visible
    void withdrawalLabel(int id);// Méthode qui prend l'id d'un patient pour le retirer des listes de Labels, m_waitingTime et m_listNumber
    QList<QScrollArea*> &scrollList();// Accesseur nécessaire pour faire défiler le scroll et afficher le patient voulu
    QStringList researchFields();// Accesseur
    QList<QList<Label*>*> labels() const;// Accesseur
    QStringList waitingTimesInformations(int tabIndex) const;// Accesseur
    int patientIndex(int id, int tabIndex);// Accesseur

    signals:
    void refresh();

    private slots:
    void researchFieldTimer(int const &tabIndex, QString researchField = "");// Slot pour lancer la minuterie laissant le temps d'écrire avant de faire l'actualisation de la recherche dans la liste d'attente
    void labelsFilling(int const &tabIndex, QString researchField = "");// Slot pour remplir (ou actualiser lors d'une recherche) un onglet de LA avec les patients correspondants

    protected:
    void tabsFilling();// Méthode pour remplir les onglets avec un QScrollArea, un QLineEdit pré-rempli si besoin et un QList<Label*>. Connexion du QLineEdit
    QStringList researchFieldsTextFilling();// Méthode qui rempli un QStringList avec les champs de recherche ou les initialise
    QList<QList< int>*> idList();// Méthode qui prend un id et renvoie l'arborescence des id
    QTimer *m_researchFieldTimer;// Membre contenant un QTimer retenant l'actualisation de la liste d'attente  pendant l'écriture de la recherche
    QStringList m_researchFieldText;// Membre contenant le texte du champ de recherche
    QStringList m_sql;// Membre contenant les requêtes MySQL
    QList<QScrollArea*> m_scrollList;// Membre contenant les QScrollArea de chaque onglet de la liste d'attente
        QList<QLineEdit*> m_researchFields;// Membre contenant les QLineEdit de chaque onglet de la liste d'attente
        QList<QList<Label*>*> m_labels;// Membre contenant l'intégralité des étiquettes
    QList<QList<QMultiMap<int, int>*>*> m_waitingTime;// Membre contenant la liste des onglets, contenant elle-même 2 listes des dates d'appel urgentes et non urgentes associées à leur id
};

#endif // WAITINGLIST_H
