#include"globals.h"

char LOCAL_IP[20] = "";//���ݱ���IP����
char DNS_SERVER_IP[20] = "";
int local_socket_fd;
int sa_size = sizeof(SOCKADDR);

int file_in_use = 0, thread_num = 0;

int option = 0;//�������0Ϊ�������1Ϊ���������2Ϊȫ���

int option_is_set = 0, cache_file_is_set = 0, dns_server_ip_is_set = 0;

REPLY_DATA rec[MAX_REC];

int rec_num = 0, rec_changed = 0;//changed���ļ��м�¼�޸���������5���ض��ļ�  

int rec_writing = 0, rec_reading[Max_Thread];//�߳�ͬ��rec�Ƿ��ڶ���д
//����д���������ڣ�дֻ��ͬʱ��һ���߳���д��������ͬʱ�ܶ��߳�һ���
//���̶߳�ʱ����д�����߳�дʱ���ܶ�

int output_in_use = 0, sys_color = 15;//����������߳���,ϵͳ�����ɫ��Ĭ��15Ϊ��

HANDLE color_out;//����ϵͳ�����ɫ��handle

char cache_file[512] = "dnsrelay.txt";

SOCKADDR_IN local_socket_addr, dns_server_addr; //ȫ�ֱ�������ÿ���̶߳����õ�

int ip_domain_num=0;//ip�������ձ�ĸ���
IP_DOMAIN ip_domain[100];//IP�������ձ�

int color_in_use;//��������ɫ�޸ĵ��߳�ͬ��

int flags[Max_Thread];

int times = 0;

void set_color(int i)//Ϊ�޸���ɫ����߳�ͬ��
{
	while (color_in_use)Sleep(1);
	color_in_use = 1;
	SetConsoleTextAttribute(color_out, i);
}
void restore_color()
{
	SetConsoleTextAttribute(color_out, sys_color);
	color_in_use = 0;
}

