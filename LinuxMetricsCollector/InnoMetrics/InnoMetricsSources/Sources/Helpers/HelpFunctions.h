//
// Created by alon on 11.03.17.
//

#ifndef INNOMETRICS_HELPFUNCTIONS_H
#define INNOMETRICS_HELPFUNCTIONS_H

#include <cstdio>
#include <string>
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

#endif //INNOMETRICS_HELPFUNCTIONS_H
