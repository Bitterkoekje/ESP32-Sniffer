#include "Packet.h"

Packet::Packet(void const *buff)
//:
{
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    d_timestamp = millis();
    d_rssi = - ppkt->rx_ctrl.rssi;

    std::memcpy(d_addr, hdr->addr2, PACKET_ADDR_SIZE);
}

Packet::~Packet()
{
}


bool Packet::checkAdress(const uint8_t addr1[], const uint8_t addr2[], uint8_t len)
{
    for(size_t idx = 0; idx != len; ++idx)
    {
        if (addr1[idx] != addr2[idx]) 
            return false;
    }
    return true;
}

bool Packet::operator==(const Packet & other) const
{
    return *this == other.address();
}

bool Packet::operator==(const uint8_t addr[]) const
{
    return Packet::checkAdress(this->address(), addr, PACKET_ADDR_SIZE);
}

const unsigned long Packet::timestamp() const
{
    return d_timestamp;
}

const int Packet::rssi() const
{
    return d_rssi;
}

const uint8_t* Packet::address() const
{
    return d_addr;
}