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
int PlayerWin();
int IsPlaying();

std::string PlayerName();

void ConnectDatabase();

int GetAllBaseAddress();

int id = 0;
std::string playerName = "Alessandro";
std::string uplayId = "test24";
int kills = 1;
int assistances = 50;
int deaths = 1;
int xp = 100;
int timePlayed = 1;
int remainingEnemies = 0;
int playerWin = 0;
int isPlaying = 0;
int isPlayingBuffer = 0;
const char* GetText();

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
        playerWin = PlayerWin();
        isPlaying = IsPlaying();

        if(isPlaying != isPlayingBuffer && onSituation == 0)
        {
            ConnectDatabase();
            isPlayingBuffer = isPlaying;
        }


        qInfo() << "On Match: " + QString::number(onMatch);
        qInfo() << "On Situation: " + QString::number(onSituation);
        qInfo() << "On Main Menu: " + QString::number(onMainMenu);
        qInfo() << "PlayerName: " + playerName;
        qInfo() << "PlayerXP: " + QString::number(xp);
        qInfo() << "PlayerDeaths: " + QString::number(deaths);
        qInfo() << "PlayerKills: " + QString::number(kills);
        qInfo() << "PlayerWin: " + QString::number(playerWin);
        qInfo() << "RemainingEnemies: " + QString::number(remainingEnemies);
        qInfo() << "IsPlaying: " + QString::number(isPlaying);

        QThread::msleep(100);
    }

    return a.exec();
}


void ConnectDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={SQL SERVER};SERVER=Alessandro\\SQLEXPRESS;DATABASE=Users;Trusted_Connection=Yes;");

    if (db.open()) {
        int dbKills = 0, dbDeaths = 0, dbAssistances = 0, dbXP = 0;

        QSqlQuery selectQuery;
        selectQuery.prepare("SELECT Kills, Assistances, Deaths, XP FROM Users WHERE UplayID = :uplayId");
        selectQuery.bindValue(":uplayId", QString::fromStdString(uplayId));

        if (selectQuery.exec() && selectQuery.next()) {
            dbKills = selectQuery.value(0).toInt();
            dbAssistances = selectQuery.value(1).toInt();
            dbDeaths = selectQuery.value(2).toInt();
            dbXP = selectQuery.value(3).toInt();
        } else {
            qWarning() << "No se encontraron filas para UplayID " << uplayId;
        }

        QSqlQuery updateQuery;

        updateQuery.prepare("UPDATE Users SET PlayerName = :playerName, Kills = :kills, Assistances = :assistances, Deaths = :deaths, XP = :xp, TimePlayed = :timePlayed WHERE UplayID = :uplayId");
        updateQuery.bindValue(":playerName", QString::fromStdString(playerName));
        updateQuery.bindValue(":uplayId", QString::fromStdString(uplayId));
        updateQuery.bindValue(":kills", kills + dbKills);
        updateQuery.bindValue(":assistances", assistances + dbAssistances);
        updateQuery.bindValue(":deaths", deaths + dbDeaths);
        updateQuery.bindValue(":xp", xp + dbXP);
        updateQuery.bindValue(":timePlayed", timePlayed);

        if (updateQuery.exec()) {
            int numRowsAffected = updateQuery.numRowsAffected();

            if (numRowsAffected == 0) {
                // Si la actualización no afecta filas, realiza una inserción en su lugar
                QSqlQuery insertQuery;
                insertQuery.prepare("INSERT INTO Users (PlayerName, UplayID, Kills, Assistances, Deaths, XP, TimePlayed) "
                                    "VALUES (:playerName, :uplayId, :kills, :assistances, :deaths, :xp, :timePlayed)");
                insertQuery.bindValue(":playerName", QString::fromStdString(playerName));
                insertQuery.bindValue(":uplayId", QString::fromStdString(uplayId));
                insertQuery.bindValue(":kills", kills);
                insertQuery.bindValue(":assistances", assistances);
                insertQuery.bindValue(":deaths", deaths);
                insertQuery.bindValue(":xp", xp);
                insertQuery.bindValue(":timePlayed", timePlayed);

                if (insertQuery.exec()) {
                    qInfo() << "Inserción exitosa";
                } else {
                    qWarning() << "Error al insertar valores en la tabla Users: " << insertQuery.lastError().text();
                }
            } else {
                qInfo() << "Actualización exitosa, se actualizaron " << numRowsAffected << " fila(s).";
            }
        } else {
            qWarning() << "Error al actualizar valores en la tabla Users: " << updateQuery.lastError().text();
        }
    } else {
        qWarning() << "Error al abrir la base de datos: " << db.lastError().text();
    }
}



