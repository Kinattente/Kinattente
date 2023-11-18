#include "options.h"

Options::Options(QWidget *parent) : QDialog(parent), m_applyIPAddress(true), m_applySaveOption(true), m_applySavePath(true), m_applyCSS(true), m_applyCity(true), m_applyBoot(true), m_applyData(true)// Constructeur
{
    QSettings const settings;

    QStandardItem *ipAddressItem { new QStandardItem() };
        ipAddressItem->setData(QVariant(QIcon("Images/IPAddress.png")), Qt::DecorationRole);
        ipAddressItem->setData(tr("Adresse IP"), Qt::DisplayRole);

    QStandardItem *saveItem { new QStandardItem() };
        saveItem->setData(QVariant(QIcon("Images/Export.png")), Qt::DecorationRole);
        saveItem->setData(tr("Sauvegardes"), Qt::DisplayRole);

    QStandardItem *appearanceItem { new QStandardItem() };
        appearanceItem->setData(QVariant(QIcon("Images/ColorPalette.png")), Qt::DecorationRole);
        appearanceItem->setData(tr("Apparence"), Qt::DisplayRole);

    QStandardItem *cityItem { new QStandardItem() };
        cityItem->setData(QVariant(QIcon("Images/GPS.png")), Qt::DecorationRole);
        cityItem->setData(tr("Ville par Défaut"), Qt::DisplayRole);

    QStandardItem *bootItem { new QStandardItem() };
        bootItem->setData(QVariant(QIcon("Images/Boot.png")), Qt::DecorationRole);
        bootItem->setData(tr("Démarrage"), Qt::DisplayRole);

    QStandardItem *dataItem { new QStandardItem() };
        dataItem->setData(QVariant(QIcon("Images/Data.png")), Qt::DecorationRole);
        dataItem->setData(tr("Données Patients"), Qt::DisplayRole);

        QStandardItem *multiworkplaceItem { new QStandardItem() };
            multiworkplaceItem->setData(QVariant(QIcon("Images/MultiWorkplace.png")), Qt::DecorationRole);
            multiworkplaceItem->setData(tr("Evoluer vers plusieurs postes"), Qt::DisplayRole);

        m_listModel.insertRows(0, 6);
        m_listModel.setItem(0, 0, ipAddressItem);
        m_listModel.setItem(1, 0, saveItem);
        m_listModel.setItem(2, 0, appearanceItem);
        m_listModel.setItem(3, 0, cityItem);
        m_listModel.setItem(4, 0, bootItem);
        m_listModel.setItem(5, 0, dataItem);
        if(settings.value("BDD/Driver").toString() == "QSQLITE")
        {
            m_listModel.insertRows(m_listModel.rowCount(), 1);
            m_listModel.setItem(6, 0, multiworkplaceItem);
        }

        m_listView.setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_listView.setModel(&m_listModel);

    QString IPAddressDisplay { QString(tr("Adresse IP du poste Principal :")) };
    m_IPAddressLineEdit.setInputMask("900.900.900.900;_");
    QLabel *IPAddressLabel { new QLabel(tr("L'adresse IP ne doit être changée que si votre poste principal a changé d'adresse IP récemment ou si vous souhaitez changer de poste principal.")) };
    IPAddressLabel->setWordWrap(true);

    if(settings.value("BDD/MasterWorkplace").toBool() || settings.value("BDD/Driver").toString() == "QSQLITE")
    {
        m_IPAddressLineEdit.setReadOnly(true);
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
                 m_IPAddressLineEdit.setText(address.toString());
    }
    else
        m_IPAddressLineEdit.setText(settings.value("BDD/IPAddress").toString());

    QLabel *saveDisplay { new QLabel(tr("Sauvegarde automatique à la fermeture :")) };
    m_saveButton.setText((settings.value("BDD/SaveOption").toBool() ? "Activé !" : "Désactivé !"));
    QLabel *savePathDisplay { new QLabel(tr("Chemin de sauvegarde :")) };
    m_savePath.setText(settings.value("BDD/SavePath").toString());
    m_savePath.setReadOnly(true);
    m_savePath.setEnabled((settings.value("BDD/SaveOption").toBool() ? true : false));
    QPushButton *savePathButton { new QPushButton(tr("Modifier")) };

    QLabel *cssDisplay { new QLabel(tr("Apparence de Kinattente :")) };
    QDirIterator dirIterator(QDir::currentPath(), QStringList("*.qss") ,QDir::Files | QDir::NoSymLinks);
    while(dirIterator.hasNext())
        m_cssFileList.addItem(dirIterator.next().replace(QRegExp(R"(.*\/([^\/]+)$)"), "\\1"));
    m_cssFileList.setCurrentIndex(m_cssFileList.findText(settings.value("CSS").toString()));
    QPushButton *cssButton { new QPushButton(tr("Ajouter")) };

    QSqlQuery sql_defaultCity(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
    sql_defaultCity.prepare("SELECT name FROM Cities WHERE id = 1");
    if(!sql_defaultCity.exec())
        error(600, this, sql_defaultCity.lastError().text());
    else
    {
        sql_defaultCity.next();
        m_cityLineEdit.setText(sql_defaultCity.value(0).toString());
    }

    m_bootCheckBox.setText(tr("Lancer Kinattente au démarrage"));

    QLabel *dataDisplay { new QLabel(tr("Vous souhaitez conserver les données patients pendant :")) };
    m_dataSpinBox.setRange(1,20);
    m_dataSpinBox.setValue(settings.value("DataDuration").toInt());
    m_dataSpinBox.setSuffix(tr(" ans"));
    m_dataSpinBox.setWrapping(true);
    QLabel *dataDisplay2 { new QLabel(tr("Conformément au <strong>Règlement Général sur la Protection des Données</strong>,<br/>"
                                         "vous ne pouvez conserver les données patients indéfiniment.<br/>"
                                         "A titre d’exemple, les médecins libéraux conservent les dossiers médicaux<br/>"
                                         "des patients pendant 20 ans à compter de leur dernière consultation.<br/>"
                                         "Pour connaître les règlementations en détails, vous pouvez vous rendre <a href='https://www.cnil.fr/fr/rgpd-et-professionnels-de-sante-liberaux-ce-que-vous-devez-savoir/' >ici</a>.")) };
    dataDisplay2->setOpenExternalLinks(true);

    QPushButton *multiWorkplaceButton { new QPushButton(tr("Passer à l'installation sur plusieurs postes...")) };

    m_acceptButton.setText(tr("OK"));
    m_cancelButton.setText(tr("Annuler"));
    m_applyButton.setText(tr("Appliquer"));

    QVBoxLayout *mainLayout { new QVBoxLayout };
    QHBoxLayout *centerLayout { new QHBoxLayout };
    QHBoxLayout *footerLayout { new QHBoxLayout };
    QTabWidget *ipAddressTabWidget { new QTabWidget };
    QTabWidget *saveTabWidget { new QTabWidget };
    QTabWidget *cssTabWidget { new QTabWidget };
    QTabWidget *cityTabWidget { new QTabWidget };
    QTabWidget *bootTabWidget { new QTabWidget };
    QTabWidget *dataTabWidget { new QTabWidget };
    QTabWidget *multiTabWidget { new QTabWidget };
    QWidget *ipAddressWidget { new QWidget };
    QWidget *saveWidget { new QWidget };
    QWidget *cssWidget { new QWidget };
    QWidget *cityWidget { new QWidget };
    QWidget *bootWidget { new QWidget };
    QWidget *dataWidget { new QWidget };
    QWidget *multiWorkplaceWidget { new QWidget };
    QHBoxLayout *ipAddressLayout { new QHBoxLayout };
    QHBoxLayout *saveLayout1 { new QHBoxLayout };
    QHBoxLayout *saveLayout2 { new QHBoxLayout };
    QHBoxLayout *cssLayout { new QHBoxLayout };
    QHBoxLayout *cityLayout { new QHBoxLayout };
    QHBoxLayout *bootLayout { new QHBoxLayout };
    QHBoxLayout *dataLayout { new QHBoxLayout };
    QHBoxLayout *multiWorkplaceLayout { new QHBoxLayout };
    setLayout(mainLayout);
        mainLayout->addLayout(centerLayout);
            centerLayout->addWidget(&m_listView);
            centerLayout->addLayout(&m_stackedLayout);
                m_stackedLayout.addWidget(ipAddressTabWidget);
                    ipAddressTabWidget->addTab(ipAddressWidget, tr("Adresse IP"));
                        ipAddressWidget->setLayout(&m_ipAddressLayout);
                            m_ipAddressLayout.setAlignment(Qt::AlignTop);
                            m_ipAddressLayout.addLayout(ipAddressLayout);
                                ipAddressLayout->addWidget(new QLabel(IPAddressDisplay));
                                ipAddressLayout->addWidget(&m_IPAddressLineEdit);
                            m_ipAddressLayout.addWidget(IPAddressLabel);
                m_stackedLayout.addWidget(saveTabWidget);
                    saveTabWidget->addTab(saveWidget, tr("Sauvegardes"));
                        saveWidget->setLayout(&m_saveLayout);
                            m_saveLayout.setAlignment(Qt::AlignTop);
                            m_saveLayout.addLayout(saveLayout1);
                                saveLayout1->addWidget(saveDisplay);
                                saveLayout1->addWidget(&m_saveButton);
                            m_saveLayout.addLayout(saveLayout2);
                                saveLayout2->addWidget(savePathDisplay);
                                saveLayout2->addWidget(&m_savePath);
                                saveLayout2->addWidget(savePathButton);
                m_stackedLayout.addWidget(cssTabWidget);
                    cssTabWidget->addTab(cssWidget, tr("Apparences"));
                        cssWidget->setLayout(&m_cssLayout);
                            m_cssLayout.setAlignment(Qt::AlignTop);
                            m_cssLayout.addLayout(cssLayout);
                                cssLayout->addWidget(cssDisplay);
                                cssLayout->addWidget(&m_cssFileList);
                                cssLayout->addWidget(cssButton);
                m_stackedLayout.addWidget(cityTabWidget);
                    cityTabWidget->addTab(cityWidget, tr("Ville par défaut"));
                        cityWidget->setLayout(&m_cityLayout);
                            m_cityLayout.setAlignment(Qt::AlignTop);
                            m_cityLayout.addLayout(cityLayout);
                                cityLayout->addWidget(new QLabel(tr("Ville par défaut :")));
                                cityLayout->addWidget(&m_cityLineEdit);
                            m_cityLayout.addWidget(&m_cityDisplay);
                m_stackedLayout.addWidget(bootTabWidget);
                    bootTabWidget->addTab(bootWidget, tr("Démarrage"));
                        bootWidget->setLayout(&m_bootLayout);
                            m_bootLayout.setAlignment(Qt::AlignTop);
                            m_bootLayout.addLayout(bootLayout);
                                bootLayout->addWidget(&m_bootCheckBox);
                m_stackedLayout.addWidget(dataTabWidget);
                    dataTabWidget->addTab(dataWidget, tr("Données Patients"));
                        dataWidget->setLayout(&m_dataLayout);
                            m_dataLayout.setAlignment(Qt::AlignTop);
                            m_dataLayout.addLayout(dataLayout);
                                dataLayout->addWidget(dataDisplay);
                                dataLayout->addWidget(&m_dataSpinBox);
                            m_dataLayout.addWidget(&m_dataDisplay);
                            m_dataLayout.addWidget(dataDisplay2);
                m_stackedLayout.addWidget(multiTabWidget);
                    multiTabWidget->addTab(multiWorkplaceWidget, tr("Evoluer vers plusieurs postes"));
                        multiWorkplaceWidget->setLayout(&m_multiWorkplaceLayout);
                            m_multiWorkplaceLayout.setAlignment(Qt::AlignTop);
                            m_multiWorkplaceLayout.addLayout(multiWorkplaceLayout);
                                multiWorkplaceLayout->addWidget(multiWorkplaceButton);
        mainLayout->addLayout(footerLayout);
            footerLayout->setAlignment(Qt::AlignRight);
            footerLayout->addWidget(&m_acceptButton);
            footerLayout->addWidget(&m_cancelButton);
            footerLayout->addWidget(&m_applyButton);

    connect(&m_IPAddressLineEdit, &QLineEdit::textEdited, this, &Options::IPAddressLineEditEdited);
    connect(&m_saveButton, &QPushButton::clicked, this, &Options::saveButtonClicked);
    connect(savePathButton, &QPushButton::clicked, this, &Options::savePathButtonClicked);
    connect(cssButton, &QPushButton::clicked, this, &Options::cssButtonClicked);
    connect(&m_cssFileList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Options::cssCurrentIndexChanged);
    connect(&m_cityLineEdit, &QLineEdit::textEdited, this, &Options::cityLineEditEdited);
    connect(&m_bootCheckBox, &QCheckBox::stateChanged, this, &Options::bootCheckBoxClicked);
    connect(&m_dataSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Options::dataSpinBoxChanged);
    connect(multiWorkplaceButton, &QPushButton::clicked, this, &Options::multiWorkplaceButtonClicked);
    connect(&m_listView, &QListView::clicked, this, &Options::optionsChange);
    connect(&m_acceptButton, &QPushButton::clicked, this, &Options::acceptButtonClicked);
    connect(&m_cancelButton, &QPushButton::clicked, this, &Options::cancelButtonClicked);
    connect(&m_applyButton, &QPushButton::clicked, this, &Options::applyButtonClicked);
    emit m_listView.clicked(m_listModel.index(0,0));
}

Options::~Options()// Destructeur
{
    
}

void Options::optionsChange(const QModelIndex &index)// Slot pour changer les options à afficher
{
    switch(index.row())
    {
        case 0:
            m_stackedLayout.setCurrentIndex(0);
            break;
        case 1:
            m_stackedLayout.setCurrentIndex(1);
            break;
        case 2:
            m_stackedLayout.setCurrentIndex(2);
            break;
        case 3:
            m_stackedLayout.setCurrentIndex(3);
            break;
        case 4:
            m_stackedLayout.setCurrentIndex(4);
            break;
        case 5:
            m_stackedLayout.setCurrentIndex(5);
            break;
        case 6:
            m_stackedLayout.setCurrentIndex(6);
            break;
        default:
            break;
    }
}

void Options::ipInvalid(const QModelIndex &index)// Slot pour bloquer le changement de page option si l'adresse ip n'est pas valide
{
    Q_UNUSED(index);
    error(601, this);
}

void Options::cityInvalid(const QModelIndex &index)// Slot pour bloquer le changement de page option si la ville par défaut n'est pas valide
{
    Q_UNUSED(index);
    error(602, this);
}

void Options::IPAddressLineEditEdited(const QString &text)// Méthode pour modifier l'adresse IP
{
    QSettings const settings;
    if(settings.value("BDD/IPAddress").toString() == text)
        m_applyIPAddress = true;
    else
        m_applyIPAddress = false;

    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    if(!m_IPAddressLineEdit.hasAcceptableInput())
    {
        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_IPAddressLineEdit.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        m_applyButton.setEnabled(false);
        m_listView.disconnect();
        connect(&m_listView, &QListView::clicked, this, &Options::ipInvalid);
        return;
    }

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_IPAddressLineEdit.setGraphicsEffect(highlighteningMarginsEditEffect);
    m_acceptButton.setEnabled(true);
    m_applyButton.setEnabled(true);
    m_listView.disconnect();
    connect(&m_listView, &QListView::clicked, this, &Options::optionsChange);
}

void Options::saveButtonClicked()// Méthode pour activer / désactiver la sauvegarder automatique
{
    QSettings const settings;
    if(m_saveButton.text() == "Activé !")
    {
        m_saveButton.setText("Désactivé !");
        m_savePath.setEnabled(false);
    }
    else
    {
        m_saveButton.setText("Activé !");
        m_savePath.setEnabled(true);
    }

    if((settings.value("BDD/SaveOption").toBool() && m_saveButton.text() == "Activé !") || (!settings.value("BDD/SaveOption").toBool() && m_saveButton.text() == "Désactivé !"))
        m_applySaveOption = true;
    else
        m_applySaveOption = false;
}

void Options::savePathButtonClicked()// Méthode pour changer l'emplacement de la sauvegarde
{
    QSettings const settings;
    QString const dir { settings.value("BDD/SavePath").toString().isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) : settings.value("BDD/SavePath").toString() };
    QString const savePath { QFileDialog::getExistingDirectory(nullptr, "Chemin de sauvegarde", dir, QFileDialog::ShowDirsOnly) };
    if(!savePath.isEmpty())
        m_savePath.setText(savePath);

    if(savePath == settings.value("BDD/SavePath").toString())
        m_applySavePath = true;
    else
        m_applySavePath = false;
}

