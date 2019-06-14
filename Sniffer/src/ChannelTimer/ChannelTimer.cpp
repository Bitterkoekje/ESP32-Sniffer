#include "ChannelTimer.h"

ChannelTimer::ChannelTimer(uint8_t channel_min, uint8_t channel_max, 
                            void (*set_channel_fn)(uint8_t))
:
    d_channel_min(channel_min),
    d_channel_max(channel_max),
    d_n_channels(channel_max-channel_min+1),
    p_set_channel_fn(set_channel_fn),
    d_packet_intervals(new uint16_t[d_n_channels]),
    d_packet_count(0)
{
    setChannel(d_channel_min);

    for (size_t idx = 0; idx != d_n_channels; ++idx)
    {
        d_packet_intervals[idx] = 2000;
    }
}

ChannelTimer::~ChannelTimer()
{
    delete d_packet_intervals;
}

uint8_t ChannelTimer::nextChannel()
{
    return setChannel(d_channel + 1);
}
        
uint8_t ChannelTimer::prevChannel()
{
    return setChannel(d_channel - 1);
}

uint8_t ChannelTimer::setChannel(int16_t channel)
{
    channel = channelMod(channel);
    p_set_channel_fn(channel);
    d_channel = channel;
}

uint8_t ChannelTimer::channelMod(int16_t channel) const
{
    return (channel - d_channel_min)%(d_channel_max - d_channel_min + 1) 
                + d_channel_min;
}

void ChannelTimer::start()
{
    d_task_running = true;
    xTaskCreate(startTaskImpl, "channel_timer", 2048, this, 1, NULL);
}

void ChannelTimer::stop()
{
    d_task_running = false;
}

void ChannelTimer::startTaskImpl(void* _this){
    static_cast<ChannelTimer*>(_this)->task();
}

uint16_t ChannelTimer::calculateTime(uint8_t channel) const
{
    uint16_t interval = d_packet_intervals[channelIndex(channel)];
    uint16_t x1 = 0;
    uint16_t y1 = 5000;
    uint16_t x2 = 2000;
    uint16_t y2 = 400;
    if (interval > x2)
        interval = x2;

    double m = 1000*(y1-y2)/(x2-x1);

    uint16_t channel_time = y1 - (m/1000)*interval;

    return channel_time;
}

void ChannelTimer::goodPacket()
{
    ++d_packet_count;
}

uint8_t ChannelTimer::channelIndex(uint8_t channel) const
{
    return channel - d_channel_min;
}