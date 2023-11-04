#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QThread>
#include <QtSql>
#include <QSqlDatabase>


#include "variables.h"

bool OnMatch();
bool OnSituation();
bool OnMainMenu();
int PlayerXP();
int PlayerDeaths();
int PlayerKills();
int RemainingEnemies();

std::string PlayerName();

void ConnectDatabase();

int GetAllBaseAddress();

int id = 0;
std::string playerName = "Alessandro";
std::string uplayId = "test";
int kills = 1;
int assistances = 1;
int deaths = 1;
int xp = 100;
int timePlayed = 1;
int remainingEnemies = 0;


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
        xp = PlayerXP();
        deaths = PlayerDeaths();
        playerName = PlayerName();
        kills = PlayerKills();
        remainingEnemies = RemainingEnemies();

        qInfo() << "On Match: " + QString::number(onMatch);
        qInfo() << "On Situation: " + QString::number(onSituation);
        qInfo() << "On Main Menu: " + QString::number(onMainMenu);
        qInfo() << "PlayerName: " + playerName;
        qInfo() << "PlayerXP: " + QString::number(xp);
        qInfo() << "PlayerDeaths: " + QString::number(deaths);
        qInfo() << "PlayerKills: " + QString::number(kills);
        qInfo() << "RemainingEnemies: " + QString::number(remainingEnemies);

        QThread::msleep(100);
    }


    //ConnectDatabase();

    return a.exec();
}


void ConnectDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={SQL SERVER};SERVER=Alessandro\\SQLEXPRESS;DATABASE=Users;Trusted_Connection=Yes;");
    if(db.open())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO Users (PlayerName, UplayID, Kills, Assistances, Deaths, XP, TimePlayed) "
                      "VALUES (:playerName, :uplayId, :kills, :assistances, :deaths, :xp, :timePlayed)");
        query.bindValue(":playerName", QString::fromStdString(playerName));
        query.bindValue(":uplayId", QString::fromStdString(uplayId));
        query.bindValue(":kills", kills);
        query.bindValue(":assistances", assistances);
        query.bindValue(":deaths", deaths);
        query.bindValue(":xp", xp);
        query.bindValue(":timePlayed", timePlayed);

        if (query.exec()) {
             qInfo() << "Logrado con éxito";
        } else {
            qWarning() << "Error al actualizar valores en la tabla stats: " << query.lastError().text();
        }

    }
    else
    {
        qDebug() << "error : " + db.lastError().text();
    }
}



