#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>






int get_local_ip(char *ips) {
        struct ifaddrs *ifAddrStruct;

        void *tmpAddrPtr=NULL;

        char ip[INET_ADDRSTRLEN];

        int n = 0;

        getifaddrs(&ifAddrStruct);

        while (ifAddrStruct != NULL) {
                if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
                        tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;

                        inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);

                        printf("%s IP Address:%s\n", ifAddrStruct->ifa_name, ip);

                        if (n == 0){
                                strncat(ips, ip, INET_ADDRSTRLEN);

                        } else {
                                strncat(ips, ",", 1);

                                strncat(ips, ip, INET_ADDRSTRLEN);

                        }

                        n++;

                }

                ifAddrStruct=ifAddrStruct->ifa_next;

        }

        //free ifaddrs

        freeifaddrs(ifAddrStruct);

        return 0;

}


int main()

{
        char ip[64];

        memset(ip, 0, sizeof(ip));

        get_local_ip(ip);

        printf("IP:%s\n", ip);

        return 0;

}