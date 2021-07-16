#include"forward.h"

void forward(void* arg)
{
	int is_over_rec = 0;
	thread_num++;//�¿��̣߳��߳�������
	int No_x_thread;
	for (int i = 0; i < Max_Thread; i++)
	{
		if (rec_reading[i] == -1)
		{
			No_x_thread = i;//�����̷߳���һ����ǰû��ʹ�õ��̺߳�
			rec_reading[No_x_thread] = 0;
			break;
		}
	}
	//("��ǰ�߳���Ϊ��%d\n", thread_num);
	//�½�һ���������ںͷ��������ӵ�socket
	int local_forward_fd;
	local_forward_fd = socket(AF_INET, SOCK_DGRAM, 0);

	//���øղ����߳�������޸Ĺ���local��ַ�������UDP socket��һ��Ϊ0�Ķ˿ںţ���ϵͳ���ѡ�񣬱�֤�����ظ�
	bind(local_forward_fd, (SOCKADDR*)&local_socket_addr, sa_size);

	//�����������������Ǹ��Ȼ���Ҫ���ص�winϵͳ��ȥ�ĵ�ַ����Ҫ��port��
	SOCKADDR_IN local_back_addr = ((buf_and_size*)arg)->local;

	//׼��select����·ͬ��I/O
	struct timeval timout = { 2, 0 }; //����select��ʱʱ�䣬�ֱ�Ϊ�룬���룻
	struct fd_set fds;				//����һ��������select��fd�ļ���
	int maxfd = 0;					//���ֵ����Ϊ����������fd��ֵ��һ�������Ｏ����ֻ��һ��fd����local_forward_fd

	//���������д������İ���Ϣ������ȡ���ú����ľֲ�����data_to_send��
	REPLY_DATA data_to_send, reply;
	int len = ((buf_and_size*)arg)->len;
	for (int i = 0; i < len; i++)
	{
		data_to_send.packet[i] = ((buf_and_size*)arg)->buf[i];
	}
	data_to_send.len = len;
	get_domain_and_type(&data_to_send);
	char readable_domain[256];
	int temp_int = 12;
	int num = 0;
	getNAME(data_to_send.packet, data_to_send.len, &temp_int, readable_domain, 256, &num);
	//printf("\n\n readable_domain:%s \n\n\n", readable_domain);
	//data_to_send.packet������

	/*
	char requset_temp_domain[256];//�����www.ȥ��������һ���ԱȽ�

	if (readable_domain[0] == readable_domain[1] && readable_domain[1] == readable_domain[2] \
		&& readable_domain[2] == 'w' && readable_domain[3] == '.')
	{
		int i, j;
		for (i = 0, j = 4; readable_domain[j] != '\0'; i++, j++)//ȥ��www.
		{
			requset_temp_domain[i] = readable_domain[j];
		}
		requset_temp_domain[i] = readable_domain[j];
	}
	else strcpy(requset_temp_domain, readable_domain);
	//*/

	//�¼����ع��ܣ���ȡ����ѯ���е�domian֮����ip-domian���ձ��в�ѯ�Ƿ�������Ҫ���صĶ���
	for (int i = 0; i < ip_domain_num; i++)
	{
		if (strcmp(readable_domain, ip_domain[i].domain_name) == 0)  //||data_to_send.QTYPE==1)
		{
			if (strcmp("0.0.0.0", ip_domain[i].ip) == 0)
			{
				data_to_send.packet[3] = 0x83;

			}
			else if(data_to_send.QTYPE==0x1c)
			{
				break;
			}
			else
			{
				flags[No_x_thread] = i;
			}
			Answer answer;
			int pos = 12;

			//������Ӧ��
			data_to_send.packet[2] = 0x81;

			data_to_send.packet[6] = 0x00;
			data_to_send.packet[7] = 0x01;

			questionExtract(data_to_send.packet, data_to_send.len, &pos, &answer);
			data_to_send.packet[pos++] = 0xc0;
			data_to_send.packet[pos++] = 0x0c;

			data_to_send.packet[pos++] = 0x00;
			data_to_send.packet[pos++] = 0x01;
			
			data_to_send.packet[pos++] = 0x00;
			data_to_send.packet[pos++] = 0x01;
			
			data_to_send.packet[pos++] = 0x00;
			data_to_send.packet[pos++] = 0x00;
			data_to_send.packet[pos++] = 0xc0;
			data_to_send.packet[pos++] = 0x02;

			data_to_send.packet[pos++] = 0x00;
			data_to_send.packet[pos++] = 0x04;

			data_to_send.len = pos +4;

			char temp_ip[20];
			strcpy(temp_ip, ip_domain[flags[No_x_thread]].ip);
			char* ch;
			ch = strtok(temp_ip, ".");
			while (ch != NULL)
			{
				data_to_send.packet[pos++] = atoi(ch);
				//printf("\n++++++++ %c +++++++++++\n", atoi(ch));
				ch = strtok(NULL, ".");
			}
			debugOutput(data_to_send.packet, data_to_send.len);

			sendto(local_socket_fd, data_to_send.packet, data_to_send.len, 0, (SOCKADDR*)&local_back_addr, sa_size); //�����ݷ������ղŴ����������ĵ�ַ
			output_in_use = 0;
			rec_reading[No_x_thread] = -1;//�黹rec���߳���
			thread_num--;
			_endthread(); //�����߳�
			return;
		}
	}


	//��dns������ת���ղ��յ��Ĳ�ѯ��
	//������һ����������ö�дͬ��������
	//��Ϊǰ���ǣ����rec��¼������һ����������
	//����߳̾���ͬʱ���������飬�����ϲ���ͬʱ����ͬһ��Ԫ�أ�
	//����Ԫ��ֻ��β����ӣ����ᵼ��ǰ����ȷ���������±귢���ı�
	//����ʹ��

	while (rec_writing)Sleep(1);//������׼����ȡrec,ֻҪrecû��д�������Զ�

	rec_reading[No_x_thread] = 1;//�����̵߳Ķ���־��Ϊһ
	int i = check_in_cache(data_to_send);
	reply = rec[i];
	rec_reading[No_x_thread] = 0;//��rec��ɣ�������־��Ϊ0��
	int rec_timeout = -1;
	if (i != -1)//�������ҵ��˼�¼
	{
		rec_timeout = if_timeout(reply);
		if (rec_timeout == 0)//û��ʱ��������
		{
			rec_reading[No_x_thread] = -1;//�黹rec���߳���
			flags[No_x_thread] = -1;
			reply.packet[0] = data_to_send.packet[0];
			reply.packet[1] = data_to_send.packet[1];

			//�ڴ˷���DNS����
			sendto(local_socket_fd, reply.packet, reply.len, 0, (SOCKADDR*)&local_back_addr, sa_size); //�����ݷ������ղŴ����������ĵ�ַ
			while (output_in_use)Sleep(1);//��֤ͬʱֻ��һ���߳����������ֹ�������������
			output_in_use = 1;//��ס���������
			printf("����IP��");
			set_color(6);
			printf("%s", inet_ntoa(local_back_addr.sin_addr));
			restore_color();
			printf(" ��DNS��������\n");
			set_color(10);//��ɫ������������������ʾ��
			if (option != 0)printf("�������ҵ���¼->��¼δ��ʱ����������->��¼��������\n");
			restore_color();
			debugOutput(reply.packet, reply.len);
			output_in_use = 0;//�������
			thread_num--;
			_endthread(); //�����߳�
			return;
		}
	}

	//�ڴ˼���DNS����
	//������û�ҵ� ���� �ҵ��˵��ǳ�ʱ�ˣ����������������
	sendto(local_forward_fd, data_to_send.packet, len, 0, (SOCKADDR*)&dns_server_addr, sa_size);

	//select���������������select�൱��ֻ�Ǹ�recvfrom��ʱ������ֻ�õ���һ�Σ�
	//��Ҫ�����socket���÷���while����
	FD_ZERO(&fds);													 //�����Ҫ�����Ƿ��ܶ���socket���ϵĺ�
	FD_SET(local_forward_fd, &fds);									 //�򼯺������local_forward_fd�ĺ�
	maxfd = maxfd > local_forward_fd ? maxfd : local_forward_fd + 1; //�������fdֵΪ����������fd�����ֵ��һ
	switch (select(maxfd, &fds, NULL, NULL, &timout)) //��ѯ��ֱ����ʱ
	/*
	����һ��select�����������������һ��������int���ͣ������м���������fd�����ֵ��һ

	�ڶ����ĸ���������Ϊstruct fd_set����ͷ����winsock2�У�
	�ֱ���Ҫ�����Ƿ��ܶ�fd�ļ��ϣ��Ƿ���д��fd�ļ��ϣ��Ƿ�����fd�ļ���

	�����������������Ϊstruct timeval* ����ͷ����winsock2�У�������ͺ���֮��select��ʱ
	�����������Ϊ0��select�����̷��أ�����Ϊnull��select��������֪����⵽��д�ɶ������Ϊֹ

	select��������ֵΪ����ֵ����ʾ��⵽��socket�ɶ����д�����
	��ֵ����ʾselect�����������
	0����ʱ�����ٴμ������������FD_ZERO��FD_SET
	*/
	{
	case -1:
		break; //���ùܵ����
	case 0:
		break;								  //���ù�
	default:								  //��socket�ɶ����д�����
		if (FD_ISSET(local_forward_fd, &fds)) //FD_ISSET���ú��ж�ĳ��socket�Ƿ�select�������Ϊ�ɶ����д�����
		{
			len = recvfrom(local_forward_fd, reply.packet, 512, 0, NULL, 0); //���ﲻ��Ҫ֪��dns�������ĵ�ַɶ�ģ���Ϊ��֪���ˣ���ֱ��дnull��0
			//if (flags[No_x_thread] != -1)
			//{
			//	int f = flags[No_x_thread];
			//	Answer answer;
			//	int pos = 12;
			//	if (f == -2)
			//	{
			//		printf("\n STOP!!!!!! f:%d \n",f);
			//		reply.packet[3] = 0x83;
			//	}
			//	else
			//	{
			//		printf("\n CHAGE!!!!!!!!! f:%d \n", f);
			//		questionExtract(reply.packet, len, &pos, &answer);
			//		if (reply.packet[3] == 0x83)
			//		{
			//			reply.packet[3] = 0x80;
			//			int tt_pos = pos;
			//			reply.packet[]
			//		}
			//		printf("pos:%d\tlen:%d\n", pos, len);
			//		while (pos < len)
			//		{
			//			printf("pos:%d\tlen:%d\n", pos, len);
			//			resourceExtract(reply.packet, len, &pos, &answer, f);/**/
			//		}
			//	}
			//	printf("forward.c \n");
			//	debugOutput(reply.packet, len);
			//}
			if (len > 0)
			{
				//����ͻ�������Է����������ݣ�bufΪ���飬lenΪ��Ч����
				reply.len = len;
				sendto(local_socket_fd, reply.packet, reply.len, 0, (SOCKADDR*)&local_back_addr, sa_size); //�����ݷ������ղŴ����������ĵ�ַ
				//reply.packet����Ӧ��

				extract_info_from_reply(&reply);
				if (reply.ttl != -1)//����ttl����Ž��뻺�棬�����ڵĲ�����
				{
					//ͬ�ϣ�����ȡ���߳�ͬ��
					///*
					while (1)//������׼���޸�rec���ȼ��rec�Ƿ�����ʹ����
					{
						int reading = 0;
						for (int i = 0; i < Max_Thread; i++)//�����û�߳��ڶ�
						{
							if (rec_reading[i] == 1)//ֻҪ��һ���߳��ڶ����Ͳ���д
							{
								reading = 1;
								break;
							}
						}
						if (!reading && !rec_writing && !file_in_use)break;//û�ж�Ҳû��дҲû���ڸ����ļ�����������дrec
						Sleep(1);
					}
					//*/
					//д�ļ�ʱ��֤��дrec���ɶ�rec
					//while (file_in_use)Sleep(1);//�ļ�����д������д
					rec_writing = 1;//��סдrec����ʾ���߳������޸�rec
					if (rec_timeout == 1)
					{
						rec[i] = reply;//��¼i��ʱ�ˣ����»�õļ�¼reply�滻��0¼i
						rec_changed++;//�ı�ļ�¼��������
					}
					else//����û�������¼��������,
					{
						if (rec_num < MAX_REC)//û�г�����¼����
						{
							rec[rec_num++] = reply;//������¼���Ҽ�¼��������
							rec_changed++;//�ı�ļ�¼��������
						}
						else is_over_rec = 1;
					}
					rec_writing = 0;//����дrec
					if (rec_changed >= 10)//����ʮ�����ϵļ�¼�ı���
					{
						while (file_in_use)Sleep(1);//�ļ�����д������д
						file_in_use = 1;//��ס�ļ�����ֹͬʱ�޸�
						rec_reading[No_x_thread] = 1;//��סrec����ֹд��ʱ����
						rec_changed = 0;//���¼����޸ĵļ�¼��
						FILE* fp = fopen(cache_file, "wb");
						for (int i = 0; i < rec_num; i++)
						{
							store_info_from_reply(rec[i], fp);//��д�ļ�
						}
						fclose(fp);
						file_in_use = 0;//�����ļ�
						rec_reading[No_x_thread] = 0;//����rec
					}
				}
				while (output_in_use)Sleep(1);
				output_in_use = 1;
				printf("����IP��");
				set_color(6);
				printf("%s", inet_ntoa(local_back_addr.sin_addr));
				restore_color();
				printf(" ��DNS��������\n");
				set_color(12);//�ۺ�ɫ�����������ļ�¼��ʾ
				if (rec_timeout != -1 && option != 0 && reply.ttl != -1)printf("�������ҵ���¼->��¼�ѳ�ʱ->��ʱ��¼���³ɹ�->���ذ���������\n");
				else if (!is_over_rec && option != 0 && reply.ttl != -1)printf("������δ�ҵ���¼->�½���¼�ɹ�����ǰ��¼������:%d->���ذ���������\n", rec_num);
				else if (!is_over_rec && option != 0 && reply.ttl == -1)printf("������δ�ҵ���¼->���ش����������󲻷�->���ذ���������\n");
				else if (is_over_rec && option != 0) printf("������δ�ҵ���¼->���������������ޣ�����������¼->��¼��������\n");
				restore_color();
				debugOutput(reply.packet, reply.len);
				output_in_use = 0;
			}
		}
	}
	//printf("time: %d", times);
	rec_reading[No_x_thread] = -1;//�黹�߳�rec������
	flags[No_x_thread] = -1;
	thread_num--;
	_endthread(); //�����߳�
}