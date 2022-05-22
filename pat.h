//
// Created by fan on 2021/5/21.
//

#ifndef TS_PAT_H
#define TS_PAT_H
#include "platform.h"
#include "ts_header.h"
typedef struct {
    S32 table_id;// 8位  固定为0x00，表示该表是PAT表
    S32 section_syntax_indicator;// 1   段语法标志位，固定为1
    S32 section_length;// 12  段的大小，表示这个字节后面有用的字节数，包括CRC32.假如后面的字节加上前面的字节数少于188，后面会用0xFF填充，假如这个数值比较大，则PAT会分成几部分来传输。
    S32 transport_stream_id;// 16  该传输流的ID，区别于一个网络中其他多路复用的流,
    S32 version_number;// 5    范围0-31，表示PAT的版本号，标注当前节目的版本，这是个非常有用的参数，当检测到这个字段改变时，说明TS流中的节目已经改变了，程序必须重新收索节目
    S32 current_next_indicator;// 1    表示发送的PAT是当前有效还是下一个PAT有效
    S32 section_number;// 8    分段的号码，PAT可能分为多个段传输，第一段为00，以后每个分段加1，最大可能有256个分段
    S32 last_section_number;// 8    最后一个分段的号码
} _pat_struct;
typedef int (*pat_callback)(unsigned int pid);

int pat_env_init(pat_callback register_pmt,pat_callback register_nit);
int pat_func(_ts_header *ptsHeader,unsigned char *buf);

#endif //TS_PAT_H
