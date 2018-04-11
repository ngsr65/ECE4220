#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <time.h>

//Broadcast IP - 128.206.19.255
//chmod +x Lab5_client_WS


int main(int argc, char** argv){
	int sock, i, votes[10] = {0}, mynum, boolv = 1, ismaster = 0, s, family, msgi, gotwhois = 0, invote = 0;
	int isanother = 0, biggesti = 0, biggestn = 0;
	char mmsg[40], msg[40], host[NI_MAXHOST], theirip[2], myip[2], theirhost[NI_MAXHOST];
	struct sockaddr_in mysock, *temp, from;	
	struct ifaddrs *ifaddr, *ifa;	
	socklen_t fromlen;

	srand(time(0));

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
	sprintf(mmsg, "Nick@%s is the master", host);
	myip[0] = host[11];
	myip[1] = host[12];

	//Create the socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Socket creation error! Exiting program...\n");
		return -1;
	}

	//Set the socket parameters
	mysock.sin_family = AF_INET;
	mysock.sin_port = htons(atoi(*(argv + 1)));
	inet_aton("128.206.19.255", &mysock.sin_addr); //Broadcast IP
	
	//Bind the socket
	if (bind(sock, &mysock, sizeof(mysock)) < 0){
		printf("Error binding socket! Exiting program...\n");
		return -1;
	} 

	//Let the socket broadcast
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolv, sizeof(boolv)) < 0){
		printf("Error setting socket broadcast permission!\n");
		return -1;
	}
	
	fromlen = sizeof(struct sockaddr_in);

	//Wait for WHOIS message
	while (1){
		msgi = recvfrom(sock, msg, 40, 0, (struct sockaddr *)&from, &fromlen);
		printf("Message received: %s\n", msg);
                if (msgi < 0){
                        printf("Error reading from socket!\n");
                }

                if (strncmp(msg, "WHOIS", 5) == 0){
			if (ismaster == 1){
				inet_aton(inet_ntoa(from.sin_addr), &mysock.sin_addr);
				printf("Telling %s that I'm master\n", inet_ntoa(from.sin_addr));
				msgi = sendto(sock, mmsg, 40, 0, &mysock, fromlen);
			} else {
				gotwhois = 1;
			}	
                } else if ((strncmp(msg, "VOTE", 4) == 0) && (gotwhois == 1)){
			mynum = rand()%10 + 1;
			sprintf(msg, "# %s %d", host, mynum);
			inet_aton("128.206.19.255", &mysock.sin_addr);
			msgi = sendto(sock, msg, 40, 0, &mysock, fromlen);
			printf("Just sent \"%s\"\n", msg);
			gotwhois = 0;
			invote = 1;
		} else if ((msg[0] == '#') && (invote == 1)){
			if ((msg[16] == '1') && (msg[17] == 0)){ 
				votes[9]++;
				if (mynum == 10){
					isanother = 1;
					theirip[0] = msg[13];
					theirip[1] = msg[14];
				}
			} else {
				votes[atoi(&msg[16]) - 1]++;	
				if (mynum == atoi(&msg[16])){
					isanother = 1;
					theirip[0] = msg[13];
                                        theirip[1] = msg[14];
				}
			}
			for (i = 0; i < 10; i++){
				printf("%d - %d \t", i + 1, votes[i]);
			}
			printf("\n");
		} else {
			gotwhois = 0;
		}

		if (invote == 1){
			biggestn = 0;
			for (i = 0; i < 10; i++){
				if (biggestn <= votes[i]){
					biggestn = votes[i];
					biggesti = i + 1;
				}
			}
			if (biggesti == mynum){
				if (isanother == 0){
					ismaster = 1;
				} else {
					if (atoi(myip) > atoi(theirip)){
						ismaster = 1;
					} else {
						ismaster = 0;
					}
				}
			} else {
				ismaster = 0;
			}			
		}
	}
	

	//Free allocated memory
	freeifaddrs(ifaddr);
	freeifaddrs(ifa);

	return 0;
}







