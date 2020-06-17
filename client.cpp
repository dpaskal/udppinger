#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <chrono>		// for timer
#include <iomanip> 		// for setprecision


#define PORT		12000
#define MAX_MSGS	10

int main() {
	int sockfd, n;
	socklen_t len;
	char buffer[1024] = "Hello World";
	struct sockaddr_in servaddr;
	struct timeval tv;
	tv.tv_sec = 1;	// set timeout delay to 1 second and 0 microsends
	tv.tv_usec = 0;

	// Create a UDP socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));

	// Fill server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY; // localhost
	servaddr.sin_port = htons(PORT); // port number
	
	// Set timeout socket option
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
		(char *)&tv, sizeof(struct timeval)) < 0) {
		std::cout << "setsockopt failed: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<MAX_MSGS; i++) {
		auto start = std::chrono::high_resolution_clock::now(); // timer start

		// Send a packet to server
		n = sendto(sockfd, (char *) buffer, sizeof(buffer),
			MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr));
		if (n<0){	// check for error
			std::cout << "sendto failed: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

		// Receive return packet from server
		n = recvfrom(sockfd, (char *)buffer, sizeof(buffer),
			MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
		buffer[n] = '\0';

		// End and calculate elapsed time
		auto end = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
		duration *= 1e-9; // move nano seconds to seconds

		// Report 
		std::cout << "RTT_" << i+1 << " = " << duration << std::setprecision(9) << "s" << std::endl;

	}
	close(sockfd);
	return 0;
}
