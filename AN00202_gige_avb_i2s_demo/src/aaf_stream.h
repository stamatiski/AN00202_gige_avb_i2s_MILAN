#ifndef AAF_STREAM_H
#define AAF_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include "milan.h"

typedef struct aaf_stream {
    uint32_t stream_id;
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bit_depth;
    void *buffer;
    size_t buffer_size;
    milan_stream_config_t *milan_cfg;
} aaf_stream_t;

void aaf_stream_init(aaf_stream_t *stream,
                     milan_stream_config_t *milan_cfg,
                     uint32_t stream_id,
                     uint32_t sample_rate,
                     uint16_t channels,
                     uint16_t bit_depth,
                     void *buffer,
                     size_t buffer_size);

int aaf_stream_send(aaf_stream_t *stream, const void *data, size_t len);

int aaf_stream_receive(aaf_stream_t *stream, void *data, size_t max_len);

void aaf_stream_print_info(const aaf_stream_t *stream);

#endif // AAF_STREAM_H
