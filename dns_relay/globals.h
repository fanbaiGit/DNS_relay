#pragma once
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <Windows.h> //���ͷ���ܷ���winsock2�Ǹ�ͷǰ�棬��Ȼ�ᱨ��
#include <WS2tcpip.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib") //������������ʹ��socket

#define MAX_REC 10000
#define Max_Thread 5

typedef struct buf_and_size //���ڶ��̴߳��������ʱ�Ľṹ��
{
	unsigned char* buf;
	int len;
	SOCKADDR_IN local;
} buf_and_size;

typedef struct
{
	UINT16 ID;	 //�ɿͻ��������ò��ɷ��������ؽ�����ͻ�����ͨ������ȷ����Ӧ���ѯ�Ƿ�ƥ��
	UINT8 QR;	 //0 ��ʾ��ѯ����1 1 ��ʾ��Ӧ��
	UINT8 RCODE; //��Ӧ�� (Response coded) ����������Ӧ��. ֵΪ 0( û�в��),ֵΪ3 ��ʾ���ֲ����Ȩ�����ַ��������أ���ʾ�ڲ�ѯ��ָ������������
	//UINT16 NAME; //��Դ��¼�������ֵ�ָ��
	char NAME[256];
	char QNAME[256]; //���ⲿ������
	char RDATA[256];
	UINT16 QTYPE;
	UINT16 TYPE;
	UINT16 QCLASS;
	UINT16 CLASS;
	UINT32 TTL;
	int RDLENGTH;
	long TIME;
	/* data */
} Answer;
//������Ҫ���������ĸ�����domainname��type������Ϣ��ttl
typedef struct
{
	UINT16 QTYPE;
	char query_type[2];
	int ttl;
	long time_stamp;
	int len;
	int domain_len;
	char domain_name[255];
	char packet[512];

} REPLY_DATA;

typedef struct
{
	char ip[20];//��ip-�������ձ��ж�ȡ��ip
	char domain_name[256];//��ip-�������ձ��ж�ȡ������
}IP_DOMAIN;

extern char LOCAL_IP[20];//����IP��ַ
extern char DNS_SERVER_IP[20];//Զ��dns��������ַ

extern char cache_file[512];//�����ļ�·��

extern int local_socket_fd;//���ڼ�������win���м�������dns�������socket

extern int sa_size ;//�ṹ���С

extern int thread_num;//ȫ�����߳���

extern int option;//�������0Ϊ�������1Ϊ���������2Ϊȫ���

extern int option_is_set;//����������Ĳ����Ƿ���option���óɹ�
extern int cache_file_is_set;//����������Ĳ����Ƿ���cache��·�����óɹ�
extern int dns_server_ip_is_set;//���������õĲ����Ƿ���dns��������ip���óɹ�

extern REPLY_DATA rec[MAX_REC];//��פ�ڴ��еĴ������м�¼������
extern int rec_num;//�ܼ�¼��

extern int rec_changed;//changed���ļ��м�¼�޸���������5�򽫳�פ����ļ�¼д���ļ��г�ʱ�䱣���ļ�  

extern int file_in_use;//�߳�ͬ��������ʾ��ǰ�����ļ��Ƿ������ڱ�ʹ�ã�д��
extern int rec_writing;//�߳�ͬ����
extern int rec_reading[Max_Thread];//�߳�ͬ��������ʾrec�Ƿ��ڶ���д
//����д���������ڣ�дֻ��ͬʱ��һ���߳���д��������ͬʱ�ܶ��߳�һ���
//���̶߳�ʱ����д�����߳�дʱ���ܶ�
extern int output_in_use;//�߳�ͬ��������ʾ��ǰ�������Ƿ����߳��������

extern int sys_color;//����������߳���,ϵͳ�����ɫ��Ĭ��15Ϊ��
extern HANDLE color_out;//����ϵͳ�����ɫ��handle
extern SOCKADDR_IN local_socket_addr, dns_server_addr; //ÿ���̶߳�����

extern int ip_domain_num;//ip�������ձ�ĸ���
extern IP_DOMAIN ip_domain[100];//IP�������ձ�

extern int color_in_use;//��������ɫ�޸ĵ��߳�ͬ��

extern flags[Max_Thread];

extern times;

void set_color(int i);//Ϊ�޸���ɫ����߳�ͬ��
void restore_color();