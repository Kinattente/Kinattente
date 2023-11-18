#include "label.h"

Label::Label(const QSqlQuery &sql_PatientData, const QString &numero, QWidget *parent) : QFrame(parent)// Constructeur
{
    // Remplissage des attributs
    QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };
    m_id = sql_PatientData.value(0).toInt();
    bool const location { sql_PatientData.value(1).toBool() };
    QString const age { (sql_PatientData.value(3).toDouble() == 0 ? "" : " (" + (sql_PatientData.value(3).toDouble() > 1 ? sql_PatientData.value(3).toString() + " ans" : QString::number(sql_PatientData.value(3).toDouble() * 100)  + " mois") + ")") };
    QString const name { genderList[sql_PatientData.value(2).toInt()] + sql_PatientData.value(4).toString() + " " + sql_PatientData.value(5).toString() + age };
    QString const phone1 { sql_PatientData.value(6).toString() };
    QString const phone2 { sql_PatientData.value(7).toString() };
    QString const email { sql_PatientData.value(8).toString() };
    QString const doctor { sql_PatientData.value(9).toString() };
    QString const kine { kineInitials(sql_PatientData.value(10).toString(), sql_PatientData.value(11).toString()) };
    QString const availability { availabilityFilling(sql_PatientData.value(12).toString()) };
    QString address { sql_PatientData.value(13).toString() };
    QString city { sql_PatientData.value(14).toString() };
    QString const pathology { sql_PatientData.value(15).toString() };
    m_emergency = !sql_PatientData.value(16).toBool();
    QString const call_date { sql_PatientData.value(17).toDateTime().toString("dd/MM/yyyy") };
    QString const daysSinceCall { QString::number(sql_PatientData.value(17).toDateTime().daysTo(QDateTime::currentDateTime())) };

    // Création des QLabel
    QLabel *numberLabel { new QLabel("<center>" + numero + "</center>") };
        numberLabel->setObjectName("numberLabel");
        numberLabel->setFixedSize(40, 40);
        numberLabel->setStyleSheet((m_emergency ? "background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 118, 87, 255));" : "background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(0, 113, 255, 255), stop:1 rgba(91, 171, 252, 255));"));

    QLabel *nameLabel { new QLabel() };
        nameLabel->setObjectName("nameLabel");
        nameLabel->setText(nameLabel->fontMetrics().elidedText(name, Qt::ElideRight, nameLabel->width()));

    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
    QColor const EditoutlineColor { (m_emergency ? QColor::fromRgb(255, 0, 0) : QColor::fromRgb(0, 113, 255)) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        highlighteningMarginsEditEffect->setBlurRadius(5);
    QPushButton *editButton { new LabelPushButton(LabelPushButton::Edit, m_emergency) };
        editButton->setToolTip(tr("Modifier la fiche patient"));
        editButton->setGraphicsEffect(highlighteningMarginsEditEffect);

    QGraphicsDropShadowEffect *highlighteningMarginsWithDrawalEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsWithDrawalEffect->setOffset(0,0);
    QColor const WithDrawaloutlineColor { (m_emergency ? QColor::fromRgb(255, 0, 0) : QColor::fromRgb(0, 113, 255)) };
        highlighteningMarginsWithDrawalEffect->setColor(WithDrawaloutlineColor);
        highlighteningMarginsWithDrawalEffect->setBlurRadius(5);
    LabelPushButton *withDrawalButton { new LabelPushButton(LabelPushButton::Withdrawal, m_emergency) };
        withDrawalButton->setToolTip(tr("Retirer le patient de la liste d'attente"));
        withDrawalButton->setGraphicsEffect(highlighteningMarginsWithDrawalEffect);

    QLabel *callLabel { new QLabel(tr("<strong>Appelé le</strong> : ") + call_date + " (" + daysSinceCall + tr("jrs)")) };

    QLabel *availabilityLabel { new QLabel(tr("<strong>Disponibilité</strong> : ") + availability) };
        availabilityLabel->setWordWrap(true);

    QLabel *addressLabel { new QLabel(tr("<strong>Adresse</strong> : ") + address + " " + city) };
    addressLabel->setWordWrap(true);
    if(!location)
        addressLabel->hide();

    LinkedPicture *addressPicture { new LinkedPicture(QSize(20, 20), "Images/GPS.png", "https://www.google.fr/maps/place/" + address.replace(" ", "+") + "," + city.replace(" ", "+"), this) };

    QLabel *phone1Label { new QLabel(tr("<strong>Tél 1</strong> : ") + phone1) };

    QLabel *phone2Label { new QLabel(tr("<strong>Tél 2</strong> : ") + phone2) };

    QLabel *emailLabel { new QLabel() };
    emailLabel->setText(emailLabel->fontMetrics().elidedText(tr("<strong>Email</strong> : ") + email, Qt::ElideRight, emailLabel->width()));

    QLabel *doctorLabel { new QLabel(tr("<strong>Médecin Prescripteur</strong> : ") + doctor) };

    QLabel *kineLabel { new QLabel(tr("<strong>Kiné référent</strong> :")) };
    QLabel *kineIcon { new QLabel };
        kineIcon->setFixedSize(16,16);
        kineIcon->setStyleSheet("background-color: " + sql_PatientData.value(18).toString() + ";border-radius: 3px;");
    QLabel *kineInitials { new QLabel("<strong>" + kine + "</strong>") };

    QFrame *HLine1 { new QFrame };
        HLine1->setFrameShape(QFrame::HLine);
        HLine1->setLineWidth(1);
        HLine1->setStyleSheet((m_emergency ? "color: red;" : "color: rgb(0, 113, 255);"));

    QFrame *HLine2 { new QFrame };
        HLine2->setFrameShape(QFrame::HLine);
        HLine2->setLineWidth(1);
        HLine2->setStyleSheet((m_emergency ? "color: red;" : "color: rgb(0, 113, 255);"));

    QFrame *HLine3 { new QFrame };
        HLine3->setFrameShape(QFrame::HLine);
        HLine3->setLineWidth(1);
        HLine3->setStyleSheet((m_emergency ? "color: red;" : "color: rgb(0, 113, 255);"));

    QLabel *pathologyLabel { new QLabel(tr("<strong>Pathologie :</strong><br/>") + pathology) };
        pathologyLabel->setTextFormat(Qt::RichText);
        pathologyLabel->setWordWrap(true);

    // Création des QLayout
    QVBoxLayout *mainLayout { new QVBoxLayout };
    QHBoxLayout *upperPartLayout { new QHBoxLayout };
    QGridLayout *gridLayout { new QGridLayout };
    QHBoxLayout *kineLayout { new QHBoxLayout };

    // Mise en layout
    setLayout(mainLayout);
        mainLayout->addLayout(upperPartLayout);
            upperPartLayout->addWidget(numberLabel, 0, Qt::AlignLeft);
            upperPartLayout->addWidget(nameLabel, 10, Qt::AlignHCenter);
            upperPartLayout->addWidget(editButton, 0, Qt::AlignRight);
            upperPartLayout->addWidget(withDrawalButton, 0, Qt::AlignRight);
        mainLayout->addLayout(gridLayout);
        if(!location)
        {
            delete addressPicture;
            gridLayout->addWidget(HLine1, 0, 0, 1, 2);
            gridLayout->addWidget(callLabel, 1, 0);
            gridLayout->addWidget(phone1Label, 1, 1);
            gridLayout->addWidget(availabilityLabel, 2, 0);
            gridLayout->addWidget(phone2Label, 2, 1);
            gridLayout->addWidget(emailLabel, 3, 0, 1, 2);
            gridLayout->addWidget(HLine2, 4, 0, 1, 2);
            gridLayout->addWidget(doctorLabel, 5, 0);
            gridLayout->addLayout(kineLayout, 5, 1);
                kineLayout->addWidget(kineLabel);
                kineLayout->addWidget(kineIcon);
                kineLayout->addWidget(kineInitials);
                kineLayout->setAlignment(Qt::AlignLeft);
            gridLayout->addWidget(HLine3, 6, 0, 1, 2);
            gridLayout->addWidget(pathologyLabel, 7, 0, 1, 2);
        }
        else
        {
            gridLayout->addWidget(HLine1, 0, 0, 1, 3);
            gridLayout->addWidget(callLabel, 1, 0);
            gridLayout->addWidget(phone1Label, 1, 2);
            gridLayout->addWidget(availabilityLabel, 2, 0);
            gridLayout->addWidget(phone2Label, 2, 2);
            gridLayout->addWidget(addressLabel, 3, 0);
            gridLayout->addWidget(addressPicture, 3, 1);
            gridLayout->addWidget(emailLabel, 3, 2);
            gridLayout->addWidget(HLine2, 4, 0, 1, 3);
            gridLayout->addWidget(doctorLabel, 5, 0);
            gridLayout->addLayout(kineLayout, 5, 2);
                kineLayout->addWidget(kineLabel);
                kineLayout->addWidget(kineIcon);
                kineLayout->addWidget(kineInitials);
                kineLayout->setAlignment(Qt::AlignLeft);
            gridLayout->addWidget(HLine3, 6, 0, 1, 3);
            gridLayout->addWidget(pathologyLabel, 7, 0, 1, 3);
        }

    QGraphicsDropShadowEffect *effect { new QGraphicsDropShadowEffect };
    effect->setOffset(0,0);
    QColor const outline { (m_emergency ? QColor::fromRgb(255, 0, 0) : QColor::fromRgb(0, 113, 255)) };
    effect->setColor(outline);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);

    // Connexion des boutons Retirer et Modifier
    connect(withDrawalButton, &QPushButton::clicked, this, &Label::withdrawalButtonClicked);
    connect(editButton, &QPushButton::clicked, this, &Label::editButtonClicked);
}

