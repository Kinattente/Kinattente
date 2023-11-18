#ifndef LABELPUSHBUTTON_H
#define LABELPUSHBUTTON_H

#include <QPushButton>
#include <QtGui>

class LabelPushButton : public QPushButton
{
    Q_OBJECT

    public:
    enum buttonType {Edit, Withdrawal };
    LabelPushButton(buttonType buttonType, bool emergency, QWidget *parent = nullptr);
    ~LabelPushButton();// Destructeur
    QSize minimumSizeHint() const override;
    QSize maximumSizeHint() const ;
    QSize sizeHint() const override;

    protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEvent * event) override;
    virtual void leaveEvent(QEvent * event) override;
    QPixmap *m_cache;
    buttonType m_buttonType;
    bool m_emergency;
    bool m_hoverState;
};

#endif // LABELPUSHBUTTON_H
