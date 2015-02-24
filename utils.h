/* utils.h
 * This file contains definitions for utility functions, structs and packet field size macros that are used by both the client and server.
 */

#ifndef UTILS_H
#define UTILS_H

#define MAX_DATA_SIZE 1000		// Number of bytes in the data field of a packet
#define MAX_DATA_SIZE_DIGITS 4		// Number of digits in MAX_DATA_SIZE
#define NUM_COLONS 4			// Number of colons in a packet when represented as a byte array
#define MAX_FRAG_DIGITS 10              // Number of digits in the largest fragment number
#define MAX_FILE_NAME_SIZE 100          // Maximum file name length
#define BUFLEN 5000			// Generic buffer size used for recv calls by both client and server

/*
 * Struct that represents a packet sent by the client. The file that the client sends is segmented into packets that look like
 * this before being sent through the socket.
 * 	total_frag - the total number of packets/fragments that the client will send to transmit the file in its entirety
 *	frag_no - the sequence number of the current packet/fragment; first packet has frag_no = 1, second packet has frag_no = 2, etc.
 *	size - number of bytes in the packet
 *	filename - the name of the file to which the packet data belongs
 *	filedata - the packet data itself
 */

typedef struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char filename[MAX_FILE_NAME_SIZE];
    char filedata[MAX_DATA_SIZE + 1];
} Packet;

/*
 * Gets the number of digits in a given base-10 integer.
 *	@ param[in]	baseTenNumber		A base-10 integer
 *	@ return 	The number of digits in baseTenNum
 */

unsigned int getNumDigits(int baseTenNum);

/*
 * Extracts one colon-delimited string from mainStr and writes it to output.
 *	@ param[in/out]		mainStr		The string to perform the extraction on.
 *	@ param[out]		output		The buffer in which to place the output.
 *	@ param[in/out]		counter		The index in mainStr at which to start the search.
 */

void extractIndivStrings (const char * mainStr, char * output, int * counter);

/*
 * Extracts information from the byte-array form of a packet.
 *	@ param[out]	result			The Packet struct in which to place the extracted information.
 *	@ param[in]	byteArray		The raw, byte-array representation of the packet.
 *	@ param[in]	totalPacketSize		The index in mainStr at which to start the search.
 */

void extractPacket (Packet * result, const char * byteArray, int totalPacketSize);

/*
 * Creates a byte-array version of the given packet that can be transmitted using a socket.
 *	@ param[out]	byteArray		A buffer in which the raw byte-array form of the packet is stored
 *	@ param[in]	packet			A Packet struct containing the data to be converted to byte-array form.
 *	@ return	The actual size of the byte-array
 *
 *	The byte-array form of a packet is total_frag:frag_no:size:filename:filedata.
 */

unsigned int createPacket (char **byteArray, const Packet* packet);

#endif
