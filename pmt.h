//
// Created by fan on 2021/5/21.
//

#ifndef TS_PMT_H
#define TS_PMT_H
#include "platform.h"
#include "ts_header.h"
typedef struct {
    S32 table_id;//8b PMT表取值随意，0x02
    S32 section_syntax_indicator;// 1b 固定为1
    S32 zero;// 1b 固定为0
//    S32 reserved;// 2b 固定为11
    S32 section_length;// 12b 后面数据的长度
    S32 program_number;// 16b 频道号码，表示当前的PMT关联到的频道，取值0x0001
//    S32 reserved;// 2b 固定为11
    S32 version_number;// 5b 版本号，固定为00000，如果PAT有变化则版本号加1
    S32 current_next_indicator;// 1b 固定为1
    S32 section_number;// 8b 固定为0x00
    S32 last_section_number;// 8b 固定为0x00
//    S32 reserved;// 3b 固定为111
    S32 PCR_PID;// 13b PCR(节目参考时钟)所在TS分组的PID，指定为视频PID
//    S32 reserved;// 4b 固定为1111
    S32 program_info_length;// 12b 节目描述信息，指定为0x000表示没有
}_pmt_struct;

typedef int (*pmt_callback)(unsigned int pid);

int pmt_func(_ts_header *ptsHeader,unsigned char *buf);
int pmt_env_init(pmt_callback register_vpid,pmt_callback register_apid,pmt_callback register_pcrpid);
#endif //TS_PMT_H
