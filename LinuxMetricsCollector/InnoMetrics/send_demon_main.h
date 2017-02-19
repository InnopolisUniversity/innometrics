//
// Created by alon on 15.01.17.
//
#include <stdio.h>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <sstream>

enum
{
    COL_ID = 0,
    COL_FOCUSED_WINDOW,
    COL_CLIENT_WINDOW,
    COL_WINDOW_TITLE,
    COL_RESOURCE_WINDOW,
    COL_RESOURCE_CLASS,
    COL_PATH,
    COL_PID,
    COL_CONNECT_TIME,
    COL_DISCONNECT_TIME,
    COL_SENT,
    NUM_COLS
} ;


void put_value(std::stringstream &sstream, const char *name, const char *type, const char *value){
    sstream << "{ ";
    sstream << "\"name\":\"" << name <<"\", ";
    sstream << "\"type\":\"" << type <<"\", ";
    if(strlen(value)==0)
        sstream << "\"value\":\"-\"";
    else
        sstream << "\"value\":\"" << value <<"\"";
    sstream << " }";
}

template <typename T>
void put_value(std::stringstream &sstream, const char *name, const char *type, const T value){
    sstream << "{ ";
    sstream << "\"name\":\"" << name <<"\", ";
    sstream << "\"type\":\"" << type <<"\", ";
    sstream << "\"value\":\"" << value <<"\"";
    sstream << " }";
}

void send_data(){
    static int rc = sqlite3_open(SQLITE_PATH, &db);

    sqlite3_stmt *stmt;

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    static int max = 10;
    rc = sqlite3_prepare_v2(db, DbQueries::SelectNonSentMetrics().c_str(), -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, max);
    std::vector<int> ids = std::vector<int>();

    int count = 0;

    std::stringstream sstream;
    sstream << "{ \"activities\":[ { \"name\":\"Linux tool\",\"measurements\":[";

    while (sqlite3_step(stmt) == SQLITE_ROW){
        if(count>0)
            sstream << ", ";

        ids.push_back(sqlite3_column_int(stmt, COL_ID));

        put_value(sstream, "focused window", "uint", static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_FOCUSED_WINDOW)));
        sstream << ", ";
        put_value(sstream, "client window", "uint", static_cast<uint64_t>(sqlite3_column_int64(stmt, COL_CLIENT_WINDOW)));
        sstream << ", ";
        put_value(sstream, "window title", "string", reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_WINDOW_TITLE)));
        sstream << ", ";
        put_value(sstream, "resource window", "string", reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_RESOURCE_WINDOW)));
        sstream << ", ";
        put_value(sstream, "resource class", "string", reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_RESOURCE_CLASS)));
        sstream << ", ";
        put_value(sstream, "path", "string", reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_PATH)));
        sstream << ", ";
        put_value(sstream, "pid", "uint", sqlite3_column_int(stmt, COL_PID));
        sstream << ", ";
        put_value(sstream, "connect time", "long", sqlite3_column_int64(stmt, COL_CONNECT_TIME));
        sstream << ", ";
        put_value(sstream, "disconnect time", "long", sqlite3_column_int64(stmt, COL_DISCONNECT_TIME));

        count++;
    }

    sstream << "] } ] }"; //1:<]> - measurements list, 2:<}> - activities row, 3:<]> activities list, 4:<}> - json close

    sqlite3_finalize(stmt);
    if(count==0)
        return;

    string data = sstream.str();

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");

        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, "https://aqueous-escarpment-80312.herokuapp.com/activities/");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        const char* tmp = data.c_str();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(tmp));
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tmp);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        long http_code = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        printf("\nCODE: %li\n", http_code);


        if(http_code==201){

            std::stringstream sstreamUpdate;
            sstreamUpdate << DbQueries::UpdateToSent().c_str();
            std::vector<int>::iterator it = ids.begin();
            sstreamUpdate << "Id = " << *it;
            for (it++; it != ids.end(); ++it){
                sstreamUpdate << " OR ";
                sstreamUpdate << "Id = " << *it;
            }
            sstreamUpdate << ";";
            data = sstreamUpdate.str();
            rc = sqlite3_prepare_v2(db, data.c_str(), -1, &stmt, 0);
            sqlite3_step(stmt);

            sqlite3_finalize(stmt);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}


int send_demon_main(){//send_demon_main

    bool exit = false;

    while(!exit){
        send_data();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60 * 5));
    }
    curl_global_cleanup();
    return 0;
}