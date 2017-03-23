//
// Created by alon on 04.03.17.
//

#ifndef INNOMETRICS_UPDATER_DEMON_MAIN_H
#define INNOMETRICS_UPDATER_DEMON_MAIN_H
#include <stdio.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

#include "Helpers/Helper.h"
#include "Helpers/HelpFunctions.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define VERSION "1.0.0.0"

bool check_version(std::string url){
    std::string totalUrl = url + "GetAvaliableVersionInfo";

    CURL *curl;
    CURLcode res;
    /* get a curl handle */
    curl = curl_easy_init();

    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, totalUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("\nCODE: %li\n", http_code);

    bool needUpdate = false;

    if(http_code==200){
        std::string versionStr = "Version:";
        std::string dateStr = ", Date:";
//        std::cout << readBuffer;
        std::size_t begin = readBuffer.find(versionStr);

        if(begin!=std::string::npos){
            begin += 10;
            std::size_t date = readBuffer.find(dateStr, begin);

            if(date!=std::string::npos){
                std::string version = readBuffer.substr(begin, date - begin - 2);
                Helper::trim(version);
                std::cout << version << "\n";
                if(version.compare(VERSION) != 0){
                    needUpdate = true;
                }
            }
        }
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    return needUpdate;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

static const char* file_path = "/tmp/InnoMetrics.Update.run";
bool download_update(std::string url){
    std::string totalUrl = url + "GetAvaliableVersion";

    CURL *curl;
    FILE *fp;
    CURLcode res;
//    char outfilename[FILENAME_MAX] = "/tmp/InnoMetrics.Update.run";
    curl = curl_easy_init();

    bool success = false;
    if (curl) {
        fp = fopen(file_path,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, totalUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return false;
        }

        long http_code = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        printf("\nCODE: %li\n", http_code);

        if(http_code==200){
            chmod(file_path, S_IRWXU);
            success = true;
        } else{
            if( remove(file_path) != 0 )
                perror( "Error deleting file" );
            else
                puts( "File successfully deleted, since there are bad http response" );
            success = false;
        }


        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    return success;
}

static void daemonize(void)
{
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

int update(std::string url){
    daemonize();
//    int b = system("gnome-terminal -e \"bash -c 'sudo ls';bash\"");
    bool success = download_update(url);

    if(!success)
        return 2;

    int a = system("gnome-terminal -e \"bash -c 'sudo /tmp/InnoMetrics.Update.run';bash\"");
    return 0;
}
#endif //INNOMETRICS_UPDATER_DEMON_MAIN_H
