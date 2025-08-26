#include "milan.h"
#include <stdio.h>
#include <string.h>

static int milan_format_compliant(const milan_stream_config_t *cfg) {
    return (cfg->channels == MILAN_STREAM_CHANNELS &&
            cfg->sample_rate == MILAN_STREAM_SAMPLE_RATE &&
            cfg->bit_depth == MILAN_STREAM_BIT_DEPTH &&
            cfg->stream_format == MILAN_STREAM_FORMAT);
}

int milan_check_compliance(const milan_stream_config_t *cfg) {
    if (!cfg) return 0;
    // Format check
    if (!milan_format_compliant(cfg)) return 0;
    // Entity/Stream ID check
    for (int i = 0; i < MILAN_ENTITY_ID_LENGTH; ++i)
        if (cfg->entity_id.id[i] == 0) return 0;
    for (int i = 0; i < MILAN_STREAM_ID_LENGTH; ++i)
        if (cfg->stream_id.id[i] == 0) return 0;
    // Redundancy: if set, redundant pair must also be compliant
    if (cfg->redundancy_role != MILAN_REDUNDANCY_NONE && cfg->redundant_pair) {
        if (!milan_format_compliant(cfg->redundant_pair)) return 0;
    }
    // More checks (clocking, AVDECC, etc.) can be integrated here
    return 1;
}

void milan_stream_init(milan_stream_config_t *cfg,
                       const milan_stream_id_t *stream_id,
                       const milan_entity_id_t *entity_id,
                       milan_redundancy_role_t redundancy_role)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
    if (stream_id) memcpy(&cfg->stream_id, stream_id, sizeof(milan_stream_id_t));
    if (entity_id) memcpy(&cfg->entity_id, entity_id, sizeof(milan_entity_id_t));
    cfg->state = MILAN_STREAM_STATE_DISABLED;
    cfg->redundancy_role = redundancy_role;
    cfg->sample_rate = MILAN_STREAM_SAMPLE_RATE;
    cfg->channels = MILAN_STREAM_CHANNELS;
    cfg->bit_depth = MILAN_STREAM_BIT_DEPTH;
    cfg->stream_format = MILAN_STREAM_FORMAT;
    cfg->packets_sent = 0;
    cfg->packets_received = 0;
    cfg->last_error = 0;
    cfg->redundant_pair = NULL;
}

void milan_stream_enable(milan_stream_config_t *cfg) {
    if (!cfg) return;
    if (milan_check_compliance(cfg)) {
        cfg->state = MILAN_STREAM_STATE_ENABLED;
        printf("[Milan] Stream enabled\n");
    } else {
        cfg->state = MILAN_STREAM_STATE_ERROR;
        cfg->last_error = 1;
        printf("[Milan] Stream enable failed: non-compliant\n");
    }
}

void milan_stream_disable(milan_stream_config_t *cfg) {
    if (!cfg) return;
    cfg->state = MILAN_STREAM_STATE_DISABLED;
    printf("[Milan] Stream disabled\n");
}

void milan_stream_connect(milan_stream_config_t *cfg) {
    if (!cfg) return;
    if (cfg->state == MILAN_STREAM_STATE_ENABLED && milan_check_compliance(cfg)) {
        cfg->state = MILAN_STREAM_STATE_CONNECTED;
        printf("[Milan] Stream connected\n");
    } else {
        cfg->state = MILAN_STREAM_STATE_ERROR;
        cfg->last_error = 2;
        printf("[Milan] Stream connect failed\n");
    }
}

void milan_stream_disconnect(milan_stream_config_t *cfg) {
    if (!cfg) return;
    if (cfg->state == MILAN_STREAM_STATE_CONNECTED) {
        cfg->state = MILAN_STREAM_STATE_ENABLED;
        printf("[Milan] Stream disconnected\n");
    }
}


void milan_set_redundant_pair(milan_stream_config_t *primary, milan_stream_config_t *secondary) {
    if (!primary || !secondary) return;
    if (primary->redundancy_role == MILAN_REDUNDANCY_PRIMARY &&
        secondary->redundancy_role == MILAN_REDUNDANCY_SECONDARY) {
        primary->redundant_pair = secondary;
        secondary->redundant_pair = primary;
        printf("[Milan] Redundant pair set\n");
    }
}

void milan_avdecc_describe_entity(const milan_stream_config_t *cfg) {
    // Stub for AVDECC entity descriptor
    printf("[Milan AVDECC] Entity ID: ");
    for(int i=0; i < MILAN_ENTITY_ID_LENGTH; ++i) printf("%02X", cfg->entity_id.id[i]);
    printf("\n");
}

void milan_avdecc_describe_stream(const milan_stream_config_t *cfg) {
    // Stub for AVDECC stream descriptor
    printf("[Milan AVDECC] Stream ID: ");
    for(int i=0; i < MILAN_STREAM_ID_LENGTH; ++i) printf("%02X", cfg->stream_id.id[i]);
    printf(", Ch: %u, SR: %u, BD: %u, Fmt: %u\n", cfg->channels, cfg->sample_rate, cfg->bit_depth, cfg->stream_format);
}

void milan_stream_report_error(milan_stream_config_t *cfg, uint32_t error_code) {
    if (!cfg) return;
    cfg->state = MILAN_STREAM_STATE_ERROR;
    cfg->last_error = error_code;
    printf("[Milan] Error reported: %u\n", error_code);
}

void milan_stream_print_info(const milan_stream_config_t *cfg) {
    if (!cfg) return;
    printf("[Milan] Stream Info:\n");
    printf("  Entity: ");
    for(int i=0; i < MILAN_ENTITY_ID_LENGTH; ++i) printf("%02X", cfg->entity_id.id[i]);
    printf("\n  Stream: ");
    for(int i=0; i < MILAN_STREAM_ID_LENGTH; ++i) printf("%02X", cfg->stream_id.id[i]);
    printf("\n  State: %u\n", cfg->state);
    printf("  Redundancy: %u\n", cfg->redundancy_role);
    printf("  Ch: %u, SR: %u, BD: %u, Format: %u\n",
           cfg->channels, cfg->sample_rate, cfg->bit_depth, cfg->stream_format);
    printf("  Packets sent: %llu, received: %llu\n",
           (unsigned long long)cfg->packets_sent, (unsigned long long)cfg->packets_received);
    printf("  Last error: %u\n", cfg->last_error);
    if (cfg->redundant_pair)
        printf("  Has redundant pair.\n");
}