//
// Created by alon on 25.10.16.
//

#ifndef INNOMETRICS2_BROWSERFILE_H
#define INNOMETRICS2_BROWSERFILE_H

#include "Helpers/Helper.h"

#include <fstream>
#include <string>
#include <locale>
#include <vector>
#include <algorithm>
#include <codecvt>
#include <regex>

#include <sqlite3.h>


class BrowserFile {
public:
    static std::string HomePath;

    //region < Chrome >

    static std::vector<unsigned char> UtfStringToUtf16Vector(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffffUL, std::little_endian>, char16_t> convert;
        //       std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;

        std::u16string u16 = convert.from_bytes(str);

        unsigned char const *p = reinterpret_cast<unsigned char const *>(&u16[0]);
        std::size_t size = u16.size() * sizeof(u16.front());
        std::vector<unsigned char> vector(p, p + size);
        return vector;
    }

    static std::vector<unsigned char> PrepareChromeTitle(const std::string &title) {
        std::string titleCopy(title.begin(), title.end() - 16);
        return UtfStringToUtf16Vector(titleCopy);
    }

    static bool ReadFileToVector(std::vector<unsigned char> &fileContent, const std::string &pathToData) {
        std::ifstream file;
        file.open(pathToData);
        file.seekg(0, std::ios::end);
        std::streampos length(file.tellg());

        if (length) {
            file.seekg(0, std::ios::beg);
            fileContent.resize(static_cast<std::size_t>(length));
            file.read((char *) &fileContent.front(), static_cast<std::size_t>(length));
            return true;
        }
        return false;
    }

    static bool ReadFileToVector(std::vector<char> &fileContent, const std::string &pathToData) {
        std::ifstream file;
        file.open(pathToData);
        file.seekg(0, std::ios::end);
        std::streampos length(file.tellg());

        if (length) {
            file.seekg(0, std::ios::beg);
            fileContent.resize(static_cast<std::size_t>(length));
            file.read((char *) &fileContent.front(), static_cast<std::size_t>(length));
            return true;
        }
        return false;
    }

    static std::vector<unsigned char> GetHttpVector() {
        static std::string s("http");
        static std::vector<unsigned char> http(s.begin(), s.end());
        return http;
    }

    static std::vector<unsigned char>::const_iterator FindTitleIndex(const std::vector<unsigned char> &fileContent,
                                                                     const std::vector<unsigned char> &title,
                                                                     std::vector<unsigned char>::const_iterator start) {

        std::vector<unsigned char>::const_iterator titleIterator = std::search(start, fileContent.end(),
                                                                               title.begin(), title.end());
        if (titleIterator != fileContent.end()) {
            std::vector<unsigned char>::const_reverse_iterator rend(start);
            std::vector<unsigned char>::const_reverse_iterator rTitleIterator(titleIterator);

            static std::vector<unsigned char> http = GetHttpVector();

            auto urlStart = std::search(rTitleIterator, rend, http.rbegin(), http.rend());

            if (urlStart != fileContent.rend()) {
                std::vector<unsigned char> vLength;

                //Determining is size enough for char or should use short
                if (title.size() / 256) {
                    unsigned short titleSize = (unsigned short) title.size();
                    char *pTitleSize = reinterpret_cast<char *>(&titleSize);
                    std::string length(pTitleSize, sizeof(titleSize));
                    vLength = std::vector<unsigned char>(length.begin(), length.end());
                } else {
                    char titleSize = static_cast<char>(title.size());
                    std::string length(&titleSize, sizeof(char));
                    vLength = std::vector<unsigned char>(length.begin(), length.end());
                }
                // Searching for string with amount of bytes if there are not then go search further
                auto lengthIterator = std::search(urlStart.base(), titleIterator, vLength.begin(), vLength.end());
                if (lengthIterator == titleIterator) { // Here is off but should be checked
                    return FindTitleIndex(fileContent, title, titleIterator + title.size());
                }
                return titleIterator;
            }
            return fileContent.begin();
        }
        return fileContent.begin();
    }