Label::~Label()// Destructeur
{

}

int Label::id() const// Accesseur
{
    return m_id;
}

void Label::withdrawalButtonClicked()// Slot pour retirer un patient
{
    emit withdrawalPatient(m_id);
}

void Label::editButtonClicked()// Slot pour retirer un patient
{
    emit editPatient(m_id);
}

void Label::highlighteningMargins()// Slot pour changer la couleur du cadre en clignotement
{
    QGraphicsDropShadowEffect *effect { new QGraphicsDropShadowEffect };
    effect->setOffset(0,0);
    QColor const outline { (m_emergency ? QColor(Qt::GlobalColor::red) : QColor::fromRgb(0, 113, 255)) };
    effect->setColor(outline);
    effect->setBlurRadius(20);
    setGraphicsEffect(effect);
}

void Label::originalMarginsColor()// Slot pour changer la couleur du cadre en fonction de l'urgence
{
    QGraphicsDropShadowEffect *effect { new QGraphicsDropShadowEffect };
    effect->setOffset(0,0);
    QColor const outline { (m_emergency ? QColor(Qt::GlobalColor::red) : QColor::fromRgb(0, 113, 255)) };
    effect->setColor(outline);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
}

QString Label::availabilityFilling(const QString &availabilityCode)// Methode pour remplir la disponibilité
{
    QStringList availabilityCodeList { availabilityCode.split("#") };
    int i { 0 };
    QStringList availabilityList;
    QString availability;
    if(availabilityCodeList[i].toInt())// Autre
        return availabilityCodeList[i + 1];

    i++;
    if(availabilityCodeList[i].toInt())// Semaine
        availability.append(tr("Tous les jours, "));
    else
    {
        i++;
        if(availabilityCodeList[i].toInt())
            availabilityList.append(tr("Lundi"));
        i++;
        if(availabilityCodeList[i].toInt())
            availabilityList.append(tr("Mardi"));
        i++;
        if(availabilityCodeList[i].toInt())
            availabilityList.append(tr("Mercredi"));
        i++;
        if(availabilityCodeList[i].toInt())
            availabilityList.append(tr("Jeudi"));
        i++;
        if(availabilityCodeList[i].toInt())
            availabilityList.append(tr("Vendredi"));

        for(int j { 0 }; j < availabilityList.size(); j++)
        {
            availability.append(availabilityList[j]);
            if(j == availabilityList.size() - 2)
                availability.append(" et ");
            else
                availability.append(", ");
        }
    }

    i++;
    availabilityList.clear();
    if(availabilityCodeList[i].toInt())// Journée
        availability.append(tr("Toute la journée"));
    else
    {
        i++;
        if(availabilityCodeList[i].toInt())// Matin
        {
            availabilityList.append(tr("Le matin"));
            i++;
            if(availabilityCodeList[i].toInt())// Matin heures
            {
                i++;
                availabilityList.append(tr(" à partir de ") + availabilityCodeList[i]);
            }
            else
                availabilityList.append("");
        }
        else
        {
            availabilityList.append("");
            availabilityList.append("");
        }

        i++;
        if(availabilityCodeList[i].toInt())// Après-Midi
        {
            availabilityList.append(tr("L'après-midi"));
            i++;
            if(availabilityCodeList[i].toInt())// Après-Midi heures
            {
                i++;
                availabilityList.append(tr(" à partir de ") + availabilityCodeList[i]);
            }
            else
                availabilityList.append("");
        }
        {
            availabilityList.append("");
            availabilityList.append("");
        }

        availability.append(availabilityList[0] + availabilityList[1]);
        if(!availabilityList[0].isEmpty() && !availabilityList[2].isEmpty())
            availability.append(tr(" et "));
        availability.append(availabilityList[2] + availabilityList[3]);
    }
    return availability;
}

QString Label::kineInitials(QString name, QString firstname)// Méthode qui prend le nom et le prénom du kiné et renvoie ses initiales en majuscules
{
    if(name != "AUCUN")
    {
        QStringList namesList { name.split(' ') };
        QStringList firstnamesList { firstname.split(' ') };
        namesList.replaceInStrings(QRegExp("^(.).*"), "\\1");
        firstnamesList.replaceInStrings(QRegExp("^(.).*"), "\\1");
        return namesList.join("") + firstnamesList.join("");
    }
    return name;
}
