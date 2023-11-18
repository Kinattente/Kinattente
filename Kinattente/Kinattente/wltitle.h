#ifndef WLTITLE_H
#define WLTITLE_H

#include <QtGui>
#include <QWidget>

class WLTitle : public QWidget
{
    Q_OBJECT

    public:
    explicit WLTitle(const QString &titleText, const QStringList &waitingTimesInformations, QWidget *parent = nullptr);// Constructeur
    ~WLTitle();// Destructeur
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void refreshTitle(const QStringList &waitingTimesInformations);

    protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QString m_titleText;
    QStringList m_waitingTimesInformations;
    QPixmap *m_cache;
};

#endif // WLTITLE_H
