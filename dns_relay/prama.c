#include"parma.h"

int judge_and_set_parma(char** varg, int pos)//���������в����ĺ�����posΪҪ��varg�����еڼ��������ñ���
{
	int is_success = 1;//�����в��������Ƿ���ȷ
	if (varg[pos][0] == '-' && !option_is_set)//�������ַ����ĵ�һ��Ϊ�ַ�'-'�����ҵ��Լ������������û���趨��
	{
		if (strcmp(varg[pos], "-d") == 0)option = 1;
		else if (strcmp(varg[pos], "-dd") == 0)option = 2;
		else option = 0;
		option_is_set = 1;
	}
	else if (isdigit(varg[pos][0]) && !dns_server_ip_is_set)//�����һ���ַ�Ϊ���֣�����dns ip��û���趨��
	{
		strcpy(DNS_SERVER_IP, varg[pos]);
		dns_server_ip_is_set = 1;
	}
	else if (isalpha(varg[pos][0]) && varg[pos][1] == ':' && varg[pos][2] == '\\' && !cache_file_is_set)//ͬ�����·��û���趨��
	{
		strcpy(cache_file, varg[pos]);
		cache_file_is_set = 1;
	}
	else is_success = 0;//������ϽԲ��ǻ��߳����趨һ���Ѿ��趨���Ĳ������������ظ�����������0����ʾ����ʧ��
	return is_success;
}

int read_ip_domain()
{
	FILE* fp=fopen("ip_domain.txt","r");
	if (fp == NULL)
	{
		printf("�޷���ip-�������ձ���վ���ع����޷�������\n");
		return;
	}
	else
	{
		printf("��ȡip-�������ձ�ɹ�����վ���ع��ܿ����ɹ���\n");
		while (!feof(fp))
		{
			fscanf(fp, "%s %s", ip_domain[ip_domain_num].ip, ip_domain[ip_domain_num].domain_name);
			ip_domain_num++;
		}
		ip_domain_num--;
	}
}
