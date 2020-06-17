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
#include <netdb.h>		// for gethostbyname


#define PORT		54444
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
	tv.tv_sec = 1;	// set timeout delay to 1 second 
	tv.tv_usec = 0; // and 0 microsends
	struct hostent *hn; // for translating ip addresses

	// Create a UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		std::cout << "socket failed: " << strerror(errno) << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	socklen_t len = sizeof(servaddr); // Needed for recvfrom


	// Translate input to hostent struct
	// https://man7.org/linux/man-pages/man3/gethostbyname.3.html
	if ((hn = gethostbyname(argv[1])) == NULL )
		return 1; // error checking gethostbyname

	// Fill server information
	servaddr.sin_family = AF_INET; 	// IPv4
	memcpy(&servaddr.sin_addr, hn->h_addr, hn->h_length); // set ip address
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
