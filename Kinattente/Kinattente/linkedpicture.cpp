#include "linkedpicture.h"

LinkedPicture::LinkedPicture(QSize size, QString picture, QString link, QWidget *parent) : QLabel(parent), m_link(link)// Constructeur
{
    QPixmap logoPixmap { QPixmap(picture) };
    setPixmap(logoPixmap);
    setScaledContents(true);
    setFixedSize(size);
    setCursor(Qt::PointingHandCursor);
}

LinkedPicture::~LinkedPicture()// Destructeur
{

}

void LinkedPicture::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if(m_link == "Update")
        emit launchUpdate();
    else
        QDesktopServices::openUrl(QUrl(m_link));
}

