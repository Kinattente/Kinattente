#include "labelpushbutton.h"

LabelPushButton::LabelPushButton(buttonType buttonType, bool emergency, QWidget *parent) : QPushButton{ parent }, m_cache{ nullptr }, m_buttonType { buttonType }, m_emergency{ emergency }, m_hoverState { false }// Constructeur
{
    setCursor(Qt::PointingHandCursor);
}

LabelPushButton::~LabelPushButton()// Destructeur
{

}

QSize LabelPushButton::sizeHint() const
{
    return QSize(40, 40);
}

QSize LabelPushButton::minimumSizeHint() const
{
    return QSize(40, 40);
}

QSize LabelPushButton::maximumSizeHint() const
{
    return QSize(40, 40);
}

void LabelPushButton::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    delete m_cache;
    m_cache = new QPixmap(size());
    m_cache->fill(Qt::transparent);

    QPainter painter(m_cache);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPoint const points[4] = {
            QPoint(4, 4),
            QPoint(width() - 4, 4),
            QPoint(width() - 4, 36),
            QPoint(4, 36)};

    if(m_hoverState)
    {
        QLinearGradient gradient { QLinearGradient(0.5, 40, 0.5, 0) };
        if(m_emergency)
        {
            gradient.setColorAt(0, QColor(255, 0, 0, 255));
            gradient.setColorAt(1, QColor(255, 118, 87, 255));
        }
        else
        {
            gradient.setColorAt(0, QColor(0, 113, 255, 255));
            gradient.setColorAt(1, QColor(91, 171, 252, 255));
        }

        QPen const pen(QBrush(gradient), 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setBrush(QBrush(gradient));
        painter.setPen(pen);
        painter.drawPolygon(points, 4);
    }
    else
    {
        QPen const pen(QColor(255,255,255), 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setBrush(QColor(255,255,255));
        painter.setPen(pen);
        painter.drawPolygon(points, 4);
    }

    painter.setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(m_buttonType == Edit)
    {
        painter.drawLine(12, 12, 21, 12);
        painter.drawLine(12, 12, 12, 28);
        painter.drawLine(12, 28, 28, 28);
        painter.drawLine(28, 19, 28, 28);
        painter.setPen(QPen(QBrush(Qt::black), 3));
        painter.drawLine(27, 13, 22, 18);
        painter.setBrush(Qt::black);
        painter.setPen(QPen(QBrush(Qt::black), 1));
        QPoint const triangle[3] = {
                QPoint(19, 19),
                QPoint(21, 21),
                QPoint(17, 23)};
        painter.drawPolygon(triangle, 3);
    }
    else
    {
        painter.drawLine(13, 13, 27, 27);
        painter.drawLine(13, 27, 27, 13);
    }
}

void LabelPushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if(m_cache != NULL)
      {
        QPainter painter(this);
        painter.drawPixmap(0, 0, *m_cache);
      }
}

void LabelPushButton::enterEvent(QEvent * event)
{
    Q_UNUSED(event);
    m_hoverState = true;
    resize(10, 10);
    resize(40, 40);
}

void LabelPushButton::leaveEvent(QEvent * event)
{
    Q_UNUSED(event);
    m_hoverState = false;
    resize(10, 10);
    resize(40, 40);
}

