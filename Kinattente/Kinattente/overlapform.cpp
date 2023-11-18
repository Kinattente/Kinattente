#include "overlapform.h"

OverlapForm::OverlapForm(QHash<QString, QVariant> patientData, QSqlQuery query, QWidget *parent) : QDialog(parent)// Constructeur
{
    QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };
    QGridLayout *mainLayout { new QGridLayout };
    QHBoxLayout *ButtonsLayout { new QHBoxLayout };
    QFrame *HLine1 { new QFrame };
        HLine1->setFrameShape(QFrame::HLine);
        HLine1->setLineWidth(2);
        HLine1->setStyleSheet("color: rgb(56, 93, 138);");
    QFrame *HLine2 { new QFrame };
        HLine2->setFrameShape(QFrame::HLine);
        HLine2->setLineWidth(2);
        HLine2->setStyleSheet("color: rgb(56, 93, 138);");
    QFrame *VLine { new QFrame };
        VLine->setFrameShape(QFrame::VLine);
        VLine->setLineWidth(1);
        VLine->setStyleSheet("color: rgb(56, 93, 138);");
        m_yesButton.setText(tr("Oui"));
        m_yesButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_noButton.setText(tr("Non"));
        m_noButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_cancelButton.setText(tr("Annuler"));
        m_cancelButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setLayout(mainLayout);
        mainLayout->addWidget(new QLabel(tr("<center>Un ou plusieurs patients ayant le même nom ou le même numéro de téléphone sont déjà en liste d'attente :</center>"), this), 0, 0, 1, -1);
        mainLayout->addWidget(HLine1, 1, 0, 1, -1);

        mainLayout->addWidget(new QLabel(tr("Lieu"), this), 2, 0);
        mainLayout->addWidget(new QLabel(tr("Nom"), this), 4, 0);
        mainLayout->addWidget(new QLabel(tr("Prénom"), this), 6, 0);
        mainLayout->addWidget(new QLabel(tr("Age"), this), 8, 0);
        mainLayout->addWidget(new QLabel(tr("Téléphone 1"), this), 10, 0);
        mainLayout->addWidget(new QLabel(tr("Téléphone 2"), this), 12, 0);
        mainLayout->addWidget(new QLabel(tr("Email"), this), 14, 0);
        mainLayout->addWidget(new QLabel(tr("Médecin"), this), 16, 0);
        mainLayout->addWidget(new QLabel(tr("Kiné référent"), this), 18, 0);
        mainLayout->addWidget(new QLabel(tr("Rue"), this), 20, 0);
        mainLayout->addWidget(new QLabel(tr("Ville"), this), 22, 0);
        mainLayout->addWidget(new QLabel(tr("Pathologie"), this), 24, 0);
        mainLayout->addWidget(new QLabel(tr("Urgence"), this), 26, 0);
        mainLayout->addWidget(new QLabel(tr("Date d'appel"), this), 28, 0);

        mainLayout->addWidget(VLine, 2, 1, 27, 1);
        mainLayout->addWidget(new QLabel((patientData.value("location").toBool() ? tr("<strong>Domicile</strong>") : tr("<strong>Cabinet</strong>")), this), 2, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("gender").toString() + " " + patientData.value("p_name").toString() + "</strong>", this), 4, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("p_firstname").toString() + "</strong>", this), 6, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("age").toString() + "</strong>", this), 8, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("phone1").toString() + "</strong>", this), 10, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("phone2").toString() + "</strong>", this), 12, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("email").toString() + "</strong>", this), 14, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("doctor").toString() + "</strong>", this), 16, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("kine_name_firstname").toString() + "</strong>", this), 18, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("address").toString() + "</strong>", this), 20, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("c_name").toString() + "</strong>", this), 22, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("pathology").toString() + "</strong>", this), 24, 2);
        mainLayout->addWidget(new QLabel((patientData.value("no_emergency").toBool() ? tr("<strong>Urgent</strong>") : tr("<strong>Non urgent</strong>")), this), 26, 2);
        mainLayout->addWidget(new QLabel("<strong>" + patientData.value("call_date").toDateTime().toString("dd/MM/yyyy") + "</strong>", this), 28, 2);

        int l { 3 };
        while(query.next())
        {
            QFrame *VLine { new QFrame };
                VLine->setFrameShape(QFrame::VLine);
                VLine->setLineWidth(1);
                VLine->setStyleSheet("color: rgb(56, 93, 138);");
            mainLayout->addWidget(VLine, 2, l, 27, 1);
            l++;
            mainLayout->addWidget(new QLabel((query.value("location").toBool() ? tr("Domicile") : tr("Cabinet")), this), 2, l);
            mainLayout->addWidget(new QLabel(genderList[query.value("gender").toInt()] + " " + query.value("p_name").toString(), this), 4, l);
            mainLayout->addWidget(new QLabel(query.value("p_firstname").toString(), this), 6, l);
            mainLayout->addWidget(new QLabel((query.value("age").toDouble() == 0 || query.value("age").toDouble() > 1 ? query.value("age").toString() + tr(" ans") : QString::number(query.value("age").toDouble() * 100) + tr(" mois")), this), 8, l);
            mainLayout->addWidget(new QLabel(query.value("phone1").toString(), this), 10, l);
            mainLayout->addWidget(new QLabel(query.value("phone2").toString(), this), 12, l);
            mainLayout->addWidget(new QLabel(query.value("email").toString(), this), 14, l);
            mainLayout->addWidget(new QLabel(query.value("doctor").toString(), this), 16, l);
            mainLayout->addWidget(new QLabel(query.value("k_name").toString() + " " + query.value("k_firstname").toString(), this), 18, l);
            mainLayout->addWidget(new QLabel(query.value("address").toString(), this), 20, l);
            mainLayout->addWidget(new QLabel(query.value("c_name").toString(), this), 22, l);
            mainLayout->addWidget(new QLabel(query.value("pathology").toString(), this), 24, l);
            mainLayout->addWidget(new QLabel((!query.value("no_emergency").toBool() ? tr("Urgent") : tr("Non urgent")), this), 26, l);
            mainLayout->addWidget(new QLabel(query.value("call_date").toDateTime().toString("dd/MM/yyyy"), this), 28, l);
            l++;
        }

        for(int i { 3 }; i < 29; i+=2)
        {
            QFrame *HLine { new QFrame };
                HLine->setFrameShape(QFrame::HLine);
                HLine->setLineWidth(1);
                HLine->setStyleSheet("color: rgb(56, 93, 138);");
            mainLayout->addWidget(HLine, i, 0, 1, -1);
        }

            mainLayout->addWidget(HLine2, 29, 0, 1, -1);
            mainLayout->addWidget(new QLabel(tr("<center>Votre <strong>Patient</strong> correspond-il à l'un de ce(s) patient(s) ?</center>"), this), 30, 0, 1, -1);
            mainLayout->addLayout(ButtonsLayout, 31, 0, 1, -1);
                ButtonsLayout->addWidget(&m_yesButton);
                ButtonsLayout->addWidget(&m_noButton);
                ButtonsLayout->addWidget(&m_cancelButton);

    QObject::connect(&m_yesButton, &QPushButton::clicked, this, &OverlapForm::yesButtonClicked);
    QObject::connect(&m_noButton, &QPushButton::clicked, this, &OverlapForm::noButtonClicked);
    QObject::connect(&m_cancelButton, &QPushButton::clicked, this, &OverlapForm::cancelButtonClicked);
}

OverlapForm::~OverlapForm()// Destructeur
{

}

QString OverlapForm::state() const// Accesseur
{
    return m_state;
}

void OverlapForm::yesButtonClicked()// Slot si le patient existe bien déjà dans la BDD
{
    m_state = "Overlap";
    QMessageBox::information(this, tr("IT'S A TRAP"), tr("Le patient vous a bien eu ! Votre ajout est annulé, aller directement modifier la fiche du patient dans la liste d'attente si vous voulez apporter de nouvelles informations !"));
    close();
}

void OverlapForm::noButtonClicked()// Slot si le patient n'existe pas dans la BDD
{
    m_state = "New";
    close();
}

void OverlapForm::cancelButtonClicked()// Slot pour annuler
{
    m_state = "Cancel";
    close();
}

