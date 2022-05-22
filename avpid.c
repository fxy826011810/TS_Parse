//
// Created by fan on 2021/5/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avpid.h"
typedef struct{
    unsigned char *pbuffer;
    unsigned int buffer_size;
    unsigned int offset;
    unsigned char *pname;
    FILE *pfile;
}_pes_packet_t;

_pes_packet_t pes_packet[8192] = {0};
#define filter_start(pid) if(pid==256)
int avpid_func(_ts_header *ptsHeader,unsigned char *buf)
{
//    printf("[avpid]\n");
    _pes_packet_t *pav_packet = &pes_packet[ptsHeader->pid];
    if(ptsHeader->payload_unit_start_indicator == 0)
    {
        if(pav_packet->pbuffer)
        {
            if((pav_packet->offset + 188 - ptsHeader->buffer_offset) > pav_packet->buffer_size)
            {
                pav_packet->pbuffer = realloc(pav_packet->pbuffer,(pav_packet->offset + 188 - ptsHeader->buffer_offset));
                pav_packet->buffer_size = (pav_packet->offset + 188 - ptsHeader->buffer_offset);
            }
            memcpy(&pav_packet->pbuffer[pav_packet->offset],buf,188 - ptsHeader->buffer_offset);
            pav_packet->offset += (188 - ptsHeader->buffer_offset);
//            filter_start(ptsHeader->pid)
//            printf("pid=%d data_len=%d\n",ptsHeader->pid,pav_packet->offset);
        }
        return 0;
    }

    if(ptsHeader->payload_unit_start_indicator != 1)
    {
        return -1;
    }

    if(pav_packet->offset && pav_packet->pbuffer)
    {
        if(pav_packet->pfile)
        {
            fwrite(pav_packet->pbuffer,1,pav_packet->offset,pav_packet->pfile);
//            printf("pid=%d data_len=%d\n",ptsHeader->pid,pav_packet->offset);
        }
//        printf("pid=%d data_len=%d\n",ptsHeader->pid,pav_packet->offset);
//        free(pav_packet->pbuffer);
        memset(pav_packet->pbuffer,0,pav_packet->buffer_size);
//        pav_packet->pbuffer = NULL;
//        pav_packet->buffer_size = 0;
        pav_packet->offset = 0;
    }

    _pes_header pesHeader={0};
    pesHeader.packet_start_code_prefix = (buf[0]<<16)|(buf[1]<<8)|buf[2];
    if(pesHeader.packet_start_code_prefix != 0x01)
    {
        return -1;
    }
    pesHeader.stream_id = buf[3];
    pesHeader.PES_packet_length = (buf[4]<<8)|buf[5];
    //printf("stream_id=%d\n",pesHeader.stream_id);
    //
    if(pesHeader.PES_packet_length == 0)
    {
        pesHeader.PES_packet_length = 64*1024;
    }
    filter_start(ptsHeader->pid)
//    printf("pid=%d pes_packet_length=%d\n",ptsHeader->pid,pesHeader.PES_packet_length);
    if(pav_packet->pbuffer == NULL && pav_packet->buffer_size == 0 && pesHeader.PES_packet_length)
    {
        pav_packet->pbuffer = malloc(pesHeader.PES_packet_length);
        pav_packet->buffer_size = pesHeader.PES_packet_length;
        memset(pav_packet->pbuffer,0,pav_packet->buffer_size);
        if(pav_packet->pname == NULL)
        {
            pav_packet->pname = malloc(128);
            memset(pav_packet->pname,0,128);
            sprintf(pav_packet->pname,"es_%d",ptsHeader->pid);
            pav_packet->pfile = fopen(pav_packet->pname,"wb+");
        }
    }

    int pes_header_offset = 0;
    if( pesHeader.stream_id!=0xBC    //program_stream_map
       && pesHeader.stream_id!=0XBE  //padding_stream
        && pesHeader.stream_id!=0XBF //private_stream_2
        && pesHeader.stream_id!=0xF0 //ECM
        && pesHeader.stream_id!=0xF1 //EMM
        && pesHeader.stream_id!=0xFF //program_stream_directory
        && pesHeader.stream_id!=0xF2 //DSMCC_stream
        && pesHeader.stream_id!=0xF8 //ITU-T Rec. H.222.1 type E stream
        )
    {
        pesHeader.PES_scrambling_control = (buf[6]>>4)&0x3;
        pesHeader.PES_priority = (buf[6]>>3)&0x1;
        pesHeader.data_alignment_indicator = (buf[6]>>2)&0x1;
        pesHeader.copyright = (buf[6]>>1)&0x1;
        pesHeader.original_or_copy = buf[6]&0x1;
        pesHeader.PTS_DTS_flags = (buf[7]>>6)&0x3;
        pesHeader.ESCR_flag = (buf[7]>>5)&0x1;
        pesHeader.ES_rate_flag = (buf[7]>>4)&0x1;
        pesHeader.DSM_trick_mode_flag = (buf[7]>>3)&0x1;
        pesHeader.additional_copy_info_flag = (buf[7]>>2)&0x1;
        pesHeader.PES_CRC_flag = (buf[7]>>1)&0x1;
        pesHeader.PES_extension_flag = buf[7]&0x1;
        pesHeader.PES_header_data_length = buf[8];
        pes_header_offset += 9;
        if(pesHeader.PTS_DTS_flags == 2)
        {
            pes_header_offset += 5;
        }
        else if(pesHeader.PTS_DTS_flags == 3)
        {
            pes_header_offset += 10;
        }
        if(pesHeader.ESCR_flag == 1)
        {
            pes_header_offset += 6;
        }
        if(pesHeader.ES_rate_flag == 1)
        {
            pes_header_offset += 3;
        }
        if(pesHeader.DSM_trick_mode_flag == 1)
        {
            pes_header_offset += 1;
        }
        if(pesHeader.additional_copy_info_flag == 1)
        {
            pes_header_offset += 1;
        }
        if(pesHeader.PES_CRC_flag)
        {
            pes_header_offset += 2;
        }
        if(pesHeader.PES_extension_flag == 1)
        {
            //pes_header_offset += 1;
            printf("test3\n");
        }
        int stuffing_byte = buf[pes_header_offset];
        int PES_packet_data_byte = buf[pes_header_offset + 1];

        //暂时这么干
        pes_header_offset = 9 + pesHeader.PES_header_data_length;
        memcpy(&pav_packet->pbuffer[pav_packet->offset],&buf[pes_header_offset],188 - ptsHeader->buffer_offset - pes_header_offset);
        pav_packet->offset += (188 - ptsHeader->buffer_offset - pes_header_offset);
    }
    else if ( pesHeader.stream_id == 0xBC //program_stream_map
              || pesHeader.stream_id == 0XBF //private_stream_2
              || pesHeader.stream_id == 0xF0 //ECM
              || pesHeader.stream_id == 0xF1 //EMM
              || pesHeader.stream_id == 0xFF //program_stream_directory
              || pesHeader.stream_id == 0xF2 //DSMCC_stream
              || pesHeader.stream_id == 0xF8//"ITU-T Rec. H.222.1 type E stream"
              )
    {
        printf("test1\n");
        //PES_packet_data_byte
    }
    else if ( pesHeader.stream_id == 0XBE/*padding_stream*/)
    {
        //padding_byte
        printf("test2\n");
    }
    return 0;
}
