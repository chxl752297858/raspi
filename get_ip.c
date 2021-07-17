#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

#include <ifaddrs.h>

//#define INET_ADDRSTRLEN 64



int get_local_ip(char *ip_list) {
        struct ifaddrs *ifAddrStruct;

        void *tmpAddrPtr;

        char ip[INET_ADDRSTRLEN];

        int n = 0;

        getifaddrs(&ifAddrStruct);

        while (ifAddrStruct != NULL) {
                if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
                        tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;

                        inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);

                        if (strcmp(ip, "127.0.0.1") != 0) {
//                              printf("%s IP Address:%s\n", ifAddrStruct->ifa_name, ip);

                                if (n == 0){
                                        memcpy(ip_list, ip, INET_ADDRSTRLEN);

                                } else {
                                        memcpy(ip_list+INET_ADDRSTRLEN, ip, INET_ADDRSTRLEN);

                                }

                                n++;

                        }

                }

                ifAddrStruct=ifAddrStruct->ifa_next;

        }

        //free ifaddrs

        freeifaddrs(ifAddrStruct);

        return n;

}

int main()

{
        char ip[3][INET_ADDRSTRLEN];

        memset(ip, 0, sizeof(ip));

        int n;

        for (n=get_local_ip(*ip); n>0; n--) {
                printf("IP:%s\n", ip[n-1]);

        }

        return 0;

}