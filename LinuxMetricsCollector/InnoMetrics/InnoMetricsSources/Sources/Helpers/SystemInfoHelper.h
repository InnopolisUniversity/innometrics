//
// Created by alon on 29.10.16.
//

#ifndef INNOMETRICS2_SYSTEMINFOHELPER_H
#define INNOMETRICS2_SYSTEMINFOHELPER_H

#include <unistd.h>     // close, gethostname
#include <vector>


#include <net/if.h>     // if_freenameindex, constants
#include <sys/ioctl.h>  // ioctl,SIOCGIFADDR
#include <arpa/inet.h>  // inet_ntoa
#include <linux/sockios.h>
//#include <netinet/in.h>

#include <string.h>     // memmove, bzero, strncpy
#include <string>

#include "Helper.h"     // string_format

struct NetInfo {
public:
    NetInfo() { }

//    NetInfo(std::string name) { Name = name; }
//
//    NetInfo(std::string ip) { Ip = ip; }
//
//    NetInfo(std::string mac) { Mac = mac; }

    NetInfo(std::string name, std::string ip, std::string mac) {
        Mac = mac;
        Ip = ip;
        Name = name;
    }

    std::string Mac;
    std::string Ip;
    std::string Name;
};

struct SystemInfo {
    std::vector<NetInfo> NetInterfaces;
    std::string UserName;
    std::string HostName;
};

class SystemInfoHelper {
public:
    // Filling vector by NetInfos structures of current machine
    static int GetNetInfos(std::vector<NetInfo> &netInfos);

    // Get user name and host name
    static void GetUserHostName(SystemInfo &info);

    //Filling structure by information of machine
    static void GetSystemInfo(SystemInfo &info);
};

#endif //INNOMETRICS2_SYSTEMINFOHELPER_H