    static void
    CutChromeUrl(std::vector<unsigned char>::const_iterator titleIterator,
                 const std::vector<unsigned char> &fileContent,
                 std::string &url) {
        if (titleIterator != fileContent.begin()) {
            std::vector<unsigned char>::const_reverse_iterator rEnd(fileContent.begin());
            std::vector<unsigned char>::const_reverse_iterator rTitleIterator(titleIterator);

            static std::vector<unsigned char> http = GetHttpVector();

            auto urlStart = std::search(rTitleIterator, rEnd, http.rbegin(), http.rend());
            auto urlEnd = std::find(urlStart.base(), titleIterator, (unsigned char) '\0');
            url = std::string(urlStart.base() - 4, urlEnd);
            return;
        } else {
            url = std::string();
        }
    }

    static void GetChromeUrl(const std::string &title, std::string &url) {

        if (title == "New Tab - Google Chrome") {
            return;
        }

        std::vector<unsigned char> vFileContent;

        static std::string pathToData = HomePath + "/.config/google-chrome/Default" + "/Current Session";

        if (!ReadFileToVector(vFileContent, pathToData)) {
            fprintf(stderr, "Can't open the Chrome file.");
            return;
        }


        std::vector<unsigned char> vTitle = PrepareChromeTitle(title);

//        printf("%lu, %lu\n", title.size()-16, vTitle.size());

        auto titleIterator = FindTitleIndex(vFileContent, vTitle, vFileContent.begin());

        CutChromeUrl(titleIterator, vFileContent, url);
//        printf("%s\n", url.c_str());
    }

    //endregion

    //region < Firefox >

    static std::string GetFirefoxFolderPath() {
        std::string firefoxPath = HomePath + "/.mozilla/firefox/";
        std::vector<char> vProfileFileContent;

        if (!ReadFileToVector(vProfileFileContent, firefoxPath + "profiles.ini")) {
            fprintf(stderr, "Can't open the Firefox file.");
            return std::string();
        }

        std::string sPath = "Path";

        auto pathBegin = std::search(vProfileFileContent.begin(), vProfileFileContent.end(), sPath.begin(),
                                     sPath.end());

        auto pathEnd = std::find(pathBegin, vProfileFileContent.end(), '\n');

        return firefoxPath + std::string(pathBegin + 5, pathEnd);
    };

    static std::string GetPath() {
        return GetFirefoxFolderPath() + "/places.sqlite";
    }

    static void PrepareFirefoxSqlite(std::string path, sqlite3 **db, bool &good) {
        int rc = sqlite3_open_v2(path.c_str(), db, SQLITE_OPEN_READONLY, nullptr);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
            sqlite3_close(*db);
            good = false;
        } else
            good = true;
    }

    static void GetUrl(const std::string &title, std::string &url) {
        static auto sPath = GetPath();
        static sqlite3 *db;
        static bool dbGood = false;

        if (!dbGood) {
            PrepareFirefoxSqlite(sPath, &db, dbGood);
            if (!dbGood) {
                url = std::string();
                return;
            }
        }

        if(title.size()<19)
        {
            url = std::string();
            return;
        }

        auto sTitle = std::string(title.begin(), title.end() - 18);
        auto pTitle = sTitle.c_str();

        static auto sqlQuery = "SELECT url FROM moz_places WHERE title = ? ORDER BY last_visit_date DESC";

        int rc;
        sqlite3_stmt *res;
        rc = sqlite3_prepare_v2(db, sqlQuery, -1, &res, 0);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
//            sqlite3_close(db);
            url = std::string();
            return;
        }

        sqlite3_bind_text(res, 1, pTitle, strlen(pTitle), SQLITE_STATIC);

        rc = sqlite3_step(res);

        if (rc == SQLITE_ROW) {
//            printf("%s\n", sqlite3_column_text(res, 0));
            url = std::string(reinterpret_cast<const char*>(sqlite3_column_text(res, 0)));
        }
        if(res)
            sqlite3_finalize(res);
//        sqlite3_close(db);
    }

    //endregion
};


#endif //INNOMETRICS2_BROWSERFILE_H
