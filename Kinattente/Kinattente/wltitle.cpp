#include "wltitle.h"

WLTitle::WLTitle(const QString &titleText, const QStringList &waitingTimesInformations, QWidget *parent) : QWidget{parent}, m_titleText{titleText}, m_waitingTimesInformations{waitingTimesInformations}, m_cache{nullptr}// Constructeur
{

}

WLTitle::~WLTitle()// Destructeur
{

}

QSize WLTitle::sizeHint() const
{
    return QSize(2000, 43);
}

QSize WLTitle::minimumSizeHint() const
{
    return QSize(243, 43);
}

void WLTitle::refreshTitle(const QStringList &waitingTimesInformations)
{
    m_waitingTimesInformations = waitingTimesInformations;
    resize(10,10);
}

void WLTitle::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    delete m_cache;
    m_cache = new QPixmap(size());
    m_cache->fill(Qt::transparent);

    QPainter painter(m_cache);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient gradient { QLinearGradient(0, 0, 0, 20) };
    gradient.setColorAt(0, QColor(0,113,255,255));
    gradient.setColorAt(1, QColor(91,171,252,255));

    QBrush const background(gradient);

    QPen const pen(background, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen const pen2(QBrush(Qt::white), 1);
    QPen const pen3(QBrush(Qt::red), 1);

    painter.setBrush(background);
    painter.setPen(pen);

    QPoint const points[4] = {
            QPoint(3, 3),
            QPoint(width() - 43, 3),
            QPoint(width() - 3, 40),
            QPoint(3, 40)};

    painter.drawPolygon(points, 4);

    painter.setPen(pen2);
    painter.setFont(QFont("Bahnschrift", 24, 99));
    painter.drawText(QRectF(QPointF(0, 0), QPointF(400, 40)), Qt::AlignCenter, m_titleText + " : " + m_waitingTimesInformations[0] + " patient(s)");

    painter.setFont(QFont("Bahnschrift", 12, 20));
    painter.drawText(QRectF(QPointF(width() - 300, 0), QPointF(width() - 150, height())), Qt::AlignCenter, "Délai d'attente : " + m_waitingTimesInformations[1] + "j");
    painter.setPen(pen3);
    painter.drawText(QRectF(QPointF(width() - 150, 0), QPointF(width() - 50, height())), Qt::AlignCenter, "Urgences : " + m_waitingTimesInformations[2] + "j");
}

void WLTitle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if(m_cache != NULL)
      {
        QPainter painter(this);
        painter.drawPixmap(0, 0, *m_cache);
      }
}

