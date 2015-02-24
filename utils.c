/* utils.c
 * Implementations of functions in utils.h.
 */

#include "utils.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

unsigned int getNumDigits(int baseTenNum) {
    return (unsigned int)(log10(baseTenNum)) + 1;
}

void extractIndivStrings (const char * mainStr, char * output, int * counter) {
    unsigned int fCounter = 0;
    while (mainStr[(*counter)]!= ':') {
        output[fCounter] = mainStr[(*counter)];
        fCounter++;
        (*counter)++;
    }
    output[fCounter] = '\0';
    (*counter)++;
    return;
}

void extractPacket (Packet * result, const char * byteArray, int totalPacketSize) {
    char totalFrag[MAX_FRAG_DIGITS];
    char currFrag[MAX_FRAG_DIGITS];
    char dataSize[MAX_DATA_SIZE_DIGITS];
    char fileName[MAX_FILE_NAME_SIZE];
    char fileContent[MAX_DATA_SIZE];

    unsigned int i = 0;

    extractIndivStrings(byteArray, totalFrag, &i );
    extractIndivStrings(byteArray, currFrag, &i );
    extractIndivStrings(byteArray, dataSize, &i );
    extractIndivStrings(byteArray, fileName, &i );

    unsigned int fCounter = 0;

    while (i < totalPacketSize) {
        result->filedata[fCounter] = byteArray[i];
        fCounter++;
        i++;
    }

    result->total_frag = atoi(totalFrag);
    result->frag_no = atoi(currFrag);
    result->size = atoi(dataSize);
    strncpy(result->filename, fileName, strlen(fileName) + 1);
    return;
}

unsigned int createPacket (char **byteArray, const Packet* packet) {
    unsigned int total_frag_digits = getNumDigits(packet->total_frag);
    unsigned int maxPacketSizeNoMsg = 2 * total_frag_digits + MAX_DATA_SIZE_DIGITS + strlen(packet->filename) + NUM_COLONS;
    *byteArray = (char*) malloc (sizeof(char)*(maxPacketSizeNoMsg + MAX_DATA_SIZE));

    snprintf(*byteArray, maxPacketSizeNoMsg + 1, "%d:%d:%d:%s:", packet->total_frag, packet->frag_no, packet->size, packet->filename);

    //After processing the packet header data, you need to copy the actual file data byte by byte since file might contain null-terminating characters
    char *message = *byteArray + getNumDigits(packet->total_frag) + getNumDigits(packet->frag_no) + getNumDigits(packet->size) + strlen(packet->filename) + NUM_COLONS;

    // message points to the position after the fourth colon in the byteArray at this point

    unsigned int j;
    for (j = 0; j < packet->size; j++) {
        message[j] = packet->filedata[j];
    }

    unsigned int currentPacketTotalSize = getNumDigits(packet->total_frag) + getNumDigits(packet->size) + getNumDigits(packet->frag_no) + strlen(packet->filename) + packet->size + NUM_COLONS;
    return currentPacketTotalSize;
}



