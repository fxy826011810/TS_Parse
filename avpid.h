//
// Created by fan on 2021/5/21.
//

#ifndef TS_AVPID_H
#define TS_AVPID_H
#include "platform.h"
#include "ts_header.h"

typedef struct {
    S32 packet_start_code_prefix;//  3B  开始码，固定为0x000001
    S32 stream_id;//  1B  音频取值（0xc0-0xdf），通常为0xc0 视频取值（0xe0-0xef），通常为0xe0
    S32 PES_packet_length;//  2B  后面pes数据的长度，0表示长度不限制，只有视频数据长度会超过0xffff
//    S32 flag;//  1B  通常取值0x80，表示数据不加密、无优先级、备份的数据
//    S32 flag;//  1B  取值0x80表示只含有pts，取值0xc0表示含有pts和dts
    S32 PES_scrambling_control;
    S32 PES_priority;
    S32 data_alignment_indicator;
    S32 copyright;
    S32 original_or_copy;
    S32 PTS_DTS_flags;
    S32 ESCR_flag;
    S32 ES_rate_flag;
    S32 DSM_trick_mode_flag;
    S32 additional_copy_info_flag;
    S32 PES_CRC_flag;
    S32 PES_extension_flag;
    S32 PES_header_data_length;
    S32 pes_data_length;//  1B  后面数据的长度，取值5或10
    S32 pts;//  5B  33bit值
    S32 dts;//  5B  33bit值
}_pes_header;
int avpid_func(_ts_header *ptsHeader,unsigned char *buf);
#endif //TS_AVPID_H
