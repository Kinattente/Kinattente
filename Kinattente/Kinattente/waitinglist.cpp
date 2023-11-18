#include "waitinglist.h"

WaitingList::WaitingList(const QString &location, const QStringList &researchField, QWidget *parent) : QTabWidget(parent), m_researchFieldTimer {new QTimer(this)}, m_researchFieldText{researchField}// Constructeur
{
    // Ajout des onglets à QWidgetTab
    QSettings const settings;
    QStringList const researchFieldsText { researchFieldsTextFilling() };
    QLabel carreCouleur(this);
        carreCouleur.setFixedSize(14,13);

    // Onglet "Tous"
    m_sql.append("FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id WHERE p.location = " + location);
    addTab(new QWidget(this), "Tous");
    tabsFilling();
    labelsFilling(m_sql.indexOf(m_sql.last()), researchFieldsText[m_sql.indexOf(m_sql.last())]);

    // Onglets des kinés actifs
    QSqlQuery sql_ActiveKines(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(!sql_ActiveKines.exec("SELECT * FROM Kines WHERE active AND name != 'AUCUN' ORDER BY name"))
        error(300, this, sql_ActiveKines.lastError().text());
    else
    {
        while (sql_ActiveKines.next())
        {
            carreCouleur.setStyleSheet("background-color: " + sql_ActiveKines.value("color").toString() + ";");
            m_sql.append("FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id WHERE p.location = " + location + " AND p.kine_id='" + sql_ActiveKines.value("id").toString() + "'");
            addTab(new QWidget(this), QIcon(carreCouleur.grab()), (sql_ActiveKines.value("name").toString() + " " + sql_ActiveKines.value("firstname").toString()));
            tabsFilling();
            labelsFilling(m_sql.indexOf(m_sql.last()), researchFieldsText[m_sql.indexOf(m_sql.last())]);
        }
    }

    // Onglet "Autres" si existance d'anciens kinés
    QSqlQuery sql_InactiveKines(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(!sql_InactiveKines.exec("SELECT DISTINCT active FROM Kines WHERE NOT active"))
        error(301, this, sql_InactiveKines.lastError().text());
    else
    {
        if(sql_InactiveKines.next())
        {
            carreCouleur.setStyleSheet("background-color: #000000;");
            m_sql.append("FROM Active_Patients AS p INNER JOIN Kines AS k ON p.kine_id = k.id INNER JOIN Cities AS c ON p.city_id = c.id WHERE p.location = " + location + " AND NOT k.active");
            addTab(new QWidget(this), QIcon(carreCouleur.grab()), "Autres");
            tabsFilling();
            labelsFilling(m_sql.indexOf(m_sql.last()), researchFieldsText[m_sql.indexOf(m_sql.last())]);
        }
    }
}

WaitingList::~WaitingList()// Destructeur
{

}

void WaitingList::scrollListToSelectedPatient(int tabIndex, int patientIndexPosition)// Méthode qui prend l'id d'un patient et l'index d'un onglet dans lequel faire défiler le scroll pour rendre le patient choisi visible
{
    m_scrollList[tabIndex]->ensureWidgetVisible((*m_labels[tabIndex])[patientIndexPosition]);
}

void WaitingList::withdrawalLabel(int id)// Méthode qui prend l'id d'un patient pour le retirer des listes de Labels, m_waitingTime et m_listNumber
{
    int tabIndex { 0 };
    int patientIndex { 0 };
    for(auto tab : idList())
    {
        patientIndex = tab->indexOf(id);
        if(patientIndex != -1)
        {
            delete (*m_labels[tabIndex])[patientIndex];
            m_labels[tabIndex]->removeAt(patientIndex);
            (*m_waitingTime[tabIndex])[0]->remove((*m_waitingTime[tabIndex])[0]->key(id));
            (*m_waitingTime[tabIndex])[1]->remove((*m_waitingTime[tabIndex])[1]->key(id));
        }
    tabIndex++;
    }
}

QList<QList<int>*> WaitingList::idList()// Méthode qui prend un id et renvoie l'arborescence des id
{
    QList<QList<int>*> id;
    int tabIndex;
    for(auto labels : qAsConst(m_labels))
    {
        id.append(new QList<int>);
        tabIndex = m_labels.indexOf(labels);
        for(auto label : *labels)
            id[tabIndex]->append(label->id());
    }
    return id;
}

QStringList WaitingList::researchFieldsTextFilling()// Méthode qui rempli un QStringList avec les champs de recherche ou les initialise
{
    if(m_researchFieldText.isEmpty())
    {
        QStringList researchFieldText;
        QSettings const settings;
        QSqlQuery sql_ActiveKines(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
        if(!sql_ActiveKines.exec("SELECT active FROM Kines WHERE active UNION ALL SELECT DISTINCT active FROM Kines WHERE NOT active"))
            error(302, this, sql_ActiveKines.lastError().text());
        else
            while (sql_ActiveKines.next())
                researchFieldText.append(QString());
        return researchFieldText;
    }
    else
        return m_researchFieldText;
}

QList<QScrollArea*> &WaitingList::scrollList()// Accesseur nécessaire pour faire défiler le scroll et afficher le patient voulu
{
    return m_scrollList;
}

QStringList WaitingList::researchFields()// Accesseur
{
    QStringList researchFields;
    for(auto const researchfield : qAsConst(m_researchFields))
        researchFields << researchfield->text();
    return researchFields;
}

QList<QList<Label*>*> WaitingList::labels() const// Accesseur
{
    return m_labels;
}

QStringList WaitingList::waitingTimesInformations(int tabIndex) const// Accesseur permettant en lui passant un numéro onglet de renvoyer les delais d'attente des patients non urgents et urgents
{
    return QStringList() << QString::number((*m_waitingTime[tabIndex])[0]->size() + (*m_waitingTime[tabIndex])[1]->size())
                         << QString::number((*m_waitingTime[tabIndex])[0]->size() == 0 ? 0 : ((*m_waitingTime[tabIndex])[0]->key((*m_waitingTime[tabIndex])[0]->last(), 0)))
                         << QString::number((*m_waitingTime[tabIndex])[1]->size() == 0 ? 0 : ((*m_waitingTime[tabIndex])[1]->key((*m_waitingTime[tabIndex])[1]->last(), 0)));
}

int WaitingList::patientIndex(int id, int tabIndex)// Accesseur
{
    return idList()[tabIndex]->indexOf(id);
}

void WaitingList::researchFieldTimer(int const &tabIndex, QString researchField)// Slot pour lancer la minuterie laissant le temps d'écrire avant de faire l'actualisation de la recherche dans la liste d'attente
{
    // On déconnecte un éventuel compte à rebours déjà lancé et on le stop
    m_researchFieldTimer->disconnect();
    if(m_researchFieldTimer->isActive())
        m_researchFieldTimer->stop();

    // On connecte le compte à rebours à l'actualisation des étiquettes et à son propre arrêt (car compte à rebours en boucle) et on le lance
    connect(m_researchFieldTimer, &QTimer::timeout, [this, tabIndex, researchField]() {labelsFilling(tabIndex, researchField);});
    connect(m_researchFieldTimer, &QTimer::timeout, m_researchFieldTimer, &QTimer::stop);
    m_researchFieldTimer->start(750);
}

void WaitingList::tabsFilling()// Méthode pour remplir les onglets avec un QScrollArea, un QLineEdit pré-rempli si besoin et un QList<Label*>. Création des délais d'attente de cet onglet. Connexion du QLineEdit
{
    int const tabIndex { m_scrollList.size() };
        m_scrollList.append(new QScrollArea(this));
            m_scrollList.last()->setWidgetResizable(true);// Permet de prendre toute la largeur disponible
            m_scrollList.last()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_researchFields.append(new QLineEdit(this));
            m_researchFields.last()->setText(m_researchFieldText.isEmpty() ? "" : m_researchFieldText[tabIndex]);
        m_labels.append(new QList<Label*>);
        m_waitingTime.append(new QList<QMultiMap<int, int>*>);// Insertion de cet onglet dans la liste des délais d'attente
            m_waitingTime[tabIndex]->append(new QMultiMap<int, int>);// Insertion pour cet onglet de la liste des délais d'attente non urgents
            m_waitingTime[tabIndex]->append(new QMultiMap<int, int>);// Insertion pour cet onglet de la liste des délais d'attente urgents

        widget(tabIndex)->setLayout(new QVBoxLayout);
            widget(tabIndex)->layout()->addWidget(m_scrollList.last());
                m_scrollList.last()->setWidget(new QWidget(this));
                m_scrollList.last()->widget()->setStyleSheet(".QWidget {background-color: rgb(249, 249, 249);border-radius: 4px;border-top-right-radius: 7px;border-bottom-right-radius: 7px;}");
                    m_scrollList.last()->widget()->setLayout(new QVBoxLayout);
                        m_scrollList.last()->widget()->layout()->addWidget(new QLabel("Rechercher :", this));
                        m_scrollList.last()->widget()->layout()->addWidget(m_researchFields.last());
                        m_scrollList.last()->widget()->layout()->setAlignment(Qt::AlignTop);

        connect(m_researchFields.last(), &QLineEdit::textEdited, [this, tabIndex](const QString &text) {researchFieldTimer(tabIndex, text);});
}

void WaitingList::labelsFilling(int const &tabIndex, QString researchField)// Slot pour remplir (ou actualiser lors d'une recherche) un onglet de LA avec les patients correspondants
{
    // Remplissage de m_cles, nécessaire car la selection des étiquettes n'est pas continue (du fait du champ recherche) donc on ne peut pas les numéroter les unes après les autres, elles ne se suivent pas
    QSettings const settings;
    QMap<int, int> ListNumber;
    QSqlQuery sql_PatientsID(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    QString const PatientsIDRequest {"SELECT p.id " + m_sql[0] + " ORDER BY p.no_emergency, p.call_date"};

    if(!sql_PatientsID.exec(PatientsIDRequest))
    {
        error(303, this, sql_PatientsID.lastError().text());
        return;
    }

    while(sql_PatientsID.next())
        ListNumber.insert(sql_PatientsID.at() + 1, sql_PatientsID.value("id").toInt());

    // Vidage de la liste des étiquettes précédentes si elle est remplie
    while(!m_labels[tabIndex]->isEmpty())
        delete m_labels[tabIndex]->takeFirst();

    // Finalisation de la requête
    QString labelsRequest { m_sql[tabIndex] };
    if(!researchField.isEmpty())
        labelsRequest.append(" AND (p.name LIKE'%" + researchField + "%' OR p.firstname LIKE'%" + researchField + "%' OR p.pathology LIKE'%" + researchField + "%')");
    labelsRequest.prepend("SELECT p.id, p.location, p.gender, p.age, p.name, p.firstname, p.phone1, p.phone2, p.email, p.doctor, k.name, k.firstname, p.availability, p.address, c.name, p.pathology, p.no_emergency, p.call_date, k.color ");
    labelsRequest.append(" ORDER BY p.no_emergency, p.call_date");

    QSqlQuery sql_PatientData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(!sql_PatientData.exec(labelsRequest))
    {
        error(304, this, sql_PatientData.lastError().text());
        return;
    }

    if(sql_PatientData.next())
    {
        sql_PatientData.previous();

        while (sql_PatientData.next())
        {
            m_labels[tabIndex]->append(new Label(sql_PatientData, QString::number(ListNumber.key(sql_PatientData.value("id").toInt())), this));
            m_scrollList[tabIndex]->widget()->layout()->addWidget(m_labels[tabIndex]->last());
            (*m_waitingTime[tabIndex])[!sql_PatientData.value("no_emergency").toBool()]->insert(sql_PatientData.value("call_date").toDateTime().daysTo(QDateTime::currentDateTime()), sql_PatientData.value("id").toInt());
        }
    }
    emit refresh();
}
