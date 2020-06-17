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

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: ./client <ipv4 address>" << std::endl;
		exit(EXIT_FAILURE);
	}
	int sockfd, n;
	char buffer[1024];
	struct sockaddr_in servaddr;
	struct timeval tv;
	tv.tv_sec = 1;	// set timeout delay to 1 second and 0 microsends
	tv.tv_usec = 0;
	

	// Create a UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		std::cout << "socket failed: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	socklen_t len = sizeof(servaddr); // Needed for recvfrom

	// Fill server information
	servaddr.sin_family = AF_INET; 			// IPv4
	servaddr.sin_addr.s_addr = inet_addr(argv[1]); // create address from user input
	servaddr.sin_port = htons(PORT); 		// port number
	
	// Set timeout socket option
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
		(char *)&tv, sizeof(struct timeval)) < 0) {
		std::cout << "setsockopt failed: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<MAX_MSGS; i++) {
		auto start = std::chrono::high_resolution_clock::now(); // timer start
		strcpy(buffer, "ping"); // Set message to be sent
		// Send a packet to server
		n = sendto(sockfd, (char *) buffer, sizeof(buffer),
			MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr));
		if (n < 0){ // check for -1
			std::cout << "sendto failed: " << strerror(errno) << std::endl;
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// Receive return packet from server
		n = recvfrom(sockfd, (char *)buffer, sizeof(buffer),
			MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
		if (n < 0) { // check for -1
			if (errno == EAGAIN) { // Check for timeout
				std::cout << "Waited 1 second, assuming packet is lost." << std::endl;
				continue;
			}
			// Otherwise print error and continue
			std::cout << "recvfrom failed: " << strerror(errno) << std::endl;
			continue;
		}
		buffer[n] = '\0';

		std::cout  << buffer << std::endl;

		// End and calculate elapsed time
		auto end = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
		duration *= 1e-9; // move nano seconds to seconds

		// Report the Round Trip Time
		std::cout << "RTT_" << i+1 << " = " << duration << std::setprecision(9) << "s" << std::endl;

	}
	close(sockfd);
	return 0;
}
