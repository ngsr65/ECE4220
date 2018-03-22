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

//Broadcast IP - 128.206.19.255

int main(int argc, char** argv){
	int sock, i, votes[10] = {0}, mynum, boolv = 1, ismaster = 0;
	char msg[40];
	struct sockaddr_in mysock, *temp;	
	struct ifaddrs *ipinfo;
	unsigned long myip;
	struct in_addr tempinaddr;	

	if (argc != 2){
		printf("\nError! \"./a.out [port]\"\n");
	}

	//Get my network information
	/*int main()
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    snprintf(ifr.ifr_name, IFNAMSIZ, "eth0");

    ioctl(fd, SIOCGIFADDR, &ifr);

    /* and more importantly */
    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(fd);
}
*/
	getifaddrs(&ipinfo);
	temp = (struct sockaddr_in *)(ipinfo->ifa_addr);
	myip = temp->sin_addr.s_addr;

	tempinaddr.s_addr = myip;
	printf("My IP is %s\n", inet_ntoa(tempinaddr));

	//Create the socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Socket creation error! Exiting program...\n");
		return -1;
	}

	//Set the socket parameters
	mysock.sin_family = AF_INET;
	mysock.sin_port = htons(atoi(*(argv + 1)));
	mysock.sin_addr.s_addr = myip;

	//Let the socket broadcast
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolv, sizeof(boolv)) < 0){
		printf("Error setting socket broadcast permission!\n");
		return -1;
	}
	



	//Free allocated memory
	freeifaddrs(ipinfo);
	
	return 0;
}




