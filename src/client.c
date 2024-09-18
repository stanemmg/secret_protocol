#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "protocol.h"

#define PORT 8162
#define CHANLLENGE_DRL_ADDR ("3.91.129.142") //challenge.drl.io

static socket_status_e create_socket(int *sockfd, struct sockaddr_in* servaddr) {
	/* Creating Socket and Validating */
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd == -1) {
		printf("socket creation failed...\n");
		return SOCKET_CREATION_ERROR;
	}

    /* Connect to server  */
	if (connect(*sockfd, (struct sockaddr*)servaddr, sizeof(*servaddr)) != 0) {
		printf("connection with the server failed...\n");
		return SOCKET_CONNECTION_ERROR;
	} else{
		printf("connected to the server..\n");
	}
    return SOCKET_CONNECTED;
}

static int16_t get_checksum(uint8_t *buffer, uint8_t length) {
    uint16_t checksum = 1;
    uint16_t counter = 0;
    uint8_t *byte = buffer;
    for(;byte <= buffer + length; byte++) {
        checksum = (checksum + counter + *byte) % 0xFFF1;
        counter = (counter + 1) % 0x100;
    }
    return checksum;
}

static void print_message(client_message_t *message) {
    printf( "\nMagic: %04x\n"
            "Length: %u\n"
            "Message Type: %u\n"
            "Timestamp: %lu\n"
            "Counter: %u\n"
            "Payload CS: %04x\n"
            "Header CS: %04x\n"
            "Payload: %s\n",
            message->msg.magic, 
            message->msg.length,
            message->msg.message_type,
            message->msg.timestamp,
            message->msg.counter,
            message->msg.payload_cs,
            message->msg.header_cs,
            message->msg.payload);
}

static void print_server_message(server_message_t *message) {
    printf( "\nMagic: %04x\n"
            "Length: %u\n"
            "Message Type: %u\n"
            "Timestamp: %lu\n"
            "Counter: %u\n"
            "Payload: %s\n",
            message->msg.magic, 
            message->msg.length,
            message->msg.message_type,
            message->msg.timestamp,
            message->msg.counter,
            message->msg.payload);
}

static void send_message(client_message_t *message, int *sockfd) {
    message->msg.magic = PROTOCOL_MAGIC;
    message->msg.timestamp = time(NULL);
    message->msg.payload_cs = get_checksum(message->msg.payload, message->msg.length);
    message->msg.header_cs = get_checksum(message->buffer, PROTOCOL_CLIENT_HEADER_SIZE);
    write(*sockfd, message, sizeof(*message));
    print_message(message);
}

int main() {
    printf("Secret Protocol\n");	
    int sockfd;
	struct sockaddr_in servaddr;

    /* Initialize Socket Parameters */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(CHANLLENGE_DRL_ADDR);
	servaddr.sin_port = htons(PORT);

    if(create_socket(&sockfd, &servaddr) != SOCKET_CONNECTED) {
        exit(0);
    }

    client_message_t nameMessage = {
        .msg.message_type = TYPE_1,
        .msg.payload = "[Stanley Garcia]",
        .msg.length = sizeof("[Stanley Garcia]"),
    };

    send_message(&nameMessage, &sockfd);

    server_message_t receivedMsg;
    bzero(&receivedMsg.buffer, sizeof(&receivedMsg.buffer));
    read(sockfd, &receivedMsg.buffer, PROTOCOL_SERVER_MESSAGE_SIZE);
    print_server_message(&receivedMsg);
	close(sockfd);
}

