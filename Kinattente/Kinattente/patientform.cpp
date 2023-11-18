#include "patientform.h"

PatientForm::PatientForm(const int &id, QWidget *parent) : QDialog(parent), m_id{id}, m_addressPicture {QSize(20, 20), "Images/GPS.png", "https://www.google.fr/maps/place/", this }// Constructeur
{
    // Récupération dans la BDD des informations du patient avec l'id
    QSettings const settings;
    QSqlQuery sql_patientData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(id != 0)
    {
        sql_patientData.prepare("SELECT p.id, p.location, p.gender, p.age, p.name AS p_name, p.firstname AS p_firstname, p.phone1, p.phone2, p.email, p.doctor, k.name AS k_name, k.firstname AS k_firstname, p.availability, p.address, c.name AS c_name, p.pathology, p.no_emergency, p.call_date FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id WHERE p.id = :id");
        sql_patientData.bindValue(":id", id);
        if(!sql_patientData.exec())
        {
            error(400, this, sql_patientData.lastError().text());
            QTimer::singleShot(200, this, &PatientForm::close);
            return;
        }
        sql_patientData.first();
        m_originLocation = sql_patientData.value("location").toBool();
        m_originCallDate = sql_patientData.value("call_date").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

    // Lieu de la séance avec boutons Cabinet / Domicile
    QLabel *locationLabel { new QLabel(tr("<center>Lieu de la séance</center>")) };
        locationLabel->setFont(QFont("MS Shell Dlg 2", 12));
    m_officeButton.setText("Cabinet");
        m_officeButton.setCursor(Qt::PointingHandCursor);
        m_officeButton.setCheckable(true);
    m_homeButton.setText("Domicile");
        m_homeButton.setCursor(Qt::PointingHandCursor);
        m_homeButton.setCheckable(true);
    (id == 0 || !sql_patientData.value("location").toBool() ? m_officeButton : m_homeButton).setChecked(true);

    // Coordonnées du patient
    QLabel *contactInformationsLabel { new QLabel(tr("<center>Coordonnées du patient</center>")) };
        contactInformationsLabel->setFont(QFont("MS Shell Dlg 2", 12));
    m_gender << new QRadioButton(tr("N/C")) << new QRadioButton(tr("Mr")) << new QRadioButton(tr("Mme")) << new QRadioButton(tr("Enft"));
        m_gender[(id != 0 ? sql_patientData.value("gender").toInt() : 0 )]->setChecked(true);
    QLabel *ageLabel { new QLabel(tr("Age :")) };
    m_age.setValue(id == 0 ? 0 : (sql_patientData.value("age").toDouble() == 0 || sql_patientData.value("age").toDouble() > 1 ? sql_patientData.value("age").toInt() : sql_patientData.value("age").toDouble() * 100));
    m_age.setMinimum(0);
    m_yearmonth.setText(id == 0 ? tr("ans") : (sql_patientData.value("age").toDouble() == 0 || sql_patientData.value("age").toDouble() > 1 ? tr("ans") : tr("mois")));
    m_yearmonth.setFixedWidth(65);
    m_name.setText(id != 0 ? sql_patientData.value("p_name").toString() : "" );
        m_name.setMaxLength(20);
    m_firstname.setText(id != 0 ? sql_patientData.value("p_firstname").toString() : "" );
        m_firstname.setMaxLength(20);
    m_phone1.setInputMask("99.99.99.99.99");
    m_phone1.setText(id != 0 ? sql_patientData.value("phone1").toString() : "" );
    m_phone2.setInputMask("99.99.99.99.99");
    m_phone2.setText(id != 0 ? sql_patientData.value("phone2").toString() : "" );
    m_email.setText(id != 0 ? sql_patientData.value("email").toString() : "" );
    m_email.setMaxLength(30);
    m_doctor.setText(id != 0 ? sql_patientData.value("doctor").toString() : "" );
        m_doctor.setMaxLength(20);
    kineListFilling((id != 0 ? sql_patientData.value("k_name").toString() : "" ), (id != 0 ? sql_patientData.value("k_firstname").toString() : "" ));

    // Disponibilité du patient
    QLabel *availability { new QLabel(tr("<center>Disponibilité du patient</center>")) };
        availability->setFont(QFont("MS Shell Dlg 2", 12));
    m_week.setText(tr("Tous les jours"));
    QFrame *VLine1 { new QFrame };
        VLine1->setFrameShape(QFrame::VLine);
        VLine1->setFrameShadow(QFrame::Sunken);
        VLine1->setLineWidth(0);
        VLine1->setMidLineWidth(1);
    m_monday.setText(tr("Lundi"));
    m_tuesday.setText(tr("Mardi"));
    m_wednesday.setText(tr("Mercredi"));
    m_thursday.setText(tr("Jeudi"));
    m_friday.setText(tr("Vendredi"));
    m_day.setText(tr("Toute la journée"));
    QFrame *VLine2 { new QFrame };
        VLine2->setFrameShape(QFrame::VLine);
        VLine2->setFrameShadow(QFrame::Sunken);
        VLine2->setLineWidth(0);
        VLine2->setMidLineWidth(1);
        m_morning.setText(tr("Le matin"));
        m_morningHoursCB.setText(tr("à partir de :"));
        m_morningHoursCB.setEnabled(false);
        m_morningHoursTE.setMinimumTime(QTime(7, 30));
        m_morningHoursTE.setMaximumTime(QTime(12, 0));
        m_morningHoursTE.setWrapping(true);
        m_morningHoursTE.setAccelerated(true);
        m_morningHoursTE.setEnabled(false);
    QFrame *VLine3 { new QFrame };
        VLine3->setFrameShape(QFrame::VLine);
        VLine3->setFrameShadow(QFrame::Sunken);
        VLine3->setLineWidth(0);
        VLine3->setMidLineWidth(1);
        m_afternoon.setText(tr("L'après-midi"));
        m_afternoonHoursCB.setText(tr("à partir de :"));
        m_afternoonHoursCB.setEnabled(false);
        m_afternoonHoursTE.setMinimumTime(QTime(12, 0));
        m_afternoonHoursTE.setMaximumTime(QTime(20, 0));
        m_afternoonHoursTE.setWrapping(true);
        m_afternoonHoursTE.setAccelerated(true);
        m_afternoonHoursTE.setEnabled(false);
    m_other.setText(tr("Autre :"));
    m_otherLE.setEnabled(false);
        m_otherLE.setMaxLength(80);
        if(id != 0)
            availabilityFilling(sql_patientData.value("availability").toString());

    // Adresse du patient
    m_addressLabel.setText(tr("<center>Adresse du patient</center>"));
        m_addressLabel.setFont(QFont("MS Shell Dlg 2", 12));
    m_address.setText(id != 0 ? sql_patientData.value("address").toString() : "" );
        m_address.setMaxLength(40);
    cityListFilling((id != 0 ? sql_patientData.value("c_name").toString() : 0 ));
    addressChanged(QString());
    cityAddButton.setText("+");
        cityAddButton.setFixedWidth(21);
        cityAddButton.setToolTip(tr("Ajouter une nouvelle ville"));
        cityAddButton.setCursor(Qt::PointingHandCursor);
    cityWithdrawButton.setText("-");
        cityWithdrawButton.setFixedWidth(21);
        cityWithdrawButton.setToolTip(tr("Retirer une ville"));
        cityWithdrawButton.setCursor(Qt::PointingHandCursor);

    // Pathologie du patient
    QLabel *pathology { new QLabel(tr("<center>Pathologie du patient</center>")) };
        pathology->setFont(QFont("MS Shell Dlg 2", 12));
    m_pathology.setText(id != 0 ? sql_patientData.value("pathology").toString() : "" );
    m_charactersLeftLabel.setText(tr("Il reste 255 caractère(s)"));
    m_emergency.setText(tr("Patient Urgent"));
        m_emergency.setChecked(id == 0 || sql_patientData.value("no_emergency").toBool() ? false : true);

    // Modification du moment de l'appel
    m_callDateCB.setText("Antidater l'appel :");
    m_callDateLE.setInputMask("9999-99-99;x");
    m_callDateLE.setText(id != 0 ? sql_patientData.value("call_date").toDateTime().toString("yyyy-MM-dd") : QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        m_callDateLE.setEnabled(false);
        m_callDateLE.setMaxLength(19);

    // Boutons Enregistrer et Annuler
        m_acceptButton.setText(tr("Enregistrer"));
        m_acceptButton.setDefault(true);
        m_acceptButton.setToolTip(tr("Enregistrer un nouveau patient"));
        m_acceptButton.setCursor(Qt::PointingHandCursor);
        if(m_name.text().isEmpty() && phone1().isEmpty() && phone2().isEmpty())
            m_acceptButton.setEnabled(false);
    QPushButton *cancelButton { new QPushButton(tr("Annuler")) };
        cancelButton->setToolTip(tr("Annuler"));
        cancelButton->setCursor(Qt::PointingHandCursor);

    // Mise en forme du formulaire
        m_mainLayout = new QFormLayout;
        QHBoxLayout *locationLayout { new QHBoxLayout };
        QHBoxLayout *genderLayout { new QHBoxLayout };
        QVBoxLayout *availabilityVLayout { new QVBoxLayout };
        QHBoxLayout *availabilityHLayout1 { new QHBoxLayout };
        QHBoxLayout *availabilityHLayout2 { new QHBoxLayout };
        QHBoxLayout *buttonsLayout { new QHBoxLayout };

        setLayout(m_mainLayout);
            m_mainLayout->addRow(locationLabel);
            m_mainLayout->addRow(locationLayout);
                locationLayout->addWidget(&m_officeButton);
                locationLayout->addWidget(&m_homeButton);
            m_mainLayout->addRow(contactInformationsLabel);
            m_mainLayout->addRow(genderLayout);
                    genderLayout->addWidget(m_gender[0], 10);
                    genderLayout->addWidget(m_gender[1], 10);
                    genderLayout->addWidget(m_gender[2], 10);
                    genderLayout->addWidget(m_gender[3], 10);
                    genderLayout->addWidget(ageLabel);
                    genderLayout->addWidget(&m_age);
                    genderLayout->addWidget(&m_yearmonth);
            m_mainLayout->addRow(tr("&Nom :"), &m_name);
                m_name.setFocus();
            m_mainLayout->addRow(tr("&Prénom :"), &m_firstname);
            m_mainLayout->addRow(tr("Téléphone &1 :"), &m_phone1);
            m_mainLayout->addRow(tr("Téléphone &2 :"), &m_phone2);
            m_mainLayout->addRow(tr("&Email :"), &m_email);
            m_mainLayout->addRow(tr("&Médecin prescripteur :"), &m_doctor);
            m_mainLayout->addRow(tr("&Kiné réferent :"), &m_kine);
            m_mainLayout->addRow(availability);
            m_mainLayout->addRow(availabilityVLayout);
                availabilityVLayout->addLayout(availabilityHLayout1);
                    availabilityHLayout1->addWidget(&m_week);
                    availabilityHLayout1->addWidget(VLine1);
                    availabilityHLayout1->addWidget(&m_monday);
                    availabilityHLayout1->addWidget(&m_tuesday);
                    availabilityHLayout1->addWidget(&m_wednesday);
                    availabilityHLayout1->addWidget(&m_thursday);
                    availabilityHLayout1->addWidget(&m_friday);
                availabilityVLayout->addLayout(availabilityHLayout2);
                    availabilityHLayout2->addWidget(&m_day);
                    availabilityHLayout2->addWidget(VLine2);
                    availabilityHLayout2->addWidget(&m_morning);
                    availabilityHLayout2->addWidget(&m_morningHoursCB, Qt::AlignRight);
                    availabilityHLayout2->addWidget(&m_morningHoursTE, Qt::AlignLeft);
                    availabilityHLayout2->addWidget(VLine3);
                    availabilityHLayout2->addWidget(&m_afternoon);
                    availabilityHLayout2->addWidget(&m_afternoonHoursCB, Qt::AlignRight);
                    availabilityHLayout2->addWidget(&m_afternoonHoursTE, Qt::AlignLeft);
            m_mainLayout->addRow(&m_other, &m_otherLE);
            if(id != 0 && sql_patientData.value(1).toBool())
            {
            QHBoxLayout *city1Layout { new QHBoxLayout };
            QHBoxLayout *city2Layout { new QHBoxLayout };
            m_mainLayout->addRow(&m_addressLabel);
            m_mainLayout->addRow(tr("Rue :"), city1Layout);
                city1Layout->addWidget(&m_address);
                city1Layout->addWidget(&m_addressPicture);
            m_mainLayout->addRow(tr("Ville :"), city2Layout);
                city2Layout->addWidget(&m_city);
                city2Layout->addWidget(&cityAddButton);
                city2Layout->addWidget(&cityWithdrawButton);
            }
            else
                m_addressPicture.hide();
            m_mainLayout->addRow(pathology);
            m_mainLayout->addRow(tr("Patholo&gie :"), &m_pathology);
            m_mainLayout->addRow("", &m_charactersLeftLabel);
            m_mainLayout->addRow(&m_emergency);
            m_mainLayout->addRow(&m_callDateCB, &m_callDateLE);
            m_mainLayout->addRow(buttonsLayout);
                buttonsLayout->addWidget(&m_acceptButton);
                buttonsLayout->addWidget(cancelButton);
            m_mainLayout->setLabelAlignment(Qt::AlignRight);

    // Connexions des boutons Cabinet / Domicile
    connect(&m_officeButton, &QPushButton::clicked, this, &PatientForm::alternationLocationButtons);
    connect(&m_homeButton, &QPushButton::clicked, this, &PatientForm::alternationLocationButtons);

    // Connexions du bouton "ans / mois"
    connect(&m_yearmonth, &QPushButton::clicked, this, &PatientForm::alternationYearMonthButtons);

    // Connexion des checkbox de la disponibilité
    connect(&m_week, &QCheckBox::stateChanged, this, &PatientForm::enableWeek);
    connect(&m_day, &QCheckBox::stateChanged, this, &PatientForm::enableDay);
    connect(&m_morning, &QCheckBox::stateChanged, this, &PatientForm::enableHalfDay);
    connect(&m_afternoon, &QCheckBox::stateChanged, this, &PatientForm::enableHalfDay);
    connect(&m_morningHoursCB, &QCheckBox::stateChanged, this, &PatientForm::enableHours);
    connect(&m_afternoonHoursCB, &QCheckBox::stateChanged, this, &PatientForm::enableHours);
    connect(&m_other, &QCheckBox::stateChanged, this, &PatientForm::enableOther);

    // Connexion pour actualiser le lien Google Maps
    connect(&m_address, &QLineEdit::textEdited, this, &PatientForm::addressChanged);
    connect(&m_city, &QComboBox::currentTextChanged, this, &PatientForm::addressChanged);

    // Connexion des boutons + / -
    connect(&cityAddButton, &QPushButton::clicked, this, &PatientForm::cityAddButtonClicked);
    connect(&cityWithdrawButton, &QPushButton::clicked, this, &PatientForm::cityWithdrawButtonClicked);

    // Connexion du checkbox Antidatage
    connect(&m_callDateCB, &QCheckBox::clicked, &m_callDateLE, &QLineEdit::setEnabled);

    // Connexion des boutons Enregistrer et Annuler
    connect(&m_acceptButton, &QPushButton::clicked, this, &PatientForm::acceptButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &PatientForm::close);

    // Connexion des champs pour édition
    connect(&m_name, &QLineEdit::textEdited, this, &PatientForm::nameFieldFormatting);
    connect(&m_firstname, &QLineEdit::textEdited, this, &PatientForm::firstnameFieldFormatting);
    connect(&m_phone1, &QLineEdit::textEdited, this, &PatientForm::phone1FieldLightening);
    connect(&m_phone2, &QLineEdit::textEdited, this, &PatientForm::phone2FieldLightening);
    connect(&m_email, &QLineEdit::textEdited, this, &PatientForm::emailFieldLightening);
    connect(&m_doctor, &QLineEdit::textEdited, this, &PatientForm::doctorFieldFormatting);
    connect(&m_pathology, &QTextEdit::textChanged, this, &PatientForm::pathologyFieldFormatting);
    connect(&m_callDateLE, &QLineEdit::textEdited, this, &PatientForm::calldateFieldLightening);
}

PatientForm::~PatientForm()// Destructeur
{

}

QMap<QString, QString> PatientForm::accessor() const// Accesseur
{
    return m_accessor;
}

QHash<QString, QVariant> PatientForm::patientData()// Accesseur
{
    QHash<QString, QVariant> patientDataHash;
    if(m_id == 0)
    {
        QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };
        patientDataHash.insert("location", m_homeButton.isChecked());
        patientDataHash.insert("gender", genderList[gender()]);
        patientDataHash.insert("age", m_age.text() + " " + m_yearmonth.text());
        patientDataHash.insert("p_name", m_name.text());
        patientDataHash.insert("p_firstname", m_firstname.text());
        patientDataHash.insert("phone1", (m_phone1.text() == "...." ? "" : m_phone1.text()));
        patientDataHash.insert("phone2", (m_phone2.text() == "...." ? "" : m_phone2.text()));
        patientDataHash.insert("email", m_email.text());
        patientDataHash.insert("doctor", m_doctor.text());
        patientDataHash.insert("kine_name_firstname", m_kine.currentText());
        patientDataHash.insert("address", m_address.text());
        patientDataHash.insert("c_name", m_city.currentText());
        patientDataHash.insert("pathology", m_pathology.toPlainText().replace(QRegExp("\n"), "<br/>"));
        patientDataHash.insert("no_emergency", m_emergency.isChecked());
        patientDataHash.insert("call_date", callDate());
    }
    return patientDataHash;
}

int PatientForm::id() const// Accesseur
{
    return m_id;
}

bool PatientForm::location() const// Accesseur
{
    return m_homeButton.isChecked();
}

int PatientForm::gender() const// Accesseur
{
    for(int i { 0 }; i < 4; i++)
    {
        if(m_gender[i]->isChecked())
            return i;
    }
    return 0;
}

double PatientForm::age() const// Accesseur
{
    if(m_yearmonth.text() == "ans")
        return m_age.value();
    else
        return double(m_age.value()) / 100;
}

QString PatientForm::name() const// Accesseur
{
    return m_name.text().simplified();
}

QString PatientForm::firstname() const// Accesseur
{
    return m_firstname.text().simplified();
}

QString PatientForm::phone1() const// Accesseur
{
    if(m_phone1.displayText() == "  .  .  .  .  ")
        return "";
    else
        return m_phone1.text();
}

QString PatientForm::phone2() const// Accesseur
{
    if(m_phone2.displayText() == "  .  .  .  .  ")
        return "";
    else
        return m_phone2.text();
}

QString PatientForm::email() const// Accesseur
{
    return m_email.text();
}

QString PatientForm::doctor() const// Accesseur
{
    return m_doctor.text().simplified();
}

int PatientForm::kine() const// Accesseur
{
    return m_kineMap.key(m_kine.currentText());
}

QString PatientForm::availability() const// Accesseur
{
    if(m_other.isChecked())
        return "1#" + m_otherLE.text().simplified();

    QString availability { "0" };
    if(m_week.isChecked())
        availability += "#1";
    else
    {
        availability += "#0";
        availability += "#" + QString::number(m_monday.isChecked());
        availability += "#" + QString::number(m_tuesday.isChecked());
        availability += "#" + QString::number(m_wednesday.isChecked());
        availability += "#" + QString::number(m_thursday.isChecked());
        availability += "#" + QString::number(m_friday.isChecked());
    }
    if(m_day.isChecked())
        availability += "#1";
    else
    {
        availability += "#0";
        if(!m_morning.isChecked())
            availability += "#0";
        else
        {
            availability += "#1";
            if(!m_morningHoursCB.isChecked())
                availability += "#0";
            else
                availability += "#1#" + m_morningHoursTE.time().toString("HH:mm");
        }

        if(!m_afternoon.isChecked())
            availability += "#0";
        else
        {
            availability += "#1";
            if(!m_afternoonHoursCB.isChecked())
                availability += "#0";
            else
                availability += "#1#" + m_afternoonHoursTE.time().toString("HH:mm");
        }
    }
    return availability;
}

QString PatientForm::address() const// Accesseur
{
    return m_address.text().simplified();
}

int PatientForm::city() const// Accesseur
{
    return m_cityMap.key(m_city.currentText());
}

QString PatientForm::pathology() const// Accesseur
{
    return m_pathology.toPlainText().replace(QRegExp("\n"), "<br/>").simplified();
}

bool PatientForm::no_emergency() const// Accesseur
{
    return !m_emergency.isChecked();
}

QString PatientForm::callDate() const// Accesseur
{// il ne faut pas oublier que la case peut ne pas être cochée mais le texte tout de même avoir été modifiée, donc on ne peut pas s'y fier
    if(m_callDateCB.isChecked())
    {
        if(m_originCallDate.contains(m_callDateLE.displayText()))
            return m_originCallDate;
        else
            return m_callDateLE.displayText() + " " + QDateTime::currentDateTime().toString("hh:mm:ss");
    }
    if(m_id == 0)
        return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return m_originCallDate;
}

void PatientForm::kineListFilling(const QString &name, const QString &firstname)// Methode pour remplir la QComboBox avec les infos kinés
{
    QSettings const settings;
    QSqlQuery sql_KinesData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(!sql_KinesData.exec("SELECT * FROM Kines ORDER BY active DESC, id ASC"))
    {
        error(401, this, sql_KinesData.lastError().text());
        QTimer::singleShot(200, this, &PatientForm::close);
        return;
    }

    while (sql_KinesData.next())
    {
        QString const kine_name_firstname { sql_KinesData.value(1).toString() + " " + sql_KinesData.value(2).toString() };
        QString const color { sql_KinesData.value(3).toString() };
        QWidget *colorIcon { new QWidget(this) };
            colorIcon->setFixedSize(14,13);
            colorIcon->hide();
            colorIcon->setStyleSheet("background-color: " + color + ";border-radius: 3px;");
        m_kine.addItem(colorIcon->grab(), kine_name_firstname);
        m_kineMap.insert(sql_KinesData.value(0).toInt(), kine_name_firstname);
        if((name + " " + firstname) == sql_KinesData.value(1).toString() + " " + sql_KinesData.value(2).toString())
            m_kine.setCurrentIndex(sql_KinesData.at());
    }
}

void PatientForm::cityListFilling(const QString &name)// Méthode pour remplir la QComboBox avec les villes
{
    m_city.clear();
    QSettings const settings;
    QSqlQuery sql_CityList(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(!sql_CityList.exec("SELECT * FROM Cities ORDER BY name"))
    {
        error(402, this, sql_CityList.lastError().text());
        QTimer::singleShot(200, this, &PatientForm::close);
        return;
    }

    while (sql_CityList.next())
    {
        m_city.addItem(sql_CityList.value(1).toString());
        m_cityMap.insert(sql_CityList.value(0).toInt(), sql_CityList.value(1).toString());
        if(name == sql_CityList.value(1).toString())
            m_city.setCurrentIndex(sql_CityList.at());
    }
}

void PatientForm::availabilityFilling(const QString &availability)// Methode pour remplir la disponibilité
{
    QStringList const availabilityList { availability.split("#") };
    int i { 0 };
    if(availabilityList[i].toInt())// Autre
    {
        enableOther(2);
        m_other.setChecked(true);
        m_otherLE.setText(availabilityList[i + 1]);
        return;
    }

    i++;
    if(availabilityList[i].toInt())// Semaine
    {
        enableWeek(2);
        m_week.setChecked(true);
    }
    else
    {
        i++;
        if(availabilityList[i].toInt())
            m_monday.setChecked(true);
        i++;
        if(availabilityList[i].toInt())
            m_tuesday.setChecked(true);
        i++;
        if(availabilityList[i].toInt())
            m_wednesday.setChecked(true);
        i++;
        if(availabilityList[i].toInt())
            m_thursday.setChecked(true);
        i++;
        if(availabilityList[i].toInt())
            m_friday.setChecked(true);
    }
    i++;
    if(availabilityList[i].toInt())// Journée
    {
        enableDay(2);
        m_day.setChecked(true);
        return;
    }

    i++;
    if(availabilityList[i].toInt())// Matin
    {
        m_morning.setChecked(true);
        m_morningHoursCB.setEnabled(true);
        i++;
        if(availabilityList[i].toInt())// Matin heures
        {
            m_morningHoursCB.setChecked(true);
            i++;
            m_morningHoursTE.setEnabled(true);
            m_morningHoursTE.setTime(QTime(availabilityList[i].section(":", 0, 0).toInt(), availabilityList[i].section(":", 1, 1).toInt()));
        }
    }
    i++;
    if(availabilityList[i].toInt())// Après-Midi
    {
        m_afternoon.setChecked(true);
        m_afternoonHoursCB.setChecked(true);
        i++;
        if(availabilityList[i].toInt())// Après-Midi heures
        {
            m_afternoonHoursCB.setEnabled(true);
            i++;
            m_afternoonHoursTE.setEnabled(true);
            m_afternoonHoursTE.setTime(QTime(availabilityList[i].section(":", 0, 0).toInt(), availabilityList[i].section(":", 1, 1).toInt()));
        }
    }
}

void PatientForm::alternationLocationButtons(bool alternation)// Slot pour masquer / démasquer de l'adresse du patient et alternance bouton Cabinet / Domicile
{
    if(!alternation)
    {
        if(m_mainLayout->rowCount() == 20)
            m_officeButton.toggle();
        else
            m_homeButton.toggle();
        return;
    }

    if(m_mainLayout->rowCount() == 20)
    {
        m_officeButton.toggle();
        QHBoxLayout *city1Layout { new QHBoxLayout };
        QHBoxLayout *city2Layout { new QHBoxLayout };
        m_mainLayout->insertRow(14, &m_addressLabel);
        m_mainLayout->insertRow(15, "Rue :", city1Layout);
            city1Layout->addWidget(&m_address);
            city1Layout->addWidget(&m_addressPicture);
        m_mainLayout->insertRow(16, "Ville :", city2Layout);
            city2Layout->addWidget(&m_city);
            city2Layout->addWidget(&cityAddButton);
            city2Layout->addWidget(&cityWithdrawButton);
        m_addressLabel.show();
        m_address.show();
        m_addressPicture.show();
        m_city.show();
        cityAddButton.show();
        cityWithdrawButton.show();
        return;
    }

    m_homeButton.toggle();
    m_mainLayout->itemAt(16, QFormLayout::LabelRole)->widget()->hide();
    m_mainLayout->itemAt(15, QFormLayout::LabelRole)->widget()->hide();
    m_mainLayout->takeRow(16);
    m_mainLayout->takeRow(15);
    m_mainLayout->takeRow(14);
    m_addressLabel.hide();
    m_address.hide();
    m_addressPicture.hide();
    m_city.hide();
    cityAddButton.hide();
    cityWithdrawButton.hide();
}

void PatientForm::alternationYearMonthButtons()// Slot pour alterner ans et mois sur le bouton associé
{
    if(m_yearmonth.text() == "ans")
        m_yearmonth.setText("mois");
    else
        m_yearmonth.setText("ans");
}

void PatientForm::nameFieldFormatting(const QString & text)// Slot pour formater correctement le champ du nom
{
    int const cursorPosition { m_name.cursorPosition() };
    m_name.setText(text.toUpper());
    m_name.setCursorPosition(cursorPosition);
    if(!m_name.text().isEmpty() || (m_phone1.hasAcceptableInput() && !phone1().isEmpty()) || (m_phone2.hasAcceptableInput() && !phone2().isEmpty()))
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void PatientForm::firstnameFieldFormatting(const QString & text)// Slot pour formater correctement le champ du prénom
{
    int cursorPosition { m_firstname.cursorPosition() };
    QStringList list { text.split(' ') };
    for(int i { 0 }; i < list.size(); i++)
        list[i] = list[i].left(1).toUpper() + list[i].mid(1).toLower();
    m_firstname.setText(list.join(' '));
    m_firstname.setCursorPosition(cursorPosition);
}

void PatientForm::phone1FieldLightening(const QString & text)// Slot pour surligner le champ téléphone 1
{
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(!text.contains(QRegExp(R"(^\d{2}\.\d{2}\.\d{2}\.\d{2}\.\d{2}$)")) && !phone1().isEmpty())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_phone1.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_phone1.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(phone1().isEmpty())
        highlighteningMarginsEditEffect->setBlurRadius(1);
    if(!m_name.text().isEmpty() || (m_phone1.hasAcceptableInput() && !phone1().isEmpty()) || (m_phone2.hasAcceptableInput() && !phone2().isEmpty()))
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void PatientForm::phone2FieldLightening(const QString & text)// Slot pour surligner le champ téléphone 2
{
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(!text.contains(QRegExp(R"(^\d{2}\.\d{2}\.\d{2}\.\d{2}\.\d{2}$)")) && !phone2().isEmpty())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_phone2.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_phone2.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(phone2().isEmpty())
        highlighteningMarginsEditEffect->setBlurRadius(1);
    if(!m_name.text().isEmpty() || (m_phone1.hasAcceptableInput() && !phone1().isEmpty()) || (m_phone2.hasAcceptableInput() && !phone2().isEmpty()))
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void PatientForm::emailFieldLightening(const QString & text)// Slot pour formater correctement le champ email
{
    int const cursorPosition { m_email.cursorPosition() };
    QStringList list { text.split(' ') };
    m_email.setText(text.toLower());
    m_email.setCursorPosition(cursorPosition);
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(!text.contains(QRegExp(R"(^\b[a-z0-9\-\_\.]+@[a-z0-9\_\-]+\.[a-z]{2,3}\b$)")) && !text.isEmpty())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_email.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_email.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(text.isEmpty())
        highlighteningMarginsEditEffect->setBlurRadius(1);
    if(!m_name.text().isEmpty() || (m_phone1.hasAcceptableInput() && !phone1().isEmpty()) || (m_phone2.hasAcceptableInput() && !phone2().isEmpty()))
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void PatientForm::doctorFieldFormatting(const QString & text)// Slot pour formater correctement le médecin
{
    int const cursorPosition { m_doctor.cursorPosition() };
    QStringList list { text.split(' ') };
    for(int i { 0 }; i < list.size(); i++)
        list[i] = list[i].left(1).toUpper() + list[i].mid(1).toLower();
    m_doctor.setText(list.join(' '));
    m_doctor.setCursorPosition(cursorPosition);
}

void PatientForm::enableWeek(int state)// Slot pour griser / dégriser les jours de la semaine
{
    if(m_other.isChecked())
    {
        m_monday.setChecked(false);
        m_monday.setEnabled(false);
        m_tuesday.setChecked(false);
        m_tuesday.setEnabled(false);
        m_wednesday.setChecked(false);
        m_wednesday.setEnabled(false);
        m_thursday.setChecked(false);
        m_thursday.setEnabled(false);
        m_friday.setChecked(false);
        m_friday.setEnabled(false);
        return;
    }

    if(state == 2)
    {
        m_monday.setChecked(true);
        m_monday.setEnabled(false);
        m_tuesday.setChecked(true);
        m_tuesday.setEnabled(false);
        m_wednesday.setChecked(true);
        m_wednesday.setEnabled(false);
        m_thursday.setChecked(true);
        m_thursday.setEnabled(false);
        m_friday.setChecked(true);
        m_friday.setEnabled(false);
        return;
    }

    m_monday.setChecked(false);
    m_monday.setEnabled(true);
    m_tuesday.setChecked(false);
    m_tuesday.setEnabled(true);
    m_wednesday.setChecked(false);
    m_wednesday.setEnabled(true);
    m_thursday.setChecked(false);
    m_thursday.setEnabled(true);
    m_friday.setChecked(false);
    m_friday.setEnabled(true);
}

void PatientForm::enableDay(int state)// Slot pour griser / dégriser le matin et l'après-midi
{
    m_morning.setChecked(false);
    m_afternoon.setChecked(false);
    if(state == 2 || m_other.isChecked())
    {
        m_morning.setEnabled(false);
        m_afternoon.setEnabled(false);
        enableHalfDay(0);
        if(state == 2)
        {
            m_morning.setChecked(true);
            m_afternoon.setChecked(true);
        }
        return;
    }

    m_morning.setEnabled(true);
    m_afternoon.setEnabled(true);
}

void PatientForm::enableHalfDay(int state)// Slot pour griser / dégriser les CheckBox du matin et de l'après-midi
{
    if(m_morning.checkState() == state)
    {
        if(state == 2 && !m_day.isChecked())
            m_morningHoursCB.setEnabled(true);
        else
        {
            m_morningHoursCB.setChecked(false);
            m_morningHoursCB.setEnabled(false);
            enableHours(0);
        }
    }
    if(m_afternoon.checkState() == state)
    {
        if(state == 2 && !m_day.isChecked())
            m_afternoonHoursCB.setEnabled(true);
        else
        {
            m_afternoonHoursCB.setChecked(false);
            m_afternoonHoursCB.setEnabled(false);
            enableHours(0);
        }
    }
}

void PatientForm::enableHours(int state)// Slot pour griser / dégriser les horaires de disponibilité
{
    if(m_morningHoursCB.checkState() == state)
    {
        if(state == 2)
        {
            m_morningHoursTE.setEnabled(true);
            m_morningHoursTE.setFocus();
        }
        else
            m_morningHoursTE.setEnabled(false);
    }
    if(m_afternoonHoursCB.checkState() == state)
    {
        if(state == 2)
        {
            m_afternoonHoursTE.setEnabled(true);
            m_afternoonHoursTE.setFocus();
        }
        else
            m_afternoonHoursTE.setEnabled(false);
    }
}

void PatientForm::enableOther(int state)// Slot pour griser / dégriser le champ personnalisé de disponibilité
{
    if(state == 2)
    {
        m_otherLE.setEnabled(true);
        enableWeek(0);
        enableDay(0);
        m_week.setChecked(false);
        m_week.setEnabled(false);
        m_day.setChecked(false);
        m_day.setEnabled(false);
        return;
    }

    m_otherLE.setEnabled(false);
    enableWeek(0);
    enableDay(0);
    m_week.setEnabled(true);
    m_day.setEnabled(true);
}

void PatientForm::pathologyFieldFormatting()// Slot pour formater correctement les champs de téléphone
{
    QString truncatedText { m_pathology.toPlainText() };
    QString charactersLeft { "Il reste " };
    charactersLeft += QString::number(255 - truncatedText.size());
    charactersLeft += " caractère(s)";
    m_charactersLeftLabel.setText(charactersLeft);
    truncatedText.resize(255);
    if(m_pathology.toPlainText().size() > 255)
    {
        m_pathology.setPlainText(truncatedText);
        m_pathology.moveCursor(QTextCursor::End);
    }
}

void PatientForm::calldateFieldLightening(const QString & text)// Slot pour surligner le champ date d'appel
{
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(!text.contains(QRegExp(R"(^\d{4}-\d{2}-\d{2}$)")))
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_callDateLE.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_callDateLE.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(!m_name.text().isEmpty() || (m_phone1.hasAcceptableInput() && !phone1().isEmpty()) || (m_phone2.hasAcceptableInput() && !phone2().isEmpty()))
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void PatientForm::addressChanged(const QString & text)// Slot pour actualiser le lien Google Maps
{
    Q_UNUSED(text);
    m_addressPicture.setLink("https://www.google.fr/maps/place/" + address().replace(" ", "+") + ",+" + m_city.currentText().replace(" ", "+"));
}

void PatientForm::cityAddButtonClicked()// Slot pour ajouter une nouvelle ville
{
    QString newCity { QInputDialog::getText(this, tr("Nouvelle Ville"), tr("Quelle ville souhaitez vous ajouter ?"), QLineEdit::Normal, QString(), nullptr, Qt::Dialog) };
    if(newCity.size() > 40)
    {
        error(403, this);
        return;
    }

    if(newCity.isEmpty())
        return;

    newCity = newCity.toUpper().simplified();
    QSettings const settings;
    QSqlQuery sql_CityList(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    sql_CityList.prepare("SELECT COUNT(name) FROM Cities WHERE name = :name");
    sql_CityList.bindValue(":name", newCity);
    if(!sql_CityList.exec())
    {
        error(404, this, sql_CityList.lastError().text());
        return;
    }

    sql_CityList.next();
    if(sql_CityList.value(0).toInt() > 0)
    {
        error(405, this);
        return;
    }

    sql_CityList.prepare("INSERT INTO Cities (id, name) VALUES (NULL, :name)");
    sql_CityList.bindValue(":name", newCity);

    if(!sql_CityList.exec())
    {
        error(406, this, sql_CityList.lastError().text());
        return;
    }

    QMessageBox::information(this, tr("Insertion nouvelle ville"), tr("La ville <strong>") + newCity + tr("</strong> a bien été ajoutée."));
    cityListFilling(newCity);
}

void PatientForm::cityWithdrawButtonClicked()// Slot pour retirer une ville
{
    QSettings const settings;
    QSqlQuery sql_CitySuppr(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    sql_CitySuppr.prepare("SELECT name FROM Cities WHERE id = 1");
    if(!sql_CitySuppr.exec())
    {
        error(407, this, sql_CitySuppr.lastError().text());
        return;
    }

    sql_CitySuppr.next();
    if(m_city.currentText() == sql_CitySuppr.value("name").toString())
        return;

    if(QMessageBox::question(this, tr("Retirer la Ville"), tr("Souhaitez-vous retirer la ville de <strong>") + m_city.currentText() + tr("</strong> ?<br/>Tous les patients associés à cette ville (s'il en reste) se verront attribuer <strong>") + sql_CitySuppr.value("name").toString() + tr("</strong>."), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    QSqlDatabase::database().transaction();
    sql_CitySuppr.prepare("UPDATE Active_Patients SET city_id = 1 WHERE city_id = :city_id");
    sql_CitySuppr.bindValue(":city_id", m_cityMap.key(m_city.currentText()));
    if(!sql_CitySuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(408, this, sql_CitySuppr.lastError().text());
        return;
    }

    sql_CitySuppr.prepare("UPDATE Archived_Patients SET city_id = 1 WHERE city_id = :city_id");
    sql_CitySuppr.bindValue(":city_id", m_cityMap.key(m_city.currentText()));
    if(!sql_CitySuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(409, this, sql_CitySuppr.lastError().text());
        return;
    }

    sql_CitySuppr.prepare("DELETE FROM Cities WHERE id = :city_id");
    sql_CitySuppr.bindValue(":city_id", m_cityMap.key(m_city.currentText()));
    if(!sql_CitySuppr.exec())
    {
        QSqlDatabase::database().rollback();
        error(410, this, sql_CitySuppr.lastError().text());
        return;
    }

    QSqlDatabase::database().commit();
    QMessageBox::information(this, tr("Retrait de la ville"), tr("La ville <strong>") + m_city.currentText() + tr("</strong> a bien été retirée."));
    m_city.removeItem(m_city.currentIndex());
    m_cityMap.remove(m_cityMap.key(m_city.currentText()));
    emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
    emit refreshTransmission("ArchivesEditor#Archives");
}

void PatientForm::acceptButtonClicked()// Slot pour fermer le formulaire après vérification du bon remplissage des champs
{
    // Contrôle de conformité des champs de date d'appel et numéros de téléphone (normalement inutile car le bouton "enregistrer" est bloqué en cas de données incomplètes)
    if(!m_callDateLE.text().contains(QRegExp(R"(^\d{4}-\d{2}-\d{2}$)")) || (!m_phone1.hasAcceptableInput() && m_phone1.text() != "....") || (!m_phone2.hasAcceptableInput() && m_phone2.text() != "...."))
    {
        if(!m_callDateLE.text().contains(QRegExp(R"(^\d{4}-\d{2}-\d{2}$)")))
            error(411, this);
        if(!m_phone1.hasAcceptableInput() && m_phone1.text() != "....")
            error(412, this);
        if(!m_phone2.hasAcceptableInput() && m_phone2.text() != "....")
            error(413, this);
        return;
    }

    // Contrôle du minimum nécessaire à l'enregistrement d'un patient
    QString message { tr("Attention certains champs ne sont pas remplis :<br />") };
    if(m_name.text().isEmpty() || (m_phone1.text() == "...." && m_phone2.text() == "....") || (m_homeButton.isChecked() && m_address.text().isEmpty()))// Champ(s) manquant(s)
    {
        if(m_name.text().isEmpty())
            message += QString(tr("- Le <strong>nom du patient</strong> n'est pas renseigné.<br />"));
        if(m_phone1.text() == "...." && m_phone2.text() == "....")
            message += QString(tr("- Aucun <strong>téléphone du patient</strong> n'est pas renseigné.<br />"));
        if(m_address.text().isEmpty() && m_homeButton.isChecked())
            message += QString(tr("- L'<strong>adresse du patient</strong> n'est pas renseigné.<br />"));
        message += tr("Voulez-vous confirmez ?");

        if(QMessageBox::question(this, tr("Formulaire Incomplet"), message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)// Test forçage de l'enregistrement du formulaire
            return;

        if(m_name.text().isEmpty() && m_phone1.text() == "...." && m_phone2.text() == "....")
        {
            error(414, this);
            return;
        }
    }

    QSettings const settings;
    QSqlQuery sql_RegisteredData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    QList<QString> const genderList { "", tr("Mr "), tr("Mme "), tr("Enft ") };

    // Modification d'un patient pré-existant
    if(m_id != 0)
    {
        sql_RegisteredData.prepare("UPDATE Active_Patients SET location = :location, gender = :gender, age = :age, name = :name, firstname = :firstname, phone1 = :phone1, phone2 = :phone2, email = :email, doctor = :doctor,"
                      "kine_id = :kine_id, availability = :availability, address = :address, city_id = :city_id, pathology = :pathology, no_emergency = :no_emergency, call_date = :call_date WHERE id = :id");
        sql_RegisteredData.bindValue(":id", id());
        sql_RegisteredData.bindValue(":location", location());
        sql_RegisteredData.bindValue(":gender", gender());
        sql_RegisteredData.bindValue(":age", age());
        sql_RegisteredData.bindValue(":name", name());
        sql_RegisteredData.bindValue(":firstname", firstname());
        sql_RegisteredData.bindValue(":phone1", phone1());
        sql_RegisteredData.bindValue(":phone2", phone2());
        sql_RegisteredData.bindValue(":email", email());
        sql_RegisteredData.bindValue(":doctor", doctor());
        sql_RegisteredData.bindValue(":kine_id", kine());
        sql_RegisteredData.bindValue(":availability", availability());
        sql_RegisteredData.bindValue(":address", address());
        sql_RegisteredData.bindValue(":city_id", city());
        sql_RegisteredData.bindValue(":pathology", pathology());
        sql_RegisteredData.bindValue(":no_emergency", no_emergency());
        sql_RegisteredData.bindValue(":call_date", callDate());

        if(!sql_RegisteredData.exec())
        {
            error(415, this, sql_RegisteredData.lastError().text());
            return;
        }

        QMessageBox::information(this, tr("Modification du patient"), tr("Le patient <strong>") + genderList[gender()] + name() + " " + firstname() + tr("</strong> a bien été mis à jour !"));
        m_accessor["actualisationType"] = "PatientEdit";
        m_accessor["id"] = QString::number(id());
        m_accessor["WaitingList"] = (location() == 0 ? "Office" : "Home");
        m_accessor["OldWaitingList"] = (m_originLocation == 0 ? "Office" : "Home");
        close();
        return;
    }

    // Nouveau Patient
    // Recherche de doublons
    sql_RegisteredData.prepare("SELECT p.location, p.gender, p.name AS p_name, p.firstname AS p_firstname, p.age, p.phone1, p.phone2, p.email, p.doctor, k.name AS k_name, k.firstname AS k_firstname, p.address, c.name AS c_name, p.pathology, p.no_emergency, p.call_date FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id WHERE (p.name = :name OR p.phone1 = :phone1 OR p.phone2 = :phone2)");
    sql_RegisteredData.bindValue(":name", m_name.text());
    sql_RegisteredData.bindValue(":phone1", (m_phone1.text().isEmpty() ? "a" : m_phone1.text()));
    sql_RegisteredData.bindValue(":phone2", (m_phone2.text().isEmpty() ? "a" : m_phone2.text()));

    if(!sql_RegisteredData.exec())
    {
        error(416, this, sql_RegisteredData.lastError().text());
        return;
    }

    QString overlapState("New");
    int i { 0 };
    while(sql_RegisteredData.next())
        i++;// numberRowAffected() ne veut pas marcher...
    if(i != 0)
    {
        sql_RegisteredData.seek(-1);
        OverlapForm OverlapForm(patientData(), sql_RegisteredData, this);
        OverlapForm.setWindowTitle(tr("Patient en doublon"));
        OverlapForm.setFont(QFont("Bahnschrift", 14));
        OverlapForm.setMinimumSize(100,100);// Pour éviter le "setGeometry: Unable to set geometry"
        OverlapForm.exec();
        overlapState = OverlapForm.state();
    }

    // Enregistrement Nouveau Patient
    if(overlapState == "New")
    {
        sql_RegisteredData.prepare("INSERT INTO Active_Patients (id, location, gender, age, name, firstname, phone1, phone2, email, doctor, kine_id, availability, address, city_id, pathology, no_emergency, call_date) "
                      "VALUES (:id, :location, :gender, :age, :name, :firstname, :phone1, :phone2, :email, :doctor, :kine_id, :availability, :address, :city_id, :pathology, :no_emergency, :call_date)");
        sql_RegisteredData.bindValue(":id", QVariant(QVariant::Int));
        sql_RegisteredData.bindValue(":location", location());
        sql_RegisteredData.bindValue(":gender", gender());
        sql_RegisteredData.bindValue(":age", age());
        sql_RegisteredData.bindValue(":name", name());
        sql_RegisteredData.bindValue(":firstname", firstname());
        sql_RegisteredData.bindValue(":phone1", phone1());
        sql_RegisteredData.bindValue(":phone2", phone2());
        sql_RegisteredData.bindValue(":email", email());
        sql_RegisteredData.bindValue(":doctor", doctor());
        sql_RegisteredData.bindValue(":kine_id", kine());
        sql_RegisteredData.bindValue(":availability", availability());
        sql_RegisteredData.bindValue(":address", address());
        sql_RegisteredData.bindValue(":city_id", city());
        sql_RegisteredData.bindValue(":pathology", pathology());
        sql_RegisteredData.bindValue(":no_emergency", no_emergency());
        sql_RegisteredData.bindValue(":call_date", callDate());

        if(!sql_RegisteredData.exec())
        {
            error(417, this, sql_RegisteredData.lastError().text());
            return;
        }

    QMessageBox::information(this, tr("Insertion nouveau patient"), tr("Le patient <strong>") + genderList[gender()] + name() + " " + firstname() + tr("</strong> a bien été ajouté !"));
    QSqlQuery queryRechercheIDAjout(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
        queryRechercheIDAjout.first();
        m_accessor["actualisationType"] = "Addition";
        m_accessor["id"] = sql_RegisteredData.lastInsertId().toString();
        m_accessor["WaitingList"] = (location() == 0 ? "Office" : "Home");
        m_accessor["OldWaitingList"] = (location() == 0 ? "Office" : "Home");
        close();
    }
}
