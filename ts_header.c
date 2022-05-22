//
// Created by fan on 2021/5/21.
//
#include <stdio.h>
#include "ts_header.h"
#include "avpid.h"

int parse_ts_header(unsigned char *buf,_pid_base_t *pfunc_list)
{
    _ts_header tsHeader={0};
    tsHeader.sync_byte = buf[0];
    tsHeader.transport_error_indicator = buf[1]>>7;
    tsHeader.payload_unit_start_indicator = (buf[1]>>6)&0x01;
    tsHeader.transport_priority = (buf[1]>>5)&0x01;
    tsHeader.pid = ((buf[1]&0x1f)<<8|buf[2]);
    tsHeader.transport_scrambling_control = buf[3]>>6;
    tsHeader.adaptation_field_control = (buf[3]>>4)&0x03;
    tsHeader.continuity_counter = buf[3]&0x0f;
    if(pfunc_list[tsHeader.pid].cb == NULL)
    {
        return -1;
    }
    if(tsHeader.adaptation_field_control == 2)//only adaptation_field
    {
        return -2;
    }
    else if(tsHeader.adaptation_field_control == 3)//payload + adaptation_field
    {
        tsHeader.buffer_offset = 4  + /*tsHeader.payload_unit_start_indicator*/ + 1 +buf[4];
    }
    else if(tsHeader.adaptation_field_control == 1) //only payload
    {
        tsHeader.buffer_offset = 4  ;//+ tsHeader.payload_unit_start_indicator;
    }
    else
    {
        printf("err\n");
    }
    if(pfunc_list[tsHeader.pid].packet_type == PACKET_SECTION)
    {
        tsHeader.buffer_offset += tsHeader.payload_unit_start_indicator;
    }
    pfunc_list[tsHeader.pid].cb(&tsHeader,&buf[tsHeader.buffer_offset]);
//    if(pid[tsHeader.pid] != 0)
//    {
//        return 0;
//    }
//    pid[tsHeader.pid] = 1;
//    if(tsHeader.pid == 8191)
//    {
//        return 0;
//    }
//    if(tsHeader.payload_unit_start_indicator == 0)
//    {
//        return 0;
//    }
//    printf("\n[tsHeader]\n");
//    printf("sync_byte=%x\n",tsHeader.sync_byte);
//    printf("transport_error_indicator=%d\n",tsHeader.transport_error_indicator);
//    printf("payload_unit_start_indicator=%d\n",tsHeader.payload_unit_start_indicator);
//    printf("transport_priority=%d\n",tsHeader.transport_priority);
//    printf("pid=%d\n",tsHeader.pid);
//    printf("transport_scrambling_control=%d\n",tsHeader.transport_scrambling_control);
//    printf("adaptation_field_control=%d\n",tsHeader.adaptation_field_control);
//    printf("continuity_counter=%d\n",tsHeader.continuity_counter);

    return 0;
}