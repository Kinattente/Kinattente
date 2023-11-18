/*
 * @file    main.cpp
 * @author  Weber Arnold <kinattente@gmail.com>
 * @version 3.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Kinattente software : allows you to manage a waiting list for one or more physiotherapists on one or more workstations.
 *
 */

#include "customwindow.h"
#include "mainwindow.h"
#include <QApplication>// Pour le main
#include <QTranslator>// Pour la traduction
#include <QLocale>// Pour la traduction
#include <QLibraryInfo>// Pour la traduction
#include <QIcon>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPixmap const pixmap("Images/SplashScreen.png");
    QSplashScreen splash(pixmap);
    splash.show();

    app.setWindowIcon(QIcon("Images/Logo.png"));

    // Traduction en Français
    QString const locale { QLocale::system().name().section('_', 0, 0) };
    QTranslator translator;
    translator.load(QString("qt_") + locale, "translations");
    app.installTranslator(&translator);

    // Récupération de QSetting
    QCoreApplication::setOrganizationDomain("https://kinattente.wordpress.com/");
    QCoreApplication::setApplicationName("Kinattente");

/*
    QFile file("film.mov");
    file.open(QIODevice::ReadOnly);
    QByteArray data { file.readAll() };
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(data);
    QByteArray hah { md5.result() };
    QString md5Text { hah.toHex() };qDebug() << md5Text;
*/

    // Execution de la fenêtre principale
    MainWindow *MainWindowKinattente { new MainWindow };
    // Code pour redessiner le contour de chaque fenêtre en suspens car ne semble pas fonctionner
    //CustomWindow *CustomWindow { new class CustomWindow(MainWindowKinattente, "Kinattente") };

    if (!MainWindowKinattente->hasProblemOccurred())
    {
        MainWindowKinattente->showMaximized();
        splash.finish(MainWindowKinattente);
        return app.exec();
    }
    else
        return 1;
}
