//
// Created by alon on 29.10.16.
//
#include "SystemInfoHelper.h"

int SystemInfoHelper::GetNetInfos(std::vector<NetInfo> &netInfos) {
    unsigned char mac[8];
    int nSD; // Socket descriptor
    struct ifreq sIfReq; // Interface request
    struct if_nameindex *pIfList;
    struct if_nameindex *pListSave;

    //pIfList = (struct if_nameindex *)NULL;
    //pListSave = (struct if_nameindex *)NULL;
#ifndef SIOCGIFADDR
    fprintf(stderr, "SystemInfoHelper: The kernel does not support the required ioctls");
    return 0;
#endif

    nSD = socket(PF_INET, SOCK_STREAM, 0);
    if (nSD < 0) {
        fprintf(stderr, "SystemInfoHelper: Cannot open socket");
        return 0;
    }

    pIfList = pListSave = if_nameindex();

    for (pIfList; *(char *) pIfList != 0; pIfList++) {
        bzero((void *) &mac[0], sizeof(mac));
        strncpy(sIfReq.ifr_name, pIfList->if_name, IF_NAMESIZE);

        // Get the sIfReq for this interface
        if (ioctl(nSD, SIOCGIFHWADDR, &sIfReq) != 0) {
            fprintf(stderr, "SystemInfoHelper: One of the interfaces is failed to obtain MAC");
            continue;
        }
        memmove((void *) &mac[0], (void *) &sIfReq.ifr_ifru.ifru_hwaddr.sa_data[0], 6);

        netInfos.push_back(NetInfo(
                std::string(sIfReq.ifr_name),
                std::string(inet_ntoa(((struct sockaddr_in *) &sIfReq.ifr_addr)->sin_addr)),
                Helper::string_format("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])
        ));
// Test
//        printf( "name: %s \tip:%s \tmac: %02X:%02X:%02X:%02X:%02X:%02X\n",
//                sIfReq.ifr_name,
//                inet_ntoa(((struct sockaddr_in *)&sIfReq.ifr_addr)->sin_addr),
//                mac[0], mac[1], mac[2],
//                mac[3], mac[4], mac[5] );
    }

    if_freenameindex(pListSave);
    close(nSD);
    return 1;
}

void SystemInfoHelper::GetUserHostName(SystemInfo& info) {
    char *pTemp = 0;
    pTemp = getenv("HOSTNAME");

    if (pTemp == 0) {
        pTemp = new char[256];
        gethostname(pTemp, 256);
    }
    info.HostName = pTemp;
    delete [] pTemp;
    info.UserName = getenv("USER");

//  Test
//    printf("%s\n", pTemp);
//    printf("%s\n", getenv("USER"));
}

void SystemInfoHelper::GetSystemInfo(SystemInfo &info) {
    GetUserHostName(info);
    GetNetInfos(info.NetInterfaces);
}
