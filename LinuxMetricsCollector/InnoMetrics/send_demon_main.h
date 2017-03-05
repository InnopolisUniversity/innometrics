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


sqlite3 *dbSendDemon;

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

static std::vector<string> filterString = std::vector<string>();

void prepare_filter_string(int rc){
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(dbSendDemon, DbQueries::SelectTitleFilter().c_str(), -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW){
        filterString.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
}

std::vector<string> get_filters(){
    std::vector<string> filters = std::vector<string>();

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(dbSendDemon, DbQueries::SelectOnlyTitleFilter().c_str(), -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW){
        filters.push_back(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
    }

    return filters;
}

string getFilterString(){
    static std::vector<string> filters = get_filters();

    if(filters.size()<1)
        return "";
    std::stringstream sstreamUpdate;
    sstreamUpdate << DbQueries::FilterMetrics().c_str();
    std::vector<string>::iterator it = filters.begin();
    sstreamUpdate << "WindowTitle LIKE '%" << *it << "%' OR ResourceName LIKE '%" << *it << "%' OR ResourceClass LIKE '%" << *it;
    for (it++; it != filters.end(); ++it){
        sstreamUpdate << "%' OR ";
        sstreamUpdate << "WindowTitle LIKE '%" << *it << "%' OR ResourceName LIKE '%" << *it << "%' OR ResourceClass LIKE '%" << *it;
    }
    sstreamUpdate << "%';";
    return sstreamUpdate.str();
}

void filter_data(){
    static string data = getFilterString();

    if(data == "")
        return;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(dbSendDemon, data.c_str(), -1, &stmt, 0);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if(rc == SQLITE_DONE){
        printf("Filtered\n");
    }
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string getToken(){
    CURL *curl;
    CURLcode res;
    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        std::string readBuffer;

        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");

        curl_easy_setopt(curl, CURLOPT_URL, "https://aqueous-escarpment-80312.herokuapp.com/api-token-auth/");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        const char* tmp = "{\"username\":\"test\", \"password\":\"test123123\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(tmp));
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tmp);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

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

        std::string token = "";
        if(http_code==200){
            std::size_t pos = readBuffer.find("{\"token\":\"");
            pos += 10;
            std::size_t pos2 = readBuffer.find("\"", pos);
            token = readBuffer.substr (pos, pos2 - pos);
//            std::cout << readBuffer.substr (pos, pos2 - pos) << std::endl;
        }
//        std::cout << http_code << " " << readBuffer << std::endl;
        /* always cleanup */
        curl_easy_cleanup(curl);

        return token;
//        curl_global_cleanup();
    }
}

int send_data(){
    static int rc = sqlite3_open(SQLITE_PATH, &dbSendDemon);
    filter_data();
    sqlite3_stmt *stmt;

    CURL *curl;
    CURLcode res;

    static int max = 10;
    rc = sqlite3_prepare_v2(dbSendDemon, DbQueries::SelectNonSentMetrics().c_str(), -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, max);
    std::vector<int> ids = std::vector<int>();

    int count = 0;

    std::stringstream sstream;
    sstream << "{ \"activities\":[ { \"name\":\"Linux tool\",\"measurements\":[";

    while (sqlite3_step(stmt) == SQLITE_ROW){
        if(count > 0)
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
        return 0;

    string data = sstream.str();

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        static std::string token = "Authorization: Token " + getToken();
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");
        headers = curl_slist_append(headers, token.c_str());

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
            rc = sqlite3_prepare_v2(dbSendDemon, data.c_str(), -1, &stmt, 0);
            sqlite3_step(stmt);

            sqlite3_finalize(stmt);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
//        curl_global_cleanup();
    }
    return count;
}


void* send_demon_main(void *arg){//void* send_demon_main

    curl_global_init(CURL_GLOBAL_ALL);

    int* pthread_state = (int *) arg;

    bool exit = false;

    while(!exit){
        int count = send_data();
        
        printf("normal\n");

        switch( *pthread_state )
        {
            case 0: // everything normal
                printf("normal\n");
                break;
            case -1:
                printf("I 'm exiting\n");
                pthread_exit(0);

                break;
            case 1: // Command for extend
                printf("command\n");
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(count > 0)
            continue;

        printf("Gone to sleep.\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60 * 5));
    }
    return 0;
}