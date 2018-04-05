#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>

//Broadcast IP - 128.206.19.255

int main(int argc, char** argv){
	int sock, i, votes[10] = {0}, mynum, boolv = 1, ismaster = 0, s, family;
	char msg[40], host[NI_MAXHOST];
	struct sockaddr_in mysock, *temp;	
	struct ifaddrs *ifaddr, *ifa;	

	if (argc != 2){
		printf("\nError! \"./a.out [port]\"\n");
		return 1;
	}



	//Get my network information
	getifaddrs(&ifaddr);
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
        	if (ifa->ifa_addr == NULL){
            		continue;
		}
        	s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    	}


	printf("My IP is %s\n", host);

	//Create the socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Socket creation error! Exiting program...\n");
		return -1;
	}

	//Set the socket parameters
	mysock.sin_family = AF_INET;
	mysock.sin_port = htons(atoi(*(argv + 1)));
	inet_aton(host, &mysock.sin_addr);

	//Let the socket broadcast
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolv, sizeof(boolv)) < 0){
		printf("Error setting socket broadcast permission!\n");
		return -1;
	}
	



	//Free allocated memory
	freeifaddrs(ifaddr);
	freeifaddrs(ifa);

	return 0;
}




