#ifndef ABOUT_H
#define ABOUT_H

#include <linkedpicture.h>
#include <QWidget>
#include <QHBoxLayout>
#include <QSettings>
#include <QPushButton>

class About : public QWidget
{
    Q_OBJECT

    public:
    explicit About(QWidget *parent = nullptr);// Constructeur
    About(About const&) = delete;// Constructeur de copie
    About& operator=(About const&) = delete;// Opérateur de copie
    ~About();// Destructeur


    private slots:
    void closeButtonClicked();// Méthode pour fermer la fenêtre à propos

    protected:
    LinkedPicture *m_logo;
    QLabel m_title;
    QLabel m_display;
    QPushButton m_closeButton;
    QHBoxLayout m_HLayout;
    QVBoxLayout m_VLayout;
};

#endif // ABOUT_H
