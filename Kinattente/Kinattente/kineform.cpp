#include "kineform.h"

KineForm::KineForm(int const id, QWidget *parent) : QDialog(parent), m_id{ id }, m_colorOk{ false }// Constructeur
{
    // Récupération dans la BDD des informations du kiné avec l'id
    QSettings const settings;
    QSqlQuery sql_KineData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(id != 0)
    {
        m_colorOk = true;
        sql_KineData.prepare("SELECT * FROM Kines WHERE id = :id");
        sql_KineData.bindValue(":id", id);
        if(!sql_KineData.exec())
        {
            error(500, this, sql_KineData.lastError().text());
            QTimer::singleShot(200, this, &KineForm::close);
            return;
        }
        else
            sql_KineData.first();
    }

    // Création des lignes du formulaire
    QLabel *title { new QLabel("<center>Nouveau Kiné</center>") };
        title->setFont(QFont("MS Shell Dlg 2", 12));
    m_name.setText(id == 0 ? "" : sql_KineData.value("name").toString());
    m_name.setFocus();
    m_firstname.setText(id == 0 ? "" : sql_KineData.value("firstname").toString());
    QLabel *activeLabel { new QLabel("kiné actif :") };
        activeLabel->setFont(QFont("MS Shell Dlg 2", 8));
    m_colorBouton.setText(tr("Choix"));
    m_active.setCheckState(id == 0 || sql_KineData.value("active").toBool() ? Qt::Checked : Qt::Unchecked);
    if(id == 0)
        m_active.hide();

    m_visualization.setMaximumHeight(45);
    m_visualization.setFrameShadow(QFrame::Sunken);
    m_visualization.setFrameShape(QFrame::Panel);
    m_visualization.setLineWidth(1);
    m_visualization.setMidLineWidth(1);
    m_color.setNamedColor((id == 0 ? "#FFFFFF" : sql_KineData.value("color").toString()));
    m_colorVisualization.setFixedSize(14,13);
    m_colorVisualization.setStyleSheet("background-color: " + m_color.name() + ";border-radius: 3px;");
    m_nameVisualization.setText(id == 0 ? "" : sql_KineData.value("name").toString() + " " + sql_KineData.value("firstname").toString());

    // Créations des boutons Enregistrer et Annuler
    m_acceptButton.setText(tr("Enregistrer"));
        m_acceptButton.setDefault(true);
        m_acceptButton.setToolTip(tr("Enregistrer un nouveau kiné"));
        m_acceptButton.setCursor(Qt::PointingHandCursor);
        if(id == 0)
            m_acceptButton.setEnabled(false);
    QPushButton *cancelButton { new QPushButton (tr("Annuler")) };
        cancelButton->setToolTip(tr("Annuler"));
        cancelButton->setCursor(Qt::PointingHandCursor);

    // Mise en formulaire
    QGridLayout *mainLayout { new QGridLayout };
    QFormLayout *layoutPartieSuperieure { new QFormLayout };
    QHBoxLayout *layoutVisualization { new QHBoxLayout };

    setLayout(mainLayout);
        mainLayout->addLayout(layoutPartieSuperieure, 0,0,1,2);
            layoutPartieSuperieure->addRow(title);
            layoutPartieSuperieure->addRow(tr("&Nom :"), &m_name);
            layoutPartieSuperieure->addRow(tr("&Prénom :"), &m_firstname);
            layoutPartieSuperieure->addRow(tr("&Couleur :"), &m_colorBouton);
            if(id != 0)
                layoutPartieSuperieure->addRow(activeLabel, &m_active);
            layoutPartieSuperieure->setLabelAlignment(Qt::AlignRight);
        mainLayout->addWidget(&m_visualization, 1,0,1,2);
            m_visualization.setLayout(layoutVisualization);
                layoutVisualization->addWidget(&m_colorVisualization);
                layoutVisualization->addWidget(&m_nameVisualization);
                layoutVisualization->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(&m_acceptButton, 2,0,1,1);
        mainLayout->addWidget(cancelButton, 2,1,1,1);

    // Connexion des champs pour édition
    connect(&m_name, &QLineEdit::textEdited, this, &KineForm::nameFieldFormatting);
    connect(&m_firstname, &QLineEdit::textEdited, this, &KineForm::firstnameFieldFormatting);

    // Connexion des champs avec la Visualization
    connect(&m_name, &QLineEdit::textEdited, this, &KineForm::refreshVisualization);
    connect(&m_firstname, &QLineEdit::textEdited, this, &KineForm::refreshVisualization);

    // Connexion des boutons Enregistrer, Annuler et Couleur
    connect(&m_colorBouton, &QPushButton::clicked, this, &KineForm::colorDialog);
    connect(&m_acceptButton, &QPushButton::clicked, this, &KineForm::acceptButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &KineForm::close);
}

KineForm::~KineForm()// Destructeur
{

}

int KineForm::id() const// Accesseur
{
    return m_id;
}

QString KineForm::name() const// Accesseur
{
    return m_name.text().simplified();
}

QString KineForm::firstname() const// Accesseur
{
    return m_firstname.text().simplified();
}

QString KineForm::color() const// Accesseur
{
    return m_color.name();
}

bool KineForm::active() const// Accesseur
{
    return m_active.isChecked();
}

