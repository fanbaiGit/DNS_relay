#include"debugOutput.h"

void typeOutput(UINT16 TYPE)
{
	if (TYPE == 1)
		printf("(A)\t");
	else if (TYPE == 5)
		printf("(CNAME)\t");
	else if (TYPE == 12)
		printf("(PTR)\t");
	else if (TYPE == 15)
		printf("(MX)\t");
	else if (TYPE == 13)
		printf("(HINFO)\t");
	else if (TYPE == 2)
		printf("(NS )\t");
	else if (TYPE == 28)
		printf("(AAAA)\t");
	else
		printf("\t");
}

void headerExtract(char* packet, int len, Answer* answer)
{
	int pos = 0;
	answer->ID = getUINT16(packet, len, &pos);
	answer->QR = getQR(packet, len);
	if (answer->QR == 1)
	{
		answer->RCODE = getRCODE(packet, len);
	}
}

void headerOutput(Answer* answer)
{
	if (option == 2)
	{
		split_line();
		set_color(6);//���ɫ���ͷ����ʾ
		printf("Header Section\n");
		restore_color();
	}
	printf("ID:0x%x\n", answer->ID);
	if (option == 2)
	{
		printf("QR:%x", answer->QR);
		if (answer->QR == 0)
			printf("����ѯ����\t");
		else
			printf("����Ӧ����\t");

		if (answer->QR == 1)
		{
			printf("RCODE:%x", answer->RCODE);
			if (answer->RCODE == 3)
				printf("�����ֲ��");
			else if (answer->RCODE == 1)
				printf("��û�в��");
		}
			printf("\n");
	}
}

void questionExtract(char* packet, int len, int* pos, Answer* answer)
{
	int tmp = *pos;
	int num = 0;
	getNAME(packet, len, &tmp, answer->QNAME, 256, &num);
	//printf("questionExtractL:%s\n", name);
	answer->QTYPE = getUINT16(packet, len, &tmp);
	answer->QCLASS = getUINT16(packet, len, &tmp);
	*pos = tmp;
}

void questionOutput(Answer* answer)
{
	if (option == 2)
	{
		split_line();
		set_color(6);//���ɫ���ͷ����ʾ
		printf("Question Section\n");
		restore_color();
	}
	printf("QNAME:%s\t", answer->QNAME);
	if (option == 1)printf("\n");
	if (option == 2)
	{
		printf("QTYPE:%x", answer->QTYPE);
		typeOutput(answer->QTYPE);
		printf("QCLASS:%x", answer->QCLASS);
		if (answer->QCLASS == 1)
			printf("(IN)");
		printf("\n");
	}
}

void resourceExtract(char* packet, int len, int* pos, Answer* answer,int f)
{
	int tmp = *pos;
	int num = 0;
	//answer->NAME = getUINT16(packet, len, &tmp);
	//printf("NAME pos:%d \n",tmp);
	getNAME(packet, len, &tmp, answer->NAME, 256, &num);
	answer->TYPE = getUINT16(packet, len, &tmp);
	//printf("\n\n sadasdasdsa %d \n\n", is_flag);
	//printf("QTYPE:%d \tTYPE:%d \tf:%d \n\n", answer->QTYPE, answer->TYPE,f);
	if (f != -1 && answer->QTYPE == answer->TYPE)
	{
		times++;
		int t_tmp = tmp + 8;
		char temp_ip[20];
		strcpy(temp_ip, ip_domain[f].ip);
		char* ch;
		ch = strtok(temp_ip, ".");
		while (ch != NULL)
		{
			packet[t_tmp++] = atoi(ch);
			//printf("\n++++++++ %c +++++++++++\n", atoi(ch));
			ch = strtok(NULL, ".");
		}
		tmp += 6;
		unsigned int uchar = getUINT16(packet, len, &tmp);//pos+=2
		answer->RDLENGTH = uchar;
		tmp += answer->RDLENGTH;
	}
	else
	{
		if (answer->QTYPE == answer->TYPE)
		{
			answer->CLASS = getUINT16(packet, len, &tmp);//pos+=2
			answer->TTL = getUINT32(packet, len, &tmp);//pos+=4
			unsigned int uchar = getUINT16(packet, len, &tmp);//pos+=2
			answer->RDLENGTH = uchar;
			//answer->RDLENGTH = getUINT16(packet, len, &tmp);
			if (f == -1)
			{
				if (answer->TYPE == 1 || answer->TYPE == 28)
				{
					int j = 0;
					for (int k = 0; k < answer->RDLENGTH; k++)
					{
						unsigned char uchar = packet[tmp++] & 0xff;
						answer->RDATA[j++] = uchar;
					}
				}
				else
				{
					//printf("RDATA pos:%d \n",tmp);
					num = 0;
					getNAME(packet, len, &tmp, answer->RDATA, answer->RDLENGTH, &num);
				}/**/
			}
		}
		else
		{
			tmp += 6;
			unsigned int uchar = getUINT16(packet, len, &tmp);//pos+=2
			answer->RDLENGTH = uchar;
			tmp += answer->RDLENGTH;
		}/**/
	}
	*pos = tmp;
	//printf("End_pos:%d\n", *pos);
}

