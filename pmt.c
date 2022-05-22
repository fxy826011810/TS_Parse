//
// Created by fan on 2021/5/21.
//
#include <stdio.h>
#include "pmt.h"

typedef enum {
    PMT_STREAM_TYPE_VIDEO,
    PMT_STREAM_TYPE_AUDIO,
}_pmt_stream_type_av_e;

typedef struct{
    unsigned int valid;
    _pmt_stream_type_av_e type;
}_pmt_stream_type_t;


//case 0x06:
//if (true == esnode_p->have_AC3_descriptor)
//{
//ts_p->pid[p_es->i_pid].pid_type = TYPE_AUDIO_AC3;
//}
//else if (true == esnode_p->is_DRA_audio)
//{
//ts_p->pid[p_es->i_pid].pid_type = TYPE_AUDIO_DRA;
//}
//else
//{
//ts_p->pid[p_es->i_pid].pid_type = TYPE_DATA;
//}

typedef enum {
    PMT_STREAM_TYPE_MPEG1_VIDEO = 0x01,
    PMT_STREAM_TYPE_MPEG2_VIDEO = 0x02,
    PMT_STREAM_TYPE_MPEG1_AUDIO = 0x03,
    PMT_STREAM_TYPE_MPEG2_AUDIO = 0x04,
    PMT_STREAM_TYPE_PRIVATE_SECTION = 0x05,
    PMT_STREAM_TYPE_PES_EXT_PRIVATE = 0x06,
    PMT_STREAM_TYPE_MPEG4_VIDEO = 0x10,
    PMT_STREAM_TYPE_H264_VIDEO = 0x1B,
    PMT_STREAM_TYPE_AVS_VIDEO = 0x42,
    PMT_STREAM_TYPE_AAC_AUDIO = 0x0F,
    PMT_STREAM_TYPE_AC3_AUDIO = 0x81,
}_pmt_stream_type_e;

static _pmt_stream_type_t pmt_stream_type[0xff]={
        [PMT_STREAM_TYPE_MPEG4_VIDEO] = {
                .valid = 1,
                .type = PMT_STREAM_TYPE_VIDEO,
        },
        [PMT_STREAM_TYPE_H264_VIDEO] = {
                .valid = 1,
                .type = PMT_STREAM_TYPE_VIDEO,
        },
        [PMT_STREAM_TYPE_AVS_VIDEO] = {
                .valid = 1,
                .type = PMT_STREAM_TYPE_VIDEO,
        },
        [PMT_STREAM_TYPE_MPEG1_VIDEO] = {
                .valid = 1,
                .type = PMT_STREAM_TYPE_VIDEO,
        },
        [PMT_STREAM_TYPE_MPEG2_VIDEO] = {
                .valid = 1,
                .type = PMT_STREAM_TYPE_VIDEO,
        },
//        [PMT_STREAM_TYPE_MPEG1_AUDIO] = {
//                .valid = 1,
//                .type = PMT_STREAM_TYPE_AUDIO,
//        },
//        [PMT_STREAM_TYPE_MPEG2_AUDIO] = {
//                .valid = 1,
//                .type = PMT_STREAM_TYPE_AUDIO,
//        },
//        [PMT_STREAM_TYPE_AAC_AUDIO] = {
//                .valid = 1,
//                .type = PMT_STREAM_TYPE_AUDIO,
//        },
//        [PMT_STREAM_TYPE_AC3_AUDIO] = {
//                .valid = 1,
//                .type = PMT_STREAM_TYPE_AUDIO,
//        },
};

int pmt_descriptor(unsigned char *buf,S32 buf_len)
{
    unsigned char *ptmp = buf;
    unsigned char descriptor_tag = 0;
    unsigned char descriptor_length = 0;
    unsigned char descriptor_index = 0;
    for(descriptor_index=0;descriptor_index<buf_len;)
    {
        descriptor_tag = ptmp[descriptor_index];
        descriptor_length = ptmp[descriptor_index + 1];
        descriptor_index += 2 + descriptor_length;
    }
    return 0;
}

