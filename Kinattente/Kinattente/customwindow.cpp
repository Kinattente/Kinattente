#include "customwindow.h"

CustomWindow::CustomWindow(QWidget *child, QString title) : QWidget(), m_TitleBar(title), m_SizeGrip(this)
{
    setWindowFlags(Qt::FramelessWindowHint);

    m_MainLayout.setMargin(0);  // No space between window's element and the border
    m_MainLayout.setSpacing(0); // No space between window's element
    setLayout(&m_MainLayout);
    m_MainLayout.addWidget(&m_TitleBar, 0, 0, 1, 1);
    m_MainLayout.setRowStretch(1, 1); // Put the title bar at the top of the window
    m_MainLayout.addWidget(child);
}

CustomWindow::~CustomWindow()
{

}

void CustomWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    CenterOnScreen();
}

void CustomWindow::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  if(m_Cache != NULL)
  {
    QPainter painter(this);

    painter.drawPixmap(0, 0, *m_Cache);
  }
}

void CustomWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    delete m_Cache;
    m_Cache = new QPixmap(size());
    m_Cache->fill(Qt::transparent);

    QPainter painter(m_Cache);

    QColor const darkBlue ( 23,  23,  34);
    QColor const lightBlue(177, 177, 203);

    /********** Window's background **********/
    QPolygon background;

    background << QPoint(           0,            16)
             << QPoint(          16,             0)
             << QPoint(width() -  1,             0)
             << QPoint(width() -  1, height() - 33)
             << QPoint(width() - 17, height() - 17)
             << QPoint(         272, height() - 17)
             << QPoint(         256, height() -  1)
             << QPoint(          16, height() -  1)
             << QPoint(          16,           272)
             << QPoint(           0,           256);

    painter.setPen  (QPen  (darkBlue));
    painter.setBrush(QBrush(darkBlue));

    painter.drawPolygon(background);
    /*****************************************/

    /********** Window's frame **********/
    QPolygon frame;

    frame << QPoint(           4,            20)
        << QPoint(          20,             4)
        << QPoint(width() -  4,             4)
        << QPoint(width() -  4, height() - 37)
        << QPoint(width() - 20, height() - 21)
        << QPoint(         268, height() - 21)
        << QPoint(         252, height() -  5)
        << QPoint(          20, height() -  5)
        << QPoint(          20,           268)
        << QPoint(           4,           252);

    painter.setPen  (QPen(lightBlue));
    painter.setBrush(Qt::NoBrush    );

    painter.drawPolygon(frame);
    /*****************************************/

    m_SizeGrip.move  (width() - 32, height() - 49);
    m_SizeGrip.resize(          32,            32);
}

void CustomWindow::CenterOnScreen()
{
    QDesktopWidget screen;

    QRect const screenGeom { screen.screenGeometry(this) };

    int const screenCenterX { screenGeom.center().x() };
    int const screenCenterY { screenGeom.center().y() };

    move(screenCenterX - width () / 2,
        screenCenterY - height() / 2);
}

