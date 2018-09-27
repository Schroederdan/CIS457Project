#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cmath>

int main(int argc, char** argv) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cout << "There was a problem creating the socket\n";
        return 1;
    }

    char ipAddress[5000];
    int port;
    std::cout << "Enter an IP address: ";
    std::cin >> ipAddress;

    std::cout << "Enter a Port number: ";
    std::cin >> port;

	if(port < 0 || port > 65535){
		std::cout << "Bad Port Number\n";
		exit(1);
	}

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr= inet_addr(ipAddress);

	if(!inet_pton(AF_INET,ipAddress,&serveraddr.sin_addr)){
		std::cout << "Bad Address\n";
		exit(1);
	}

    char line[5000];
    char fout[5000];
    //std::string input;
    std::cout << "Enter a file to copy: ";
    std::cin.ignore();
    std::cin.getline(line,5000);
    std::cout << "Enter a file to write to: ";
    std::cin.getline(fout,5000);


    std::FILE* myfile = std::fopen(fout,"w");
    if(myfile == NULL){
        return 1;
    }

    char line2[1025];
    int len = sizeof(serveraddr);
    sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    // recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr,(socklen_t*)&len);

    int packets = 1;
    int recieve = 1024;
    char pcount[100];
    int pos;
    std::size_t fw;

    int nPacket = 0;
    char wait1[1025];
    char wait2[1025];
    char wait3[1025];
    char wait4[1025];

    while(1){

        recieve = recvfrom(sockfd,line2,1025,0,(struct sockaddr*)&serveraddr,(socklen_t*)&len);
        pcount[0] = line2[recieve - 1];
        std::cout << "Packet Number: " << pcount[0] << "\n";
        if (strcmp(line2,"EOF") == 0) {
            std::cout << "End of file reached.\n";
            break;
        }
        if (strcmp(line2,"Error") == 0) {
            std::cout << "File does not exist.\n";
            break;
        }
        if(recieve != -1){
            //	pos = strlen(line2) - 1;
            // if(recieve != 1024){
            //     line2[recieve] = EOF;
            // }
            //std::cout << recieve-1 << "\n";

		if(atoi(pcount) == nPacket){
            		fw = fwrite(line2,sizeof(char),recieve-1,myfile);
			nPacket++;
			if(nPacket == 10){
				nPacket = 0;
			}
		}else if(std::abs(atoi(pcount)-nPacket) == 1 || std::abs(atoi(pcount)-(nPacket+10))==1){
			for(int i = 0;i<recieve;i++){		
				wait1[i] = pcount[i];
			}
	    	}else if(std::abs(atoi(pcount)-nPacket) == 2 || std::abs(atoi(pcount)-(nPacket+10))==2){
			for(int i = 0;i<recieve;i++){		
				wait2[i] = pcount[i];
	    		}
	    	}else if(std::abs(atoi(pcount)-nPacket) == 3 || std::abs(atoi(pcount)-(nPacket+10))==3){
			for(int i = 0;i<recieve;i++){		
				wait3[i] = pcount[i];
			}
	    	}else if(std::abs(atoi(pcount)-nPacket) == 4 || std::abs(atoi(pcount)-(nPacket+10))==4){
			for(int i = 0;i<recieve;i++){
				wait4[i] = pcount[i];
			}
	    	}

		if(pcount[0] == nPacket){
			while(wait1[0] != ' '){
				fw = fwrite(wait1,sizeof(char),recieve-1,myfile);
				for(int i = 0;i<1025;i++){
					wait1[i] = wait2[i];
					wait2[i] = wait3[i];
					wait3[i] = wait4[i];
				}
				wait4[0] = ' ';

				nPacket++;
				if(nPacket == 10){
					nPacket = 0;
				}
			}
	    	}	
        }

        std::cout << "Packets recieved: " << packets << "\n";
        packets++;
        // want to send the number of the packet recieved back to the server
        //		pcount = string(itoa(packets));
        // send a message to server saying the the packet was recieved
        sendto(sockfd,pcount,strlen(pcount)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    }

    fclose(myfile);

    close(sockfd);

    return 0;
}
