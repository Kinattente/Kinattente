#ifndef WINDOWTITLEBAR_H
#define WINDOWTITLEBAR_H

#include "windowbutton.h"
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>

class WindowTitleBar : public QWidget
{
    Q_OBJECT
    public:
    explicit WindowTitleBar(QString title, QWidget *parent = nullptr);
    ~WindowTitleBar();

    public slots:
    void Minimized();
    void Maximized();
    void Quit();

    protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent  *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    private:
    QPixmap *m_Cache;
    QLabel m_Title;
    WindowButton m_Minimize;
    WindowButton m_Maximize;
    WindowButton m_Close;
    QPoint m_Diff;
};

#endif // WINDOWTITLEBAR_H
