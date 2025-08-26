#include "crf_stream.h"
#include "milan.h"
#include <stdio.h>
#include <string.h>

// Initialize a CRF stream with advanced Milan integration
void crf_stream_init(crf_stream_t *stream,
                     milan_stream_config_t *milan_cfg,
                     uint32_t crf_id,
                     uint32_t frequency,
                     void *buffer,
                     size_t buffer_size)
{
    if (!stream) return;
    stream->crf_id = crf_id;
    stream->frequency = frequency;
    stream->buffer = buffer;
    stream->buffer_size = buffer_size;
    stream->milan_cfg = milan_cfg;

    printf("[CRF] Stream initialized. Milan: %s\n",
           milan_cfg && milan_check_compliance(milan_cfg) ? "YES" : "NO");
}

// Send a CRF packet, respecting Milan state and redundancy
int crf_stream_send(crf_stream_t *stream, const void *data, size_t len)
{
    if (!stream || !stream->buffer || len > stream->buffer_size) return -1;
    if (stream->milan_cfg) {
        if (stream->milan_cfg->state != MILAN_STREAM_STATE_CONNECTED) {
            printf("[CRF] ERROR: Milan CRF stream not connected. Transmission aborted.\n");
            return -2;
        }
        // Redundant transmission if required
        if (stream->milan_cfg->redundant_pair) {
            memcpy(stream->milan_cfg->redundant_pair->buffer, data, len);
            stream->milan_cfg->redundant_pair->packets_sent++;
            printf("[CRF] Redundant transmission to secondary CRF stream.\n");
        }
        stream->milan_cfg->packets_sent++;
    }
    memcpy(stream->buffer, data, len);
    printf("[CRF] Sent %zu bytes (%s)\n", len,
           stream->milan_cfg ? "Milan" : "Legacy");
    return (int)len;
}

// Receive a CRF packet, respecting Milan state
int crf_stream_receive(crf_stream_t *stream, void *data, size_t max_len)
{
    if (!stream || !stream->buffer || max_len > stream->buffer_size) return -1;
    if (stream->milan_cfg) {
        if (stream->milan_cfg->state != MILAN_STREAM_STATE_CONNECTED) {
            printf("[CRF] ERROR: Milan CRF stream not connected. Receive aborted.\n");
            return -2;
        }
        stream->milan_cfg->packets_received++;
    }
    memcpy(data, stream->buffer, max_len);
    printf("[CRF] Received %zu bytes (%s)\n", max_len,
           stream->milan_cfg ? "Milan" : "Legacy");
    return (int)max_len;
}

// Print CRF stream info (for debug)
void crf_stream_print_info(const crf_stream_t *stream)
{
    if (!stream) return;
    printf("[CRF] CRF ID: %u, Frequency: %u Hz, Milan: %s\n",
        stream->crf_id, stream->frequency,
        stream->milan_cfg ? "YES" : "NO");
    if (stream->milan_cfg) {
        milan_stream_print_info(stream->milan_cfg);
    }
}