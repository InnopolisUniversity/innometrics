//
// Created by alon on 04.11.16.
//

#ifndef INNOMETRICS2_DBQUERIES_H
#define INNOMETRICS2_DBQUERIES_H

#include <string>
#include "WindowInfo.h"
#include "Helpers/Helper.h"

#define SQLITE_PATH "$metrics.db" //HOME/innometrics/

class DbQueries {
public:

//region < Metrics >

    static std::string CreateMetricTable() {
        return
                "CREATE TABLE IF NOT EXISTS Metrics("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "FocusedWindow  UNSIGNED BIG INT         NOT NULL, " \
                "ClientWindow   UNSIGNED BIG INT         NOT NULL, " \
                "WindowTitle    NVARCHAR(150), " \
                "ResourceName   NVARCHAR(100), " \
                "ResourceClass  NVARCHAR(100), " \
                "Path           NVARCHAR(256), " \
                "Url            NVARCHAR(256), " \
                "Pid            INT, " \
                "ConnectTime    BIGINT, " \
                "DisconnectTime BIGINT, " \
                "Sent           BOOLEAN, " \
                "Filtered       BOOLEAN, " \
                "Time           INTEGER " \
                ");";
    }

    static std::string InsertMetric(WindowInfo &info) {
        return Helper::string_format(
                "INSERT INTO Metrics (FocusedWindow,ClientWindow,WindowTitle,ResourceName,ResourceClass,Path,Url,Pid,ConnectTime,DisconnectTime,Sent) "  \
         "VALUES (%d, %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %d, %d, %d, %d); ",
                info.FocusedWindow, info.ClientWindow, info.WindowTitle, info.ResourceName, info.ResourceClass,
                info.Path, info.URL, info.Pid, info.ConnectTime, info.DisconnectTime, false);

    }

    static std::string InsertMetric() {
        return  "INSERT INTO Metrics (FocusedWindow,ClientWindow,WindowTitle,ResourceName,ResourceClass,Path,Url,Pid,ConnectTime,DisconnectTime,Sent,Filtered,Time) "  \
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, 0, ?); ";

    }

    static std::string UpdateToSent() {
        return  "UPDATE Metrics " \
                "SET Sent = 1 " \
                "WHERE ";

    }

    static std::string SelectAllMetrics() {
        return "SELECT * FROM Metrics WHERE ConnectTime >= ? AND ConnectTime <=? AND (? OR Sent=0) LIMIT ?;";
    }


    static std::string SelectNonSentMetrics() {
        return "SELECT * FROM Metrics WHERE Sent = 0 AND Filtered = 0 LIMIT ?;";
    }

    static std::string DeleteAllMeasurements() {
        return "DELETE FROM Metrics;";
    }

    static std::string DeleteFilteredAndSentMeasurements() {
        return "DELETE FROM Metrics WHERE Sent=1 OR Filtered=1;";
    }

    static std::string DeleteFilteredAndSentMeasurementsByDate() {
        return "DELETE FROM Metrics WHERE (Sent=1 OR Filtered=1) AND DisconnectTime < ?;";
    }

//endregion < Metrics >


    static std::string FilterMetrics() {
        return  "UPDATE Metrics " \
                "SET Filtered = 1 " \
                "WHERE ";

    }

//region < Tokens >

    static std::string CreateTokenTable() {
        return
                "CREATE TABLE IF NOT EXISTS Tokens("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "UserName NVARCHAR(150)," \
                "Token    NVARCHAR(150)," \
                "GetTime BIGINT," \
                "Expired BOOLEAN);";
    }

    static std::string InsertToken(){
        return  "INSERT INTO Tokens (UserName, Token, GetTime, Expired) "  \
                "VALUES (?, ?, ?, 0); ";
    }

    static std::string GetLastToken(){
        return  "SELECT Token FROM Tokens WHERE Expired=0 ORDER BY GetTime DESC LIMIT 1;";
    }

    static std::string UpdateTokensFilter() {
        return "UPDATE Tokens SET Expired = 1;";
    }

//endregion < Tokens >

//region < StaticMetrics >

    static std::string CreateStaticMetricTable() {
        return
                "CREATE TABLE IF NOT EXISTS StaticMetrics("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "UserName  NVARCHAR(150), " \
                "HostName   NVARCHAR(150), " \
                "NetworkNames    NVARCHAR(150), " \
                "NetworkIps   NVARCHAR(100), " \
                "NetworkMacs  NVARCHAR(100), " \
                "Collected    BIGINT, " \
                "Sent BOOLEAN " \
                ");";
    }

    static std::string InsertStaticMetric(){
        return  "INSERT INTO StaticMetrics (UserName, HostName, NetworkNames, NetworkIps, NetworkMacs, Collected, Sent) "  \
                "VALUES (?, ?, ?, ?, ?, ?, 0); ";
    }

    static std::string GetLastStaticMetric(){
        return  "SELECT * FROM StaticMetrics ORDER BY Collected DESC LIMIT 1;";
    }

//endregion < StaticMetrics >

//region < Filter >

    static std::string CreateTitleFilterTable() {
        return
                "CREATE TABLE IF NOT EXISTS TitleFilter("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "Title    NVARCHAR(150)" \
                ");";
    }

    static std::string SelectTitleFilter() {
        return "SELECT * FROM TitleFilter;";
    }

    static std::string SelectOnlyTitleFilter() {
        return "SELECT Title FROM TitleFilter;";
    }

    static std::string InsertTitleFilter() {
        return  "INSERT INTO TitleFilter (Title) "  \
                "VALUES (?);";
    }

    static std::string UpdateTitleFilter() {
        return "UPDATE TitleFilter SET Title = ? WHERE Id = ?;";
    }

    static std::string DeleteTitleFilter() {
        return "DELETE FROM TitleFilter WHERE Id = ?;";
    }

//endregion

//region < Time filter >

    static std::string CreateTimeFilterTable() {
        return  "CREATE TABLE IF NOT EXISTS TimeFilter("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "FromTime          INTEGER, " \
                "ToTime            INTEGER " \
                ");";
    }

    static std::string SelectTimeFilter() {
        return "SELECT * FROM TimeFilter;";
    }

    static std::string SelectOnlyTimeFilter() {
        return "SELECT FromTime, ToTime FROM TimeFilter;";
    }

    static std::string InsertTimeFilter() {
        return  "INSERT INTO TimeFilter (FromTime, ToTime) "  \
                "VALUES (?, ?);";
    }

    static std::string UpdateTimeFilter() {
        return "UPDATE TimeFilter SET FromTime = ?, ToTime = ? WHERE Id = ?;";
    }

    static std::string DeleteTimeFilter() {
        return "DELETE FROM TimeFilter WHERE Id = ?;";
    }

//endregion

private:
    DbQueries() {}
};

#endif //INNOMETRICS2_DBQUERIES_H
