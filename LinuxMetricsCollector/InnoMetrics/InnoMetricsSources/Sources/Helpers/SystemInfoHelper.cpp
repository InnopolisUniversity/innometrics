//
// Created by alon on 29.10.16.
//
#include "SystemInfoHelper.h"


#include <iostream>
#include <ifaddrs.h>
#include <netdb.h>
#include <map>


int SystemInfoHelper::GetNetInfos(std::vector<NetInfo> &netInfos) {
    /// get interface addresses
    struct ifaddrs* interface_addrs  = NULL;
    if( getifaddrs( &interface_addrs  ) == -1 )
    {
        return 0;
    }

    if( !interface_addrs )
    {
        return 0;
    }

    int32_t sd = socket( PF_INET, SOCK_DGRAM, 0 );
    if( sd < 0 )
    {
        /// free memory allocated by getifaddrs
        freeifaddrs( interface_addrs );
        return 0;
    }

    std::map<std::string, NetInfo> my_map = {    };

    /// get MAC address for each interface
    for( struct ifaddrs* ifa = interface_addrs; ifa != NULL; ifa = ifa->ifa_next )
    {
        /// print MAC address
        if( ifa->ifa_data != 0 )
        {
            struct ifreq req;
            strcpy( req.ifr_name, ifa->ifa_name );
            if( ioctl( sd, SIOCGIFHWADDR, &req ) != -1 )
            {
                uint8_t* mac = (uint8_t*)req.ifr_ifru.ifru_hwaddr.sa_data;
                char macstr[50];
                std::sprintf(macstr,"%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

                if(my_map.count( ifa->ifa_name )<1){
                    my_map[ifa->ifa_name] = NetInfo();
                    my_map[ifa->ifa_name].Name = std::string(ifa->ifa_name);
                    my_map[ifa->ifa_name].Mac = std::string(macstr);
                }
                else{
                    my_map[ifa->ifa_name].Mac = std::string(macstr);
                }
//                printf( "%s:MAC[%02X:%02X:%02X:%02X:%02X:%02X]\n",
//                        ifa->ifa_name,
//                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
            }
        }

        /// print IP address
        if( ifa->ifa_addr != 0 )
        {
            int family = ifa->ifa_addr->sa_family;
            if( family == AF_INET || family == AF_INET6 )
            {
                char host[NI_MAXHOST];
                if( getnameinfo( ifa->ifa_addr,
                                 (family == AF_INET)? sizeof( struct sockaddr_in ) : sizeof( struct sockaddr_in6 ),
                                 host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST ) == 0 )
                {
                    char address[70];
                    std::sprintf(address,"Family:[%d%s]:IP[%s]",
                                 family,
                                 (family == AF_PACKET) ? " (AF_PACKET)" :
                                 (family == AF_INET) ?   " (AF_INET)" :
                                 (family == AF_INET6) ?  " (AF_INET6)" : "",
                                 host );

                    if(my_map.count( ifa->ifa_name )<1){
                        my_map[ifa->ifa_name] = NetInfo();
                        my_map[ifa->ifa_name].Name = std::string(ifa->ifa_name);
                        my_map[ifa->ifa_name].Ip = std::string(address);
                    }
                    else{
                        my_map[ifa->ifa_name].Ip = std::string(address);
                    }

//                    printf( "%s:Address Family:[%d%s]:IP[%s]\n",
//                            ifa->ifa_name,
//                            family,
//                            (family == AF_PACKET) ? " (AF_PACKET)" :
//                            (family == AF_INET) ?   " (AF_INET)" :
//                            (family == AF_INET6) ?  " (AF_INET6)" : "",
//                            host );
                }
            }
        }
    }

    /// close socket
    close( sd );

    /// free memory allocated by getifaddrs
    freeifaddrs( interface_addrs );

    for (std::map<std::string, NetInfo>::iterator it = my_map.begin(); it != my_map.end(); ++it)
    {
        netInfos.push_back(it->second);
        //printf("%s, %s, %s \n", it->second.Name, it->second.Ip, it->second.Mac);
    }

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
