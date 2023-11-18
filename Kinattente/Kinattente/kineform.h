#ifndef KINEFORM_H
#define KINEFORM_H

#include "usualfonctions.h"
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QColorDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTimer>
#include <QSettings>
#include <QSqlError>
#include <QGraphicsDropShadowEffect>

/*
 * Classe qui ouvre un formulaire pour ajouter un kiné
 * Propose des accesseurs pur toutes les données remplies dans le formulaire
 */

class KineForm : public QDialog
{
    Q_OBJECT

    public:
    KineForm(int const id = 0, QWidget *parent = nullptr);// Constructeur
    ~KineForm();// Destructeur

    public slots:
    void nameFieldFormatting(const QString & text);// Slot pour formater correctement le champ du nom
    void firstnameFieldFormatting(const QString & text);// Slot pour formater correctement le champ du prénom
    void colorDialog();// Slot pour ouvrir la boite de dialogue couleur et changer la couleur du carré
    void refreshVisualization();// Slot pour actualiser la Visualization
    void acceptButtonClicked();// Slot pour fermer le formulaire après vérification du bon remplissage des champs

    protected:
    int id() const;// Accesseur
    QString name() const;// Accesseur
    QString firstname() const;// Accesseur
    QString color() const;// Accesseur
    bool active() const;// Accesseur

    int m_id;
    bool m_colorOk;
    QLineEdit m_name;
    QLineEdit m_firstname;
    QCheckBox m_active;
    QFrame m_visualization;
    QLabel m_nameVisualization;
    QWidget m_colorVisualization;
    QColor m_color;
    QPushButton m_colorBouton;
    QPushButton m_acceptButton;
};

#endif // KINEFORM_H
