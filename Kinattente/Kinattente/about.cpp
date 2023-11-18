#include "about.h"

About::About(QWidget *parent) : QWidget(parent)// Constructeur
{
    m_logo = new LinkedPicture(QSize(150, 150), "Images/Logo.png", "https://kinattente.000webhostapp.com/", this);
    QPixmap logoPixmap { QPixmap("Images/Kinattente.png") };
        m_title.setPixmap(logoPixmap);
        m_title.setScaledContents(true);
        m_title.setFixedSize(200, 50);

    QSettings settings;
    QString display;
        display.append("Version : " + settings.value("Version").toString() + " (32Bits)<br/><br/><br/><br/>");
        display.append("Kinattente est un logiciel libre conçu par un kiné passionné qui vous veut du bien.<br/><br/>");
        display.append("Vous souhaitez aider ? Ou en savoir plus ? rendez-vous sur le site internet de Kinattente : <br/><br/>");
        display.append("<a href='https://kinattente.000webhostapp.com/'>https://kinattente.000webhostapp.com/</a>");
    m_display.setText(display);
    m_display.setOpenExternalLinks(true);

    m_closeButton.setText(tr("Fermer"));

    setLayout(&m_HLayout);
    m_HLayout.addWidget(m_logo);
    m_HLayout.addLayout(&m_VLayout);
        m_VLayout.addWidget(&m_title);
        m_VLayout.addWidget(&m_display);
        m_VLayout.addWidget(&m_closeButton, 0, Qt::AlignRight);

    connect(&m_closeButton, &QPushButton::clicked, this, &About::closeButtonClicked);
}

About::~About()// Destructeur
{

}

void About::closeButtonClicked()// Méthode pour fermer la fenêtre à propos
{
    close();
}
