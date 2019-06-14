#ifndef INCLUDED_CHANNELTIMER_
#define INCLUDED_CHANNELTIMER_

#include "esp_event.h"

#include <stdint.h>

class ChannelTimer
{
    uint8_t     d_channel;
    uint8_t     d_channel_min;
    uint8_t     d_channel_max;
    uint8_t     d_n_channels;

    uint16_t   *d_packet_intervals;
    uint16_t    d_packet_count;

    bool        d_task_running = false;

    void (*p_set_channel_fn)(uint8_t);

    public:
        ChannelTimer(uint8_t channel_min, uint8_t channel_max,
                        void (*set_channel_fn)(uint8_t));
        ~ChannelTimer();

        uint8_t nextChannel();
        uint8_t prevChannel();
        uint8_t setChannel(int16_t channel);
        uint8_t channelMod(int16_t channel) const;

        const uint8_t channel() const;

        void    start();
        void    stop();
        static void startTaskImpl(void*);

        uint16_t calculateTime(uint8_t channel) const;

        void    goodPacket();

    private:
        void    task();

        uint8_t channelIndex(uint8_t channel) const;
};

#endif