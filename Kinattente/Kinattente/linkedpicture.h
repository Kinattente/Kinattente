#ifndef LINKEDPICTURE_H
#define LINKEDPICTURE_H

#include <QLabel>
#include <QDesktopServices>
#include <QUrl>

class LinkedPicture : public QLabel
{
    Q_OBJECT

    public:
    LinkedPicture(QSize size, QString picture, QString link, QWidget *parent = nullptr);// Constructeur
    ~LinkedPicture();// Destructeur

    signals:
    void launchUpdate(bool const forcingUpdate = false);// Signal pour lancer la recherche de nouvelle mise à jour

    private:
    void mouseReleaseEvent(QMouseEvent* event) override;

    protected:
    QString m_link;
};

#endif // LINKEDPICTURE_H
