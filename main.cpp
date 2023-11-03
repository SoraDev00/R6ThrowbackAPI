#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QThread>

#include "variables.h"

bool OnMatch();
bool OnSituation();
bool OnMainMenu();



int GetAllBaseAddress();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "R6ThrowbackAPI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    GetAllBaseAddress();

    while(true)
    {
        bool onMatch = OnMatch();
        bool onSituation = OnSituation();
        bool onMainMenu = OnMainMenu();
        qInfo() << "On Match: " + QString::number(onMatch);
        qInfo() << "On Situation: " + QString::number(onSituation);
        qInfo() << "On Main Menu: " + QString::number(onMainMenu);

        QThread::msleep(1);
    }




    return a.exec();
}




