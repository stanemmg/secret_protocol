#ifndef PROTOCOL_H_   
#define PROTOCOL_H_

#include <stdint.h>
#pragma pack(1)

#define PROTOCOL_MAGIC  0xF00D
#define PROTOCOL_PAYLOAD_SIZE   64
#define PROTOCOL_CLIENT_MESSAGE_SIZE   (2/*magic*/ + 2/*length*/ + 1/*message_type*/ +  8/*timestamp*/+  1/*counter*/ +  2/*payload_cs*/ +  2/*header_cs*/ + PROTOCOL_PAYLOAD_SIZE)
#define PROTOCOL_CLIENT_HEADER_SIZE    PROTOCOL_CLIENT_MESSAGE_SIZE - PROTOCOL_PAYLOAD_SIZE - 2 /*header_cs*/


typedef enum {
    TYPE_1 = 1,
    TYPE_2 = 2,
}message_types_e;

typedef enum {
    SOCKET_CONNECTED,
    SOCKET_CREATION_ERROR,
    SOCKET_CONNECTION_ERROR,

}socket_status_e;

typedef union {
    uint8_t buffer[PROTOCOL_CLIENT_MESSAGE_SIZE];
    struct {
        uint16_t magic;
        uint16_t length;
        uint8_t message_type;
        uint64_t timestamp;
        uint8_t counter;
        uint16_t payload_cs;
        uint16_t header_cs;
        uint8_t payload[PROTOCOL_PAYLOAD_SIZE];
    } msg;
} client_message_t;

#endif // PROTOCOL_H_