void KineForm::nameFieldFormatting(const QString & text)// Slot pour formater correctement le champ du nom
{
    int const cursorPosition { m_name.cursorPosition() };
    m_name.setText(text.toUpper());
    m_name.setCursorPosition(cursorPosition);
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(text.toUpper() == "AUCUN" || text.isEmpty())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_name.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_name.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(m_name.text().isEmpty())
        highlighteningMarginsEditEffect->setBlurRadius(1);
    if(!m_name.text().isEmpty() && !m_firstname.text().isEmpty() && m_colorOk)
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void KineForm::firstnameFieldFormatting(const QString & text)// Slot pour formater correctement le champ du prénom
{
    int const cursorPosition { m_firstname.cursorPosition() };
    QStringList list { text.split(' ') };
    for(int i { 0 }; i < list.size(); i++)
        list[i] = list[i].left(1).toUpper() + list[i].mid(1).toLower();
    m_firstname.setText(list.join(' '));
    m_firstname.setCursorPosition(cursorPosition);
    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(text.isEmpty())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_firstname.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_firstname.setGraphicsEffect(highlighteningMarginsEditEffect);
    if(m_firstname.text().isEmpty())
        highlighteningMarginsEditEffect->setBlurRadius(1);
    if(!m_name.text().isEmpty() && !m_firstname.text().isEmpty() && m_colorOk)
        m_acceptButton.setEnabled(true);
    else
        m_acceptButton.setEnabled(false);
}

void KineForm::colorDialog()// Slot pour ouvrir la boite de dialogue couleur et changer la couleur du carré
{
    m_color = QColorDialog::getColor(m_color, this);
    QSettings const settings;

    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    QSqlQuery sql_KineColor(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    sql_KineColor.prepare("SELECT color FROM Kines WHERE id != :id");
    sql_KineColor.bindValue(":id", m_id);
    if(!sql_KineColor.exec())
    {
        error(501, this, sql_KineColor.lastError().text());
        return;
    }

    while (sql_KineColor.next())
    {
        if(m_color.name() == sql_KineColor.value("color").toString() || m_color.rgb() == QColor(Qt::gray).rgb() || m_color.rgb() == QColor(Qt::black).rgb())
        {
            error(502, this);
            QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
                highlighteningMarginsEditEffect->setColor(EditoutlineColor);
            m_colorBouton.setGraphicsEffect(highlighteningMarginsEditEffect);
            m_acceptButton.setEnabled(false);
            break;
        }
        else
        {
            QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
                highlighteningMarginsEditEffect->setColor(EditoutlineColor);
            m_colorBouton.setGraphicsEffect(highlighteningMarginsEditEffect);
            m_colorVisualization.setStyleSheet("background-color: " + m_color.name() + ";border-radius: 3px;");
            m_colorOk = true;
            if(!m_name.text().isEmpty() && !m_firstname.text().isEmpty() && m_colorOk)
                m_acceptButton.setEnabled(true);
            else
                m_acceptButton.setEnabled(false);
        }
    }
}

void KineForm::refreshVisualization()// Slot pour actualiser la Visualization
{
    m_nameVisualization.setText("<center>" + m_name.text().toUpper() + " " + m_firstname.text().left(1).toUpper() + m_firstname.text().mid(1).toLower() + "</center>");
}

void KineForm::acceptButtonClicked()// Slot pour fermer le formulaire après vérification du bon remplissage des champs
{

    if(m_name.text().isEmpty() || m_firstname.text().isEmpty() || m_color.rgb() == QColor(Qt::gray).rgb() || (m_active.isChecked() && m_color.rgb() == QColor(Qt::black).rgb()))
    {
        QString message { tr("Attention certains champs ne sont pas bien remplis :<br />") };
        if(m_name.text().isEmpty())
            message += QString(tr("- Le <strong>nom du kiné</strong> n'est pas renseigné.<br />"));
        if(m_firstname.text().isEmpty())
            message += QString(tr("- Le <strong>prénom du kiné</strong> n'est pas renseigné.<br />"));
        if(m_color.rgb() == QColor(Qt::gray).rgb() || (m_color.rgb() == QColor(Qt::black).rgb() && m_active.isChecked()))
            message += QString(tr("- Les <strong>couleurs noire et blanche</strong> sont réservées. Veuillez changer."));
        QMessageBox::information(this, tr("Formulaire Incomplet"), message);
        return;
    }

    QSettings const settings;
    QSqlQuery sql_DataInsert(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    if(id() == 0)
    {
        sql_DataInsert.prepare("INSERT INTO Kines (id, name, firstname, color, active) VALUES (:id, :name, :firstname, :color, :active)");
        sql_DataInsert.bindValue(":id", QVariant(QVariant::Int));
        sql_DataInsert.bindValue(":name", name());
        sql_DataInsert.bindValue(":firstname", firstname());
        sql_DataInsert.bindValue(":color", color());
        sql_DataInsert.bindValue(":active", active());

        if(!sql_DataInsert.exec())
            error(503, this, sql_DataInsert.lastError().text());
        else
        {
            QMessageBox::information(this, tr("Insertion nouveau Kiné"), tr("Le kiné <strong>") + name() + " " + firstname() + tr("</strong> a bien été ajouté !"));
            done(QDialog::Accepted);
        }
        return;
    }

    sql_DataInsert.prepare("UPDATE Kines SET name = :name, firstname = :firstname, color = :color, active = :active, active_string = :active_string WHERE id = :id");
    sql_DataInsert.bindValue(":id", id());
    sql_DataInsert.bindValue(":name", name());
    sql_DataInsert.bindValue(":firstname", firstname());
    sql_DataInsert.bindValue(":color", (active() == 1 ? color() : "black"));
    sql_DataInsert.bindValue(":active", active());
    sql_DataInsert.bindValue(":active_string", (active() ? "Oui" : "Non"));

    if(!sql_DataInsert.exec())
    {
        error(504, this, sql_DataInsert.lastError().text());
        return;
    }

    QMessageBox::information(this, tr("Modification Kiné"), tr("Le kiné <strong>") + name() + " " + firstname() + tr("</strong> a bien été mis à jour !"));
    done(QDialog::Accepted);
}