pmt_callback pmt_register_vpid = NULL;
pmt_callback pmt_register_apid = NULL;
pmt_callback pmt_register_avpid = NULL;
pmt_callback pmt_register_pcrpid = NULL;
int pmt_func(_ts_header *ptsHeader,unsigned char *buf)
{
//    printf("[pmt]\n");
    _pmt_struct pmtStruct={0};
    pmtStruct.table_id = buf[0];
    pmtStruct.section_syntax_indicator = buf[1]>>7;
    pmtStruct.zero = (buf[1]>>6)&0x01;
    pmtStruct.section_length = ((buf[1]&0x0f)<<8)|buf[2];
    pmtStruct.program_number = (buf[3]<<8)|buf[4];
    pmtStruct.version_number = ((buf[5]>>1)&0x1f);
    pmtStruct.current_next_indicator = buf[5]&0x01;
    pmtStruct.section_number = buf[6];
    pmtStruct.last_section_number = buf[7];
    pmtStruct.PCR_PID = ((buf[8]&0x1f)<<8)|buf[9];
    pmtStruct.program_info_length = ((buf[10]&0xf)<<8)|buf[11];
    if(pmtStruct.current_next_indicator == 0)
    {
        return -1;
    }
    if(pmtStruct.program_info_length >= 2)
    {
        pmt_descriptor(&buf[12],pmtStruct.program_info_length);
    }
//    printf("table_id=%d\n",pmtStruct.table_id);
//    printf("section_syntax_indicator=%d\n",pmtStruct.section_syntax_indicator);
//    printf("zero=%d\n",pmtStruct.zero);
//    printf("section_length=%d\n",pmtStruct.section_length);
//    printf("program_number=%d\n",pmtStruct.program_number);
//    printf("version_number=%d\n",pmtStruct.version_number);
//    printf("current_next_indicator=%d\n",pmtStruct.current_next_indicator);
//    printf("section_number=%d\n",pmtStruct.section_number);
//    printf("last_section_number=%d\n",pmtStruct.last_section_number);
//    printf("PCR_PID=%d\n",pmtStruct.PCR_PID);
//    printf("program_info_length=%d\n",pmtStruct.program_info_length);
    unsigned int same_pcr_pid = 0;
    unsigned int stream_type = 0;
    unsigned int elementary_PID = 0;
    unsigned int ES_info_length = 0;
    unsigned char *ptmp = &buf[12 + pmtStruct.program_info_length];
    int max_data_len = pmtStruct.section_length - 9 - 4;
//    printf("max_data_len=%d\n",max_data_len);
    for (int data_len = 0; data_len < max_data_len; ) {
        stream_type = ptmp[0];

        elementary_PID = ((ptmp[1]&0x1f)<<8)|ptmp[2];
        ES_info_length = ((ptmp[3]&0xf)<<8)|ptmp[4];
//        printf("stream_type=0x%x\n",stream_type);
//        printf("reserved=%d\n",ptmp[1]>>5);
//        printf("elementary_PID=%d\n",elementary_PID);
//        printf("reserved=%d\n",ptmp[3]>>4);
//        printf("ES_info_length=%d\n",ES_info_length);
        if(pmt_stream_type[stream_type].valid == 1)
        {
            if(pmt_register_vpid && pmt_stream_type[stream_type].type == PMT_STREAM_TYPE_VIDEO)
            {
                pmt_register_vpid(elementary_PID);
            }
            if(pmt_register_apid && pmt_stream_type[stream_type].type == PMT_STREAM_TYPE_AUDIO)
            {
                pmt_register_apid(elementary_PID);
            }
        }

        if(pmtStruct.PCR_PID == elementary_PID)
        {
            same_pcr_pid = 1;
        }
        if(ES_info_length>=2)
        {
            pmt_descriptor(&ptmp[5],ES_info_length);
        }
        data_len += 5 + ES_info_length;
        ptmp += 5 + ES_info_length;
    }
    if(same_pcr_pid == 0 && pmt_register_pcrpid)
    {
        pmt_register_pcrpid(pmtStruct.PCR_PID);
    }
    return 0;
}

int pmt_env_init(pmt_callback register_vpid,pmt_callback register_apid,pmt_callback register_pcrpid)
{
    pmt_register_vpid = register_vpid;
    pmt_register_apid = register_apid;
    pmt_register_pcrpid = register_pcrpid;
    return 0;
}
