#include <stdio.h>
#include "platform.h"
#include "pat.h"
#include "pmt.h"
#include "avpid.h"
#include "ts_header.h"
#include "sdt.h"

int cat_func(_ts_header *ptsHeader,unsigned char *buf)
{
    printf("[cat]\n");
    return 0;
}
int tsdt_func(_ts_header *ptsHeader,unsigned char *buf)
{
    printf("[tsdt]\n");
    return 0;
}
//int sdt_func(unsigned char *buf)
//{
//    printf("[sdt]\n");
//    return 0;
//}
int nit_func(_ts_header *ptsHeader,unsigned char *buf)
{
    printf("[nit]\n");
    return 0;
}
int eit_func(_ts_header *ptsHeader,unsigned char *buf)
{
    printf("[eit]\n");
    return 0;
}
int pcr_func(_ts_header *ptsHeader,unsigned char *buf)
{
    printf("[pcr]\n");
    return 0;
}
int null_func(_ts_header *ptsHeader,unsigned char *buf)
{
//    printf("[null]\n");
    return 0;
}
#define PID_PAT 0
#define PID_CAT 1
#define PID_TSDT 2
#define PID_SDT  0x11
#define PID_EIT  0x12
#define PID_NULL  0x1FFF

_pid_base_t func_list[8192]={
        [PID_PAT].cb=pat_func,
        [PID_PAT].packet_type=PACKET_SECTION,

        [PID_CAT].cb=cat_func,
        [PID_CAT].packet_type=PACKET_SECTION,

        [PID_TSDT].cb=tsdt_func,
        [PID_TSDT].packet_type=PACKET_SECTION,

//        [PID_SDT].cb=sdt_func,
//        [PID_SDT].packet_type=PACKET_SECTION,

        [PID_EIT].cb=eit_func,
        [PID_EIT].packet_type=PACKET_SECTION,

        [PID_NULL].cb=null_func,
        [PID_NULL].packet_type=PACKET_SECTION,
};

int avpid_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    printf("[avpid]:register %d\n",pid);
    func_list[pid].cb = avpid_func;
    func_list[pid].packet_type = PACKET_PES;
    return 0;
}
int vpid_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    printf("[vpid]:register %d\n",pid);
    func_list[pid].cb = avpid_func;
    func_list[pid].packet_type = PACKET_PES;
    return 0;
}
int apid_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    printf("[apid]:register %d\n",pid);
    func_list[pid].cb = avpid_func;
    func_list[pid].packet_type = PACKET_PES;
    return 0;
}
int nit_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    func_list[pid].cb = nit_func;
    func_list[pid].packet_type = PACKET_SECTION;
    printf("[nit]:register %d\n",pid);
    return 0;
}
int pmt_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    func_list[pid].cb = pmt_func;
    func_list[pid].packet_type = PACKET_SECTION;
    printf("[pmt]:register %d\n",pid);
    return 0;
}

int pcr_register(unsigned int pid)
{
    if(func_list[pid].cb)
    {
        return 0;
    }
    func_list[pid].cb = pcr_func;
    func_list[pid].packet_type = PACKET_PCR;
    printf("[pcr]:register %d\n",pid);
    return 0;
}

int pid[8192]={0};

int base_check(void)
{
    printf("char=%d\n",sizeof(char));
    printf("unsigned char=%d\n",sizeof(unsigned char));
    printf("short=%d\n",sizeof(short));
    printf("unsigned short=%d\n",sizeof(unsigned short));
    printf("int=%d\n",sizeof(int));
    printf("unsigned int=%d\n",sizeof(unsigned int));
    printf("long=%d\n",sizeof(long));
    printf("unsigned long=%d\n",sizeof(unsigned long));
    printf("long long=%d\n",sizeof(long long));
    printf("unsigned long long=%d\n",sizeof(unsigned long long));
}


int main() {
    base_check();
    pat_env_init(pmt_register,nit_register);
    pmt_env_init(vpid_register,apid_register,pcr_register);
    FILE *pfile_ts=NULL;
//    pfile_ts = fopen("D:\\ts\\3. Baedang_3M 1ch_SPTS.ts","rb");
    pfile_ts = fopen("D:\\ts\\spider.ts","rb");
    if(pfile_ts == NULL)
    {
        printf("file open err\n");
        return 0;
    }
    unsigned char buf[2048]={0};
    int len=0;
    fseek(pfile_ts, 0, SEEK_SET);
    do
    {
        len = fread(buf,1,7*188,pfile_ts);
//        printf("read=%d\n",len);
        for (int i = 0; i < 7 ; ++i) {
            if(buf[i*188] != 0x47)
            {
                printf("error!\n");
                continue;
            }
            parse_ts_header(&buf[i*188],func_list);
        }
    } while (0!=len);
    printf("Hello, World!\n");
    fclose(pfile_ts);
    pfile_ts=NULL;
    return 0;
}
