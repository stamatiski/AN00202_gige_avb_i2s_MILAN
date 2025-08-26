#ifndef CRF_STREAM_H
#define CRF_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include "milan.h"

typedef struct crf_stream {
    uint32_t crf_id;
    uint32_t frequency;
    void *buffer;
    size_t buffer_size;
    milan_stream_config_t *milan_cfg; // Milan integration
} crf_stream_t;

void crf_stream_init(crf_stream_t *stream,
                     milan_stream_config_t *milan_cfg,
                     uint32_t crf_id,
                     uint32_t frequency,
                     void *buffer,
                     size_t buffer_size);

int crf_stream_send(crf_stream_t *stream, const void *data, size_t len);

int crf_stream_receive(crf_stream_t *stream, void *data, size_t max_len);

void crf_stream_print_info(const crf_stream_t *stream);

#endif // CRF_STREAM_H
