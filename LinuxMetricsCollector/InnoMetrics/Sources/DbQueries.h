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
                "Pid            INT, " \
                "ConnectTime    BIGINT, " \
                "DisconnectTime BIGINT, " \
                "Sent BOOLEAN, " \
                "Filtered BOOLEAN" \
                ");";
    }

    static std::string CreateTimeFilterTable() {
        return
                "CREATE TABLE IF NOT EXISTS TimeFilter("  \
                "Id             INTEGER     PRIMARY KEY AUTOINCREMENT, " \
                "Title    NVARCHAR(150)" \
                ");";
    }

    static std::string InsertMetric(WindowInfo &info) {
        return Helper::string_format(
                "INSERT INTO Metrics (FocusedWindow,ClientWindow,WindowTitle,ResourceName,ResourceClass,Path,Pid,ConnectTime,DisconnectTime,Sent) "  \
         "VALUES (%d, %d, \"%s\", \"%s\", \"%s\", \"%s\", %d, %d, %d, %d); ",
                info.FocusedWindow, info.ClientWindow, info.WindowTitle, info.ResourceName, info.ResourceClass,
                info.Path, info.Pid, info.ConnectTime, info.DisconnectTime, false);

    }

    static std::string InsertMetric() {
        return  "INSERT INTO Metrics (FocusedWindow,ClientWindow,WindowTitle,ResourceName,ResourceClass,Path,Pid,ConnectTime,DisconnectTime,Sent) "  \
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 0); ";

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
        return "SELECT * FROM Metrics WHERE Sent=0 LIMIT ?;";
    }


    static std::string FilterMetrics() {
        return  "UPDATE Metrics " \
                "SET Filtered = 1 " \
                "WHERE ";

    }

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

private:
    DbQueries() {}
};

#endif //INNOMETRICS2_DBQUERIES_H