void resourceOutput(Answer* answer, int* num)
{
	int n = *num;
	printf("NO:%d \t", n++);
	*num = n;
	if (answer->TYPE == answer->QTYPE)
	{
		//printf("NAME_point:0x%x \t", answer->NAME);
		printf("NAME:%s \n", answer->NAME);
		printf("TYPE:%x \t", answer->TYPE);
		typeOutput(answer->TYPE);
		printf("CLASS:%x ", answer->CLASS);
		if (answer->CLASS == 1)
			printf("(IN)");
		printf("\t");
		printf("TTL:(UINT32)0x%x (int)%d\n", answer->TTL, answer->TTL);
		printf("RDLENGTH:%d\t", answer->RDLENGTH);
		if (answer->TYPE == 1 || answer->TYPE == 28)
		{
			printf("IP ADRESS:");
			printf("%d", answer->RDATA[0] & 0xff);
			for (int k = 1; k < answer->RDLENGTH; k++)
			{
				if (answer->TYPE == 1)
					printf(".");
				else
				{
					printf(":");
				}
				printf("%d", answer->RDATA[k] & 0xff);
			}
		}
		else
		{
			printf("RDATA:%s\n", answer->RDATA);
			answer->RDATA[0] = '\0';
		}
		printf("\n");

	}
	else
	{
		printf("TYPE MISMATCH!!!\n");
	}/**/
}

void debugOutput(char* packet, int len)
{
	int l = 0;
	int pos = 0;//posָ��NAME����Ǹ��ֽ�����λ��
	char name[256] = "";//��������
	Answer answer;
	if (option == 0)
	{
		return;
	}
	split_line();
	answer.TIME = getTime();
	printf("NOW TIME:%ld \t LEN:%d \n", answer.TIME,len);
	//printf("\nExtracting Header Section.....\n");
	headerExtract(packet, len, &answer);
	headerOutput(&answer);
	//printf("\nExtracting Question Section.....\n");
	pos = 12;
	questionExtract(packet, len, &pos, &answer);
	//printf("\nanswwer.Qname:%s\n\n", name);
	questionOutput(&answer);
	if (answer.QR == 1 && option == 2 && answer.RCODE != 3 && answer.QTYPE != '\x1c')//�˴�����ֻ������Ӧ�����н������
	{
		//printf("Extracting Resource Section.....\n");
		split_line();
		set_color(6);//���ɫ���ͷ����ʾ
		printf("Resource Section\n\n");
		restore_color();
		int num = 1;
		while (pos < len)
		{
			set_color(14);//�ư�ɫ�ָ�start_pos
			printf("Start_pos:%d\n", pos);
			restore_color();
			resourceExtract(packet, len, &pos, &answer,-1);
			resourceOutput(&answer, &num);
			set_color(14);//�ư�ɫ�ָ�start_pos
			printf("End_pos:%d\n", pos);
			restore_color();
			set_color(7);//��ɫ������ָ���
			printf("******************************************");
			restore_color();
			printf("\n");
		}
	}
	else if(answer.QTYPE == '\x1c'&&option==2)
	{
		split_line();
		set_color(6);//���ɫ���ͷ����ʾ
		printf("Resource Section\n\n");
		restore_color();
		printf("����һ��IPV6�������ǲ�����ˡ� \n");
	}
	split_line();
	set_color(79);//��װ����������������
	printf("Extract done.");
	restore_color();
	printf("\n\n");
}

void split_line()
{
	set_color(9);//��ɫ����ָ���
	printf("-----------------------------------------------------\n");
	restore_color();
}
