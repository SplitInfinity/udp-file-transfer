/* deliver.c
 * This file implements the client program that sends a specified file to the server. The file to be sent, server address, server port and client list port
 * are accepted as command line arguments. Make sure to run the client and server in different directories.
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "utils.h"

int main (int argc, char **argv) {
    struct addrinfo hints, *server_info, *my_info;
    int socketFD;
    const char *inputFileName = argv[4];
    FILE *inputFile;

    // Ensure that the correct number of command line arguments were provided
    if (argc != 5) {
        printf("Usage: deliver <server address> <server port number> <client listen port> <file name>\n");
        return 1;
    }

    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    // Get IP address of the server
    getaddrinfo(argv[1], argv[2], &hints, &server_info);

    // Get client's IP address
    getaddrinfo(NULL, argv[3], &hints, &my_info);

    if (server_info != NULL) {

        // Create socket
        socketFD = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

        struct timeval tv;

        // 1-second timeout on recv calls with socket
        tv.tv_sec = 1;

        // Not initialzingthis can cause strange errors
        tv.tv_usec = 0;
        setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

        bind(socketFD, my_info->ai_addr, my_info->ai_addrlen);

        inputFile = fopen(inputFileName, "rb");

        struct stat inputFileInfo;

        // Get filesize to calculate total number of fragments and total number of fragment digits
        stat(inputFileName, &inputFileInfo);

        unsigned int total_frag = ((inputFileInfo.st_size) / MAX_DATA_SIZE) + 1;
        unsigned int current_frag_no = 1;
        // unsigned int total_frag_digits = getNumDigits(total_frag);
        // unsigned int maxPacketSizeNoMsg = 2 * total_frag_digits + MAX_DATA_SIZE_DIGITS + strlen(inputFileName) + NUM_COLONS;
        // char *currentPacket = (char*) malloc (sizeof(char)*(maxPacketSizeNoMsg + MAX_DATA_SIZE));

        char *byteArrayPacket;
        Packet currentPacket;
        currentPacket.total_frag = total_frag;
        strncpy (currentPacket.filename, inputFileName, strlen(inputFileName) + 1);

        while (current_frag_no <= total_frag) {
            currentPacket.frag_no = current_frag_no;

            //char fileBuffer[MAX_DATA_SIZE + 1] = {0};
            bool endOfFile = feof(inputFile);
            char nextChar;

            unsigned int i;
            for (i = 0; i < MAX_DATA_SIZE && !endOfFile; i++) {
                nextChar = getc (inputFile);
                if (feof(inputFile)) {
                    endOfFile = true;
                    i--;
                } else {
                    currentPacket.filedata[i] = nextChar;
                }
            }

            currentPacket.filedata[i] = '\0';
            currentPacket.size = i;

            unsigned int currentPacketTotalSize = createPacket (&byteArrayPacket, &currentPacket);
            // unsigned int currentPacketTotalSize = createPacket (currentPacket, fileBuffer,  inputFileName, i, total_frag, current_frag_no, maxPacketSizeNoMsg);

            char buf[BUFLEN];
            int ackReceived = 0;
            Packet ackPacket;

            do {
                //Transmit the packet that was just created
                int bytesSent = sendto(socketFD, byteArrayPacket, currentPacketTotalSize, 0, server_info->ai_addr, sizeof(struct sockaddr_storage));

                struct sockaddr_storage dummyVar;
                int dummyVar_len = sizeof(dummyVar);

                // Wait 1 second for ACK packet from server
                int bytesReceived = recvfrom(socketFD, buf, BUFLEN, 0, (struct sockaddr *) &dummyVar, &dummyVar_len);

                if (bytesReceived != -1) {
                    ackReceived = 1;
                    memset(&ackPacket,0, sizeof(Packet));
                    extractPacket(&ackPacket, buf, bytesReceived);
                    current_frag_no = ackPacket.frag_no;
                }

            } while(ackReceived == 0 && current_frag_no <= total_frag);

            free(byteArrayPacket);
        }

        //Free the server_info and my_info linked lists from getaddrinfo calls
        freeaddrinfo(server_info);
        freeaddrinfo(my_info);
    }

    return 0;
}