void Options::cssButtonClicked()// Méthode pour ajouter un fichier css
{
    QString cssFilePath { QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier CSS"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "CSS (*.qss)") };
    QString newFilePath { cssFilePath };
        newFilePath.replace(QRegExp(R"(.*\/([^\/]+)$)"), "\\1");
        newFilePath.prepend(QDir::currentPath() + "/");
    if(QFile::copy(cssFilePath, newFilePath))
        m_cssFileList.addItem(cssFilePath.replace(QRegExp(R"(.*\/([^\/]+)$)"), "\\1"));
}

void Options::cssCurrentIndexChanged(int index)// Méthode pour changer de fichier css
{
    QSettings const settings;
    if(settings.value("CSS").toString() == m_cssFileList.itemText(index))
        m_applyCSS = true;
    else
        m_applyCSS = false;
}

void Options::cityLineEditEdited(const QString &text)// Slot pour changer le nom de la ville par défaut
{
    int cursorPosition { m_cityLineEdit.cursorPosition() };
    m_cityLineEdit.setText(text.toUpper());
    m_cityLineEdit.setCursorPosition(cursorPosition);

    QGraphicsDropShadowEffect *highlighteningMarginsEditEffect { new QGraphicsDropShadowEffect };
        highlighteningMarginsEditEffect->setOffset(0,0);
        highlighteningMarginsEditEffect->setBlurRadius(5);

    QSettings const settings;
    QSqlQuery sql_defaultCity(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
        sql_defaultCity.exec("SELECT name FROM Cities WHERE id = 1");
        sql_defaultCity.next();
    QString defaultCity { QString(sql_defaultCity.value(0).toString()) };
        sql_defaultCity.prepare("SELECT COUNT(name) FROM Cities WHERE name = :name");
        sql_defaultCity.bindValue(":name", text.toUpper().simplified());
    if(!sql_defaultCity.exec())
    {
        error(603, this, sql_defaultCity.lastError().text());
        m_acceptButton.setEnabled(false);
        m_applyButton.setEnabled(false);
        m_listView.disconnect();
        connect(&m_listView, &QListView::clicked, this, &Options::cityInvalid);
        return;
    }

    sql_defaultCity.next();
    if(text.isEmpty() || (sql_defaultCity.value(0).toInt() > 0 && text.toUpper().simplified() != defaultCity))
    {
        if(sql_defaultCity.value(0).toInt() > 0)
            m_cityDisplay.setText(tr("<i>Cette ville existe déjà dans la liste.</i>"));
        else
            m_cityDisplay.setText(tr("<i>La ville par défaut ne peut être vide.</i>"));

        QColor const EditoutlineColor { QColor::fromRgb(255, 0, 0) };
            highlighteningMarginsEditEffect->setColor(EditoutlineColor);
        m_cityLineEdit.setGraphicsEffect(highlighteningMarginsEditEffect);
        m_acceptButton.setEnabled(false);
        m_applyButton.setEnabled(false);
        m_listView.disconnect();
        connect(&m_listView, &QListView::clicked, this, &Options::cityInvalid);
        return;
    }

    if(defaultCity == text.toUpper().simplified())
        m_applyCity = true;
    else
        m_applyCity = false;

    QColor const EditoutlineColor { QColor::fromRgb(0, 113, 255) };
        highlighteningMarginsEditEffect->setColor(EditoutlineColor);
    m_cityLineEdit.setGraphicsEffect(highlighteningMarginsEditEffect);
    m_cityDisplay.setText("");
    m_acceptButton.setEnabled(true);
    m_applyButton.setEnabled(true);
    m_listView.disconnect();
    connect(&m_listView, &QListView::clicked, this, &Options::optionsChange);
}

void Options::bootCheckBoxClicked(int state)// Slot pour ajouter / retirer Kinattente au démarrage
{
    Q_UNUSED(state);
    QSettings const settings;
    if(settings.value("StartUpLaunch").toBool() == m_bootCheckBox.isChecked())
        m_applyBoot = true;
    else
        m_applyBoot = false;
}

void Options::dataSpinBoxChanged(int i)// Slot pour changer la durée de conservation des données patients
{
    QSettings const settings;
    if(i >= settings.value("DataDuration").toInt())
    {
        m_dataDisplay.setText("");
        if(i == settings.value("DataDuration").toInt())
            m_applyData = true;
        else
            m_applyData = false;
        return;
    }

    m_applyData = false;
    int lostDuration { settings.value("DataDuration").toInt() - i };
    m_dataDisplay.setText(tr("<strong>Attention !!!</strong> Vous allez perdre <strong>DEFINITIVEMENT</strong> ") + QString::number(lostDuration) + tr(" année(s) de données patients."));
}

void Options::multiWorkplaceButtonClicked()// Méthode pour créer un fichier SQL à partir de SQLITE
{
    QMessageBox *msgBox { new QMessageBox(
                    QMessageBox::Question,
                    tr("Passage au multiposte"),
                    tr("Kinattente est actuellement installé pour ce poste uniquement.<br/>"
                   "Si vous souhaitez à présent l'installer sur plusieurs postes, vous devez d'abord installer MySQL sur le <strong>poste principal</strong>.<br/>"
                   "S'il s'agit de ce poste, cliquez sur <strong>Installer MySQL</strong>.<br/>"
                   "Si vous souhaitez l'installer sur un autre poste, cliquez sur <strong>Récupérer la base de données</strong>. Ensuite <strong>Installer Kinattente</strong> sur le poste principal puis <strong>importer votre ancienne base de données</strong>.<br/>"
                   "Si vous souhaitez être accompagné pour ne pas faire d'erreur, réferez-vous au tutoriel détaillé ici :<br/>"
                   "<a href='https://kinattente.wordpress.com/videos/#PassageSoloMulti'>https://kinattente.wordpress.com/videos/#PassageSoloMulti</a>")) };
    msgBox->addButton(tr("Installer MySQL"), QMessageBox::AcceptRole);
    msgBox->addButton(tr("Récupérer la base de données"), QMessageBox::ActionRole);
    msgBox->addButton(tr("Annuler"), QMessageBox::RejectRole);
    int const ret { msgBox->exec() };

    QSettings settings;
    switch(ret)
    {
        case QMessageBox::AcceptRole:
        {
                settings.setValue("OnlyMySQLInstall", true);

            ConfigAssistant *ConfigAssistant { new class ConfigAssistant };
            ConfigAssistant->setModal(true);
            QEventLoop loop;
            connect(ConfigAssistant, &ConfigAssistant::closed, &loop, &QEventLoop::quit);
            connect(ConfigAssistant, &ConfigAssistant::finished, &loop, &QEventLoop::quit);
            ConfigAssistant->show();
            loop.exec();

            if(!settings.value("OnlyMySQLInstall").toBool())
            {
                settings.setValue("BDD/Driver", "QMYSQL");
                settings.setValue("BDD/DatabaseName", "Kinattente");
                settings.setValue("BDD/UserName", "userskine");
                settings.setValue("BDD/Password", "localnetwork");
                settings.setValue("BDD/MasterWorkplace", true);
                settings.setValue("BDD/IPAddress", "127.0.0.1");
                QMessageBox::information(this, tr("Installation terminée"), tr("Le logiciel Kinattente va se fermer pour charger les nouveaux réglages. Vous devrez le relancer vous-même."));
                emit closed();
                close();
            }
            break;
        }
        case QMessageBox::ActionRole:
            break;
        case QMessageBox::RejectRole:
        {
            // Pour une raison que j'ignore, le 2e bouton arrive ici et le 3e dans default...
            emit exportBDD(true);
            if(QMessageBox::question(this, tr("Changement de type poste"), tr("Souhaitez-vous faire passer cet ordinateur en <strong>poste secondaire</strong> ?<br/>"
                                                                              "Attention cette action est <strong>définitive</strong>. Pour revenir en arrière il faudra désinstaller Kinattente et le réinstaller en <strong>poste simple</strong>.")) == QMessageBox::Yes)
            {
                settings.setValue("BDD/Driver", "QMYSQL");
                settings.setValue("BDD/DatabaseName", "Kinattente");
                settings.setValue("BDD/UserName", "userskine");
                settings.setValue("BDD/Password", "localnetwork");
                settings.setValue("BDD/MasterWorkplace", false);
                settings.setValue("BDD/IPAddress", "192.168.0.1");
                QMessageBox::information(this, tr("Nouvelle adresse IP"), tr("Kinattente va maintenant se fermer. Au prochain redémarrage il vous sera demandé l'adresse IP du poste principal.<br/>"
                                                                             "Vous pourrez la trouver sur le poste principal dans l'onglet \"Outils -> Options -> Adresse IP\""));
                close();
            }
            break;
        }
        default:
            // should never be reached
            break;
    }
}

void Options::acceptButtonClicked()// Slot pour valider tous les changements et quitter les options
{
    applyButtonClicked();
    close();
}

void Options::cancelButtonClicked()// Slot pour quitter les options sans changement
{
    close();
}

void Options::applyButtonClicked()// Slot pour valider tous les changements
{
    QSettings settings;
    if(!m_applyIPAddress && QMessageBox::question(this, tr("Changement Adresse IP"), tr("Etes-vous sûr de vouloir changer l'adresse IP pour : <strong>") + m_IPAddressLineEdit.text() + "</strong> ?") == QMessageBox::Yes)
    {
        settings.setValue("BDD/IPAddress", m_IPAddressLineEdit.text());
        emit ipChanged();
        close();
    }

    if(!m_applySaveOption)
    {
        if(m_saveButton.text() == "Activé !")
            settings.setValue("BDD/SaveOption", true);
        else
            settings.setValue("BDD/SaveOption", false);
    }

    if(!m_applySavePath)
    {
        settings.setValue("BDD/SavePath", m_savePath.text());
    }

    if(!m_applyCSS && QMessageBox::question(this, tr("Changement Adresse IP"), tr("Etes-vous sûr de vouloir changer l'apparence de Kinattente pour : <strong>") + m_cssFileList.itemText(m_cssFileList.currentIndex()) + "</strong> ?") == QMessageBox::Yes)
    {
        settings.setValue("CSS", m_cssFileList.itemText(m_cssFileList.currentIndex()));
        emit css();
    }

    if(!m_applyCity && !pendingAction("Modification Ville par défaut échouée", "La modification de la ville par défaut est impossible lorsqu'une action est en cours sur un autre poste.<br/>Veuillez patientez.") && QMessageBox::question(this, tr("Changement Ville par défaut"), tr("Etes-vous sûr de vouloir changer le nom de la ville par défaut pour : <strong>") + m_cityLineEdit.text() + "</strong> ?") == QMessageBox::Yes)
    {
        emit refreshTransmission("PendingAction#" + settings.value("ClientIndex").toString());

        QSqlQuery sql_defaultCity(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
        sql_defaultCity.prepare("UPDATE Cities SET name = :name WHERE id = 1");
        sql_defaultCity.bindValue(":name", m_cityLineEdit.text());

        if(!sql_defaultCity.exec())
            error(604, this, sql_defaultCity.lastError().text());
        else
        {
            emit refreshTransmission("WaitingList#KineEdit#0#Office#Office");
            emit refreshTransmission("ArchivesEditor#Archives");
        }

        emit refreshTransmission("PendingAction#-1");
    }

    if(!m_applyBoot)
    {
        QSettings settings_startup("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);

        if(m_bootCheckBox.isChecked())
        {
            QString const path { QCoreApplication::applicationFilePath().replace('/', '\\') };
            settings_startup.setValue("Kinattente",path);
        }
        else
            settings_startup.remove("Kinattente");

        settings.setValue("StartUpLaunch", m_bootCheckBox.isChecked());
    }

    if(!m_applyData && !pendingAction("Changement de la durée de conservation des données patients échoué", "Le changement de la durée de conservation des données patients est impossible lorsqu'une action est en cours sur un autre poste.<br/>Veuillez patientez.") && QMessageBox::question(this, tr("Changement durée conservation données patients"), tr("Etes-vous sûr de vouloir changer la durée de conservation des données patients pour : <strong>") + m_dataSpinBox.value() + " ans</strong> ?") == QMessageBox::Yes)
    {
        if(settings.value("DataDuration").toInt() <= m_dataSpinBox.value())
        {
            settings.setValue("DataDuration", m_dataSpinBox.value());
            return;
        }

        int lostDuration { settings.value("DataDuration").toInt() - m_dataSpinBox.value() };
        if(QMessageBox::Apply == QMessageBox::warning(this,
                             tr("Changement de la durée de conservation des données patients"),
                             tr("<strong>Attention !!!</strong> Vous allez perdre <strong>DEFINITIVEMENT</strong> ") + QString::number(lostDuration) + tr(" année(s) de données patients.<br/>"
                                "Etes-vous sûr de vouloir confirmer ?"), QMessageBox::Abort|QMessageBox::Apply, QMessageBox::Apply))
        {

            emit refreshTransmission("PendingAction#" + settings.value("ClientIndex").toString());

            QSqlQuery sql_EraseData(QSqlDatabase::database(settings.value("BDD/Driver").toString()));
            sql_EraseData.prepare("DELETE FROM archived_patients WHERE call_date < :call_date");
            sql_EraseData.bindValue(":call_date", QDateTime::currentDateTime().addYears(-m_dataSpinBox.value()).toString("yyyy-MM-dd 00:00:00"));

            if(!sql_EraseData.exec())
                error(605, this, sql_EraseData.lastError().text());
            else
            {
                emit refreshTransmission("ArchivesEditor#Archives");
                settings.setValue("DataDuration", m_dataSpinBox.value());
                m_dataDisplay.setText("");
            }
            emit refreshTransmission("PendingAction#-1");
        }
    }
}
