#include "aaf_stream.h"
#include "milan.h"
#include <stdio.h>
#include <string.h>

void aaf_stream_init(aaf_stream_t *stream,
                     milan_stream_config_t *milan_cfg,
                     uint32_t stream_id,
                     uint32_t sample_rate,
                     uint16_t channels,
                     uint16_t bit_depth,
                     void *buffer,
                     size_t buffer_size)
{
    if (!stream) return;
    stream->stream_id = stream_id;
    stream->sample_rate = sample_rate;
    stream->channels = channels;
    stream->bit_depth = bit_depth;
    stream->buffer = buffer;
    stream->buffer_size = buffer_size;
    stream->milan_cfg = milan_cfg;

    printf("[AAF] Stream initialized. Milan: %s\n",
           milan_cfg && milan_check_compliance(milan_cfg) ? "YES" : "NO");
}

int aaf_stream_send(aaf_stream_t *stream, const void *data, size_t len)
{
    if (!stream || !stream->buffer || len > stream->buffer_size) return -1;
    if (stream->milan_cfg) {
        if (stream->milan_cfg->state != MILAN_STREAM_STATE_CONNECTED) {
            printf("[AAF] ERROR: Milan AAF stream not connected. Transmission aborted.\n");
            return -2;
        }
        // Redundant transmission if required
        if (stream->milan_cfg->redundant_pair) {
            memcpy(stream->milan_cfg->redundant_pair->buffer, data, len);
            stream->milan_cfg->redundant_pair->packets_sent++;
            printf("[AAF] Redundant transmission to secondary AAF stream.\n");
        }
        stream->milan_cfg->packets_sent++;
    }
    memcpy(stream->buffer, data, len);
    printf("[AAF] Sent %zu bytes (%s)\n", len,
           stream->milan_cfg ? "Milan" : "Legacy");
    return (int)len;
}

int aaf_stream_receive(aaf_stream_t *stream, void *data, size_t max_len)
{
    if (!stream || !stream->buffer || max_len > stream->buffer_size) return -1;
    if (stream->milan_cfg) {
        if (stream->milan_cfg->state != MILAN_STREAM_STATE_CONNECTED) {
            printf("[AAF] ERROR: Milan AAF stream not connected. Receive aborted.\n");
            return -2;
        }
        stream->milan_cfg->packets_received++;
    }
    memcpy(data, stream->buffer, max_len);
    printf("[AAF] Received %zu bytes (%s)\n", max_len,
           stream->milan_cfg ? "Milan" : "Legacy");
    return (int)max_len;
}

void aaf_stream_print_info(const aaf_stream_t *stream)
{
    if (!stream) return;
    printf("[AAF] Stream ID: %u, SR: %u, Ch: %u, BD: %u, Milan: %s\n",
        stream->stream_id, stream->sample_rate, stream->channels, stream->bit_depth,
        stream->milan_cfg ? "YES" : "NO");
    if (stream->milan_cfg) {
        milan_stream_print_info(stream->milan_cfg);
    }
}