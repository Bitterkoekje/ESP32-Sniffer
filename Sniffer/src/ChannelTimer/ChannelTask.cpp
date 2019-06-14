#include "ChannelTimer.h"

void ChannelTimer::task()
{
    while (d_task_running)
    {
        nextChannel();

        uint16_t channel_time = calculateTime(d_channel);

        // printf("%2d, %6d, ", d_channel, channel_time);
        // for(size_t idx = 0; idx != d_n_channels; ++idx)
        // {
        //     printf("%5d ", d_packet_intervals[idx]);
        // }
        // printf("\n");

        d_packet_count = 0;
        vTaskDelay(channel_time/portTICK_PERIOD_MS);

        uint16_t interval;

        if (d_packet_count != 0)
        {
            interval = channel_time/d_packet_count;
        }
        else
        {
            interval = float(1000);
        }

        // a*Y + (1-a)*S
        float a = 0.4;
        float b = 0.6;
        interval *= a;
        d_packet_intervals[channelIndex(d_channel)] *= b;
        d_packet_intervals[channelIndex(d_channel)] += interval;
    }

    vTaskDelete( NULL );
}