#pragma once
#include"globals.h"

void getRec(FILE* fp);//���ļ���ȡ���м�¼

int check_in_cache(REPLY_DATA data_to_search);//������¼�Ƿ���� ���ڷ����±� ������ȥ������Ѱ�Ҳ�д���ļ�������-1 

int if_timeout(REPLY_DATA reply);//����Ƿ�ʱ ��ʱ����1 δ��ʱ����0��tΪ��rec�е��±�

void store_info_from_reply(REPLY_DATA reply, FILE* fp);
