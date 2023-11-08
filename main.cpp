#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QThread>
#include <QtSql>
#include <QSqlDatabase>
#include <QFile>
#include <QTextStream>
#include <Windows.h>
#include <iostream>

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

void ConnectDatabase();

int GetAllBaseAddress();

int id = 0;
QString playerName = "Alessandro";
QString uplayId = "test24";
int kills = 0;
int assistances = 0;
int deaths = 0;
int xp = 0;
int mmr = 0;
int timePlayed = 0;
int remainingEnemies = 0;
int playerWin = 0;
int isPlaying = 0;
int isPlayingBuffer = 0;
const char* GetText();

QString GetPath();
void GetUserLocalInfo(QString path);

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

    GetUserLocalInfo(GetPath() + "\\CODEX.ini");

    GetAllBaseAddress();

    while(true)
    {
        bool onMatch = OnMatch();
        bool onSituation = OnSituation();
        bool onMainMenu = OnMainMenu();
        xp = PlayerXP();
        deaths = PlayerDeaths();
        kills = PlayerKills();
        remainingEnemies = RemainingEnemies();
        playerWin = PlayerWin();
        isPlaying = IsPlaying();

        if(isPlaying != isPlayingBuffer && onSituation == 0)
        {
            ConnectDatabase();
            isPlayingBuffer = isPlaying;
        }

        QThread::msleep(100);
    }

    return a.exec();
}

void GetUserLocalInfo(QString path){
    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream s(&f);

        QString text = f.readAll();

        QRegularExpression regexUserName("UserName=(\\w+)");
        QRegularExpression regexAccountId("AccountId=([\\w-]+)");

        QRegularExpressionMatch matchUserName = regexUserName.match(text);
        QRegularExpressionMatch matchAccountId = regexAccountId.match(text);

        if (matchUserName.hasMatch() && matchAccountId.hasMatch()) {
            playerName = matchUserName.captured(1);
            uplayId = matchAccountId.captured(1);
        } else {
            qDebug() << "No se encontraron UserName o AccountId en el texto.";
        }
        f.close();
    } else {
        qDebug() << "Error al abrir el archivo: " << f.errorString();
    }
}



void ConnectDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={SQL SERVER};SERVER=Alessandro\\SQLEXPRESS;DATABASE=Users;Trusted_Connection=Yes;");

    if (db.open()) {
        int dbKills = 0, dbDeaths = 0, dbAssistances = 0, dbXP = 0, dbWins = 0, dbDefeats = 0, dbMMR = 0, dbTimePlayed = 0;

        QSqlQuery selectQuery;
        selectQuery.prepare("SELECT Kills, Assistances, Deaths, XP, Wins, Defeats, MMR, TimePlayed FROM Users WHERE UplayID = :uplayId AND PlayerName = :playerName");
        selectQuery.bindValue(":uplayId", uplayId);
        selectQuery.bindValue(":playerName", playerName);

        if (selectQuery.exec() && selectQuery.next()) {
            dbKills = selectQuery.value(0).toInt();
            dbAssistances = selectQuery.value(1).toInt();
            dbDeaths = selectQuery.value(2).toInt();
            dbXP = selectQuery.value(3).toInt();
            dbWins = selectQuery.value(4).toInt();
            dbDefeats = selectQuery.value(5).toInt();
            dbMMR = selectQuery.value(6).toInt();
            dbTimePlayed = selectQuery.value(7).toInt();
        } else {
            qWarning() << "No se encontraron filas para UplayID " << uplayId;
        }

        int win = 0;
        int lose = 0;

        if(playerWin == 1)
            win = 1;
        else
            lose = 1;

        QSqlQuery updateQuery;

        updateQuery.prepare("UPDATE Users SET Kills = :kills, Assistances = :assistances, Deaths = :deaths, XP = :xp, Wins = :wins, Defeats = :defeats, MMR = :mmr, TimePlayed = :timePlayed WHERE UplayID = :uplayId AND PlayerName = :playerName");
        updateQuery.bindValue(":playerName", playerName);
        updateQuery.bindValue(":uplayId", uplayId);
        updateQuery.bindValue(":kills", kills + dbKills);
        updateQuery.bindValue(":assistances", assistances + dbAssistances);
        updateQuery.bindValue(":deaths", deaths + dbDeaths);
        updateQuery.bindValue(":xp", xp + dbXP);
        updateQuery.bindValue(":wins", win + dbWins);
        updateQuery.bindValue(":defeats", lose + dbDefeats);
        updateQuery.bindValue(":mmr", mmr + dbMMR);
        updateQuery.bindValue(":timePlayed", timePlayed + dbTimePlayed);

        if (updateQuery.exec()) {
            int numRowsAffected = updateQuery.numRowsAffected();

            if (numRowsAffected == 0) {
                // Si la actualización no afecta filas, realiza una inserción en su lugar
                QSqlQuery insertQuery;
                insertQuery.prepare("INSERT INTO Users (PlayerName, UplayID, Kills, Assistances, Deaths, XP, Wins, Defeats, MMR, TimePlayed) "
                                    "VALUES (:playerName, :uplayId, :kills, :assistances, :deaths, :xp, :wins, :defeats, :mmr, :timePlayed)");
                insertQuery.bindValue(":playerName", playerName);
                insertQuery.bindValue(":uplayId", uplayId);
                insertQuery.bindValue(":kills", kills);
                insertQuery.bindValue(":assistances", assistances);
                insertQuery.bindValue(":deaths", deaths);
                insertQuery.bindValue(":xp", xp);
                insertQuery.bindValue(":wins", win);
                insertQuery.bindValue(":defeats", lose);
                insertQuery.bindValue(":mmr", mmr);
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



