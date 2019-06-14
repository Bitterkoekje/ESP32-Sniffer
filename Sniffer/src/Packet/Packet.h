#ifndef INCLUDED_PACKET_
#define INCLUDED_PACKET_

#include "esp_wifi_types.h"
#include <stdint.h>
#include <cstring>

#include <Arduino.h>

enum
{
    PACKET_ADDR_SIZE = 6
};

class Packet
{

    unsigned long d_timestamp;
    uint8_t d_rssi; // Sign flipped
    uint8_t d_addr[PACKET_ADDR_SIZE];

    public:
        
        static uint8_t const addr_size = PACKET_ADDR_SIZE;
        Packet(void const *buff);
        ~Packet();

        static bool checkAdress(const uint8_t addr1[], const uint8_t addr2[], uint8_t len);
        // bool checkAdress(const uint8_t addr1[], const uint8_t addr2[], uint8_t len) const;

        const unsigned long timestamp() const;
        const int rssi() const;
        const uint8_t* address() const;

        bool operator==(const Packet & other) const;
        bool operator==(const uint8_t addr[]) const;
    private:
};

typedef struct {
    unsigned frame_ctrl:16;
    unsigned duration_id:16;
    uint8_t addr1[6]; /* receiver address */
    uint8_t addr2[6]; /* sender address */
    uint8_t addr3[6]; /* filtering address */
    unsigned sequence_ctrl:16;
    uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

#endif
