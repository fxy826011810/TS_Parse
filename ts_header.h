//
// Created by fan on 2021/5/21.
//

#ifndef TS_TS_HEADER_H
#define TS_TS_HEADER_H
#include "platform.h"
typedef struct {
    S32 sync_byte;
    S32 transport_error_indicator;
    S32 payload_unit_start_indicator;
    S32 transport_priority;
    S32 pid;
    S32 transport_scrambling_control;
    S32 adaptation_field_control;
    S32 continuity_counter;
    S32 buffer_offset;
}__attribute__((packed,aligned(1))) _ts_header;

typedef int (*pfunc)(_ts_header *ptsHeader,unsigned char *buf);

typedef enum{
    PACKET_PES,
    PACKET_SECTION,
    PACKET_PCR,
}_packet_type_e;
typedef struct{
    pfunc cb;
    _packet_type_e packet_type;
}_pid_base_t;

int parse_ts_header(unsigned char *buf,_pid_base_t *pfunc_list);
#endif //TS_TS_HEADER_H
