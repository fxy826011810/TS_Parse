//
// Created by fan on 2021/5/21.
//
#include <stdio.h>
#include "pat.h"

pat_callback pat_register_pmt = NULL;
pat_callback pat_register_nit = NULL;

int pat_func(_ts_header *ptsHeader,unsigned char *buf)
{
    _pat_struct patStruct={0};

    patStruct.table_id = buf[0];
    patStruct.section_syntax_indicator = buf[1]>>7;
    patStruct.section_length = ((buf[1]&0xf)<<8|buf[2]);
    patStruct.transport_stream_id = (buf[3]<<8)|buf[4];
    patStruct.version_number = (buf[5]>>1)&0x1f;
    patStruct.current_next_indicator = buf[5]&0x01;
    patStruct.section_number = buf[6];
    patStruct.last_section_number = buf[7];
    if(patStruct.section_syntax_indicator!=1)
    {
        return -1;
    }
//    printf("\n[pat]\n");
//    printf("table_id=%d\n",patStruct.table_id);
//    printf("section_syntax_indicator=%d\n",patStruct.section_syntax_indicator);
//    printf("section_length=%d\n",patStruct.section_length);
//    printf("transport_stream_id=%d\n",patStruct.transport_stream_id);
//    printf("version_number=%d\n",patStruct.version_number);
//    printf("current_next_indicator=%d\n",patStruct.current_next_indicator);
//    printf("section_number=%d\n",patStruct.section_number);
//    printf("last_section_number=%d\n",patStruct.last_section_number);

    unsigned int program_number = 0;
    unsigned int nit_pid = 0;
    unsigned int pmt_pid = 0;
    unsigned char *ptmp = &buf[8];
    int max_data_len = patStruct.section_length - 5 - 4;
    for (int data_len = 0; data_len < max_data_len; ) {
        program_number = (ptmp[0]<<8)|ptmp[1];
        if(program_number==0xFFFF)
        {
            break;
        }
//        printf("program_number=%d\n",program_number);
        if(program_number == 0)
        {
            nit_pid = ((ptmp[2]&0x1f)<<8)|ptmp[3];
            if(pat_register_nit)
            {
                pat_register_nit(nit_pid);
            }
        }
        else
        {
            pmt_pid = ((ptmp[2]&0x1f)<<8)|ptmp[3];
            if(pat_register_pmt)
            {
                pat_register_pmt(pmt_pid);
            }
        }
        data_len+=4;
        ptmp+=4;
    }
    return 0;
}
int pat_env_init(pat_callback register_pmt,pat_callback register_nit)
{
    pat_register_pmt = register_pmt;
    pat_register_nit = register_nit;
    return 0;
}