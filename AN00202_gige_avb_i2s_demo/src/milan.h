#ifndef MILAN_H
#define MILAN_H

#include <stdint.h>
#include <stddef.h>

/*
 * Milan Protocol - Advanced Structure
 * Follows Milan Stream Format, Entity, Redundancy, Diagnostics
 */

// Milan-compliant stream format
#define MILAN_STREAM_CHANNELS      8
#define MILAN_STREAM_SAMPLE_RATE   48000
#define MILAN_STREAM_BIT_DEPTH     24
#define MILAN_STREAM_FORMAT        0x02 // MBLA

#define MILAN_ENTITY_ID_LENGTH     8
#define MILAN_STREAM_ID_LENGTH     8

typedef enum {
    MILAN_STREAM_STATE_DISABLED = 0,
    MILAN_STREAM_STATE_ENABLED  = 1,
    MILAN_STREAM_STATE_CONNECTED = 2,
    MILAN_STREAM_STATE_ERROR = 3
} milan_stream_state_t;

typedef enum {
    MILAN_REDUNDANCY_NONE = 0,
    MILAN_REDUNDANCY_PRIMARY = 1,
    MILAN_REDUNDANCY_SECONDARY = 2
} milan_redundancy_role_t;

typedef struct {
    uint8_t id[MILAN_ENTITY_ID_LENGTH];
} milan_entity_id_t;

typedef struct {
    uint8_t id[MILAN_STREAM_ID_LENGTH];
} milan_stream_id_t;

// Advanced Milan stream configuration
typedef struct milan_stream_config_t {
    milan_stream_id_t stream_id;
    milan_entity_id_t entity_id;
    milan_stream_state_t state;
    milan_redundancy_role_t redundancy_role;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bit_depth;
    uint8_t stream_format;
    // Diagnostics and statistics
    uint64_t packets_sent;
    uint64_t packets_received;
    uint32_t last_error;
    // Redundancy
    struct milan_stream_config_t* redundant_pair;
} milan_stream_config_t;

// Milan compliance check (format, clocking, redundancy, etc.)
int milan_check_compliance(const milan_stream_config_t *cfg);

// Milan stream configuration and state machine
void milan_stream_init(milan_stream_config_t *cfg,
                       const milan_stream_id_t *stream_id,
                       const milan_entity_id_t *entity_id,
                       milan_redundancy_role_t redundancy_role);

void milan_stream_enable(milan_stream_config_t *cfg);
void milan_stream_disable(milan_stream_config_t *cfg);
void milan_stream_connect(milan_stream_config_t *cfg);
void milan_stream_disconnect(milan_stream_config_t *cfg);

// AVDECC descriptor hooks (to integrate with AVDECC stack)
void milan_avdecc_describe_entity(const milan_stream_config_t *cfg);
void milan_avdecc_describe_stream(const milan_stream_config_t *cfg);

// Redundancy management
void milan_set_redundant_pair(milan_stream_config_t *primary, milan_stream_config_t *secondary);

// Diagnostics and error reporting
void milan_stream_report_error(milan_stream_config_t *cfg, uint32_t error_code);
void milan_stream_print_info(const milan_stream_config_t *cfg);

#endif // MILAN_H
