#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include "windowtitlebar.h"
#include <QWidget>
#include <QDesktopWidget>
#include <QPainter>
#include <QGridLayout>
#include <QSizeGrip>

class CustomWindow : public QWidget
{
    Q_OBJECT

    public:
    explicit CustomWindow(QWidget *child = nullptr, QString title = QString());
    ~CustomWindow();

    protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    private:
    void CenterOnScreen();
    QPixmap *m_Cache;
    QGridLayout m_MainLayout;
    WindowTitleBar m_TitleBar;
    QSizeGrip m_SizeGrip;
};

#endif // CUSTOMWINDOW_H
