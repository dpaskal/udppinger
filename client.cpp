#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <chrono>		// for timing
#include <iomanip> 		// for setprecision


#define PORT		12000
#define NUM_MSGS	10

int main() {
	int sockfd, n;
	socklen_t len;
	char buffer[1024] = "Hello World";
	struct sockaddr_in servaddr;

	// Create a UDP socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));

	// Fill server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY; // localhost
	servaddr.sin_port = htons(PORT); // port number

	auto start = std::chrono::high_resolution_clock::now(); // timer start
	// Send a packet to server
	n = sendto(sockfd, (char *) buffer, sizeof(buffer),
		MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (n<0){
		std::cout << "sendto failed: " << strerror(errno) << std::endl;
		exit(0);
	}
	std::cout << n << " bytes sent." << std::endl;

	n = recvfrom(sockfd, (char *)buffer, sizeof(buffer),
		MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
	buffer[n] = '\0';
	std::cout << "Client received: " << buffer << std::endl;
	auto end = std::chrono::high_resolution_clock::now(); // timer end
	double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
	duration *= 1e-9; // move nano seconds to seconds
	std::cout << "RTT = " << duration << std::setprecision(9) << "s" << std::endl;

	close(sockfd);
	return 0;
}

// void send(int sockfd, struct sockaddr_in servaddr) {
// 	return sendto(sockfd, (char *) buffer, sizeof(buffer),
// 		MSG_CONFIRM, (struct sockaddr *) &servaddr, len);
// }