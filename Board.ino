#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include <unordered_set>
#include <cstring>
#include <Arduino.h>

#include <Sniffer.h>

enum
{
    BAUD_RATE = 115200,
    WIFI_CHANNEL_MAX = 13,
    WIFI_CHANNEL_MIN = 1,
    WIFI_CHANNEL_INTERVAL_MIN = 100,
    WIFI_CHANNEL_INTERVAL_FACTOR = 30,
    RSSI_THRESHOLD = -70,
    PACKET_TIMEOUT = 20000,
    MAX_PACKETS = 50
};

namespace std
{
    // A specialized std::hash template for the Packet class
    template<>
    struct hash<Packet>
    {
        size_t operator()(const Packet & obj) const
        {
            uint8_t const *addr = obj.address();

            size_t r = 0;
            for (uint8_t i = 0; i != Packet::addr_size; ++i)
                r = (r<<1) ^ addr[i];
            return r;
        }
    };
}

void wifi_sniffer_set_channel(uint8_t const channel)
{
    // printf("CHN:\t%2d\n", channel);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

bool check_timeout(Packet const pkt)
{
    return (millis() - pkt.timestamp()) > PACKET_TIMEOUT;
}

std::unordered_set<Packet> packets;

ChannelTimer channel_timer(1, 13, wifi_sniffer_set_channel);

unsigned long mem0;

unsigned long t0 = millis();
uint16_t total_packets = 0;

void setup() {

    /* setup */
    Serial.begin(BAUD_RATE);
    wifi_sniffer_init();

    channel_timer.start();
    mem0 = ESP.getFreeHeap();
}


void loop()
{   
    delay(500);

    while (true) // Find all Packets that have timed out and erase them.
    {
        auto it = std::find_if(packets.begin(), packets.end(), check_timeout);
        if (it == packets.end()) // No timed out packet was found.
            break;
        packets.erase(it);
    }

    // printf("%f\t%d\n", (double)((long)mem0-(long)ESP.getFreeHeap())/100, packets.size());
    // printf("Rate: %6.2f\n", float(total_packets*1000)/float(millis()-t0));
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void wifi_sniffer_init(void)
{
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}


bool add_packet(Packet const *pkt)
{
    std::pair<std::unordered_set<Packet>::iterator,bool> ret;

    ret = packets.insert(*pkt); // Try to insert into the set
    return ret.second;          // Return success value
}

void print_packet(Packet const *pkt)
{
    const uint8_t *addr = pkt->address();
    printf("%02d,%02x:%02x:%02x:%02x:%02x:%02x\n",
            pkt->rssi(),
            addr[0],addr[1],addr[2],
            addr[3],addr[4],addr[5]
        );
}

void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type)
{
    // unsigned long timer = micros();
    
    Packet pkt(buff); // Initialize Packet from buffer
    
    if (add_packet(&pkt)) // Try to insert into set
    {
        print_packet(&pkt);
        channel_timer.goodPacket(); // Print the packet to serial
        ++total_packets;
    }
    // printf("TIME: %d\n", (micros() - timer));
}