#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 12000

int main() {
	int sockfd, n;
	// socklen_t len;
	char buffer[1024] = "Hello World";
	struct sockaddr_in servaddr;

	// Create a UDP socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));

	// Fill server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY; // localhost
	servaddr.sin_port = htons(PORT); // port number

	// Send a packet to server
	n = sendto(sockfd, (char *) buffer, sizeof(buffer),
		MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (n<0){
		std::cout << "sendto failed: " << strerror(errno) << std::endl;
		exit(0);
	}
	std::cout << n << " bytes sent." << std::endl;

	close(sockfd);
	return 0;
}

// void send(int sockfd, struct sockaddr_in servaddr) {
// 	return sendto(sockfd, (char *) buffer, sizeof(buffer),
// 		MSG_CONFIRM, (struct sockaddr *) &servaddr, len);
// }