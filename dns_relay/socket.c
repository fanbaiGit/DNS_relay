#include"globals.h"
#include"forward.h"
#include"parma.h"

int main(int carg,char **varg)//varg[0]�̶�Ϊ�ǳ���ִ�е�λ�ã�varg[1]֮���������Ĳ���
{
	color_out = GetStdHandle(STD_OUTPUT_HANDLE);//������ɫ��handel
	memset(rec_reading, -1, sizeof(rec_reading));//���Ƚ����е��̶߳���Ϊ��һ
	memset(flags, -1, sizeof(flags));
	int is_set_parma_success = 1;//�����в��������Ƿ���ȷ
	for (int i = 1; i < carg; i++)//���뼸��������ѭ������
	{
		is_set_parma_success &= judge_and_set_parma(varg, i);//ÿ����varg�����е���һ���ַ������ò���
		//�������&=���������ÿ���趨��������ȷ������һ���趨����ȷ�����ʾ��������������򱨴�
	}
	if (!is_set_parma_success)
	{
		printf("��������ȷ�Ĳ�����ʽ���磺\ndnsrelay [-d | -dd] [dns-server-ipaddr] [filename]");
		return 0;
	}
	split_line();
	set_color(4);//��ɫ�������ģʽ
	if (option == 1)
		printf("�����������ģʽ\n");
	else if (option == 2)
		printf("ȫ�������ģʽ\n");
	else
		printf("���������ģʽ\n");
	printf("�����ļ�·����%s\n", cache_file);
	printf("DNS������IP��ַ��%s\n", DNS_SERVER_IP);
	read_ip_domain();//���ع���
	restore_color();
	split_line();
	//winƽ̨�£�socket��̵ı��벽�裬��linux�¿�ʡ��
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); //����socket��̵Ķ�̬dll

	//�½�һudp socket�����ڽ��մ�win���Ĳ�ѯ��
	local_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	//���ñ��ؼ���socket����Ϣ
	local_socket_addr.sin_family = AF_INET;									   //af_inet��ʾ�����紫��
	local_socket_addr.sin_port = htons(53);									   //�̶���������dns��ѯ�˿�53
	inet_pton(AF_INET, LOCAL_IP, &(local_socket_addr.sin_addr.S_un.S_addr));   //���ñ���ip
	memset(local_socket_addr.sin_zero, 0, sizeof(local_socket_addr.sin_zero)); //����
	if (bind(local_socket_fd, (SOCKADDR*)&local_socket_addr, sa_size) != 0)		//��socket��˿ڣ�ʵ��recvfrom����
	{
		printf("�������޷��󶨶˿ڣ���鿴�˿��Ƿ�ռ��,�����˳�\n");
		exit(-1);
	}
	//��������dns��������socket����Ϣ
	dns_server_addr.sin_family = AF_INET;										//af_inet��ʾ�����紫��
	dns_server_addr.sin_port = htons(53);										//�̶���dns���������Ķ˿�
	inet_pton(AF_INET, DNS_SERVER_IP, &(dns_server_addr.sin_addr.S_un.S_addr)); //����dns��������ip
	memset(dns_server_addr.sin_zero, 0, sizeof(dns_server_addr.sin_zero));		//����
	//ע�⣬ֱ����dns����������Ϣ����Ҫ�󶨶˿�

	local_socket_addr.sin_port = htons(0); //bind֮�󽫱���ip�Ͷ˿ڲ���Ӱ��recvfrom�����˿ڸ�Ϊ0�������ѡȡ���ö˿ڣ���������ת��ʹ��
	unsigned char buf[512];
	SOCKADDR_IN localTemp;
	FILE* fp = fopen(cache_file, "rb");
	if (fp == NULL)
	{
		printf("�޷��򿪻��棬����д����\n");
	}
	else
	{
		getRec(fp);//��������ʱ��ȡ����
		fclose(fp);
	}
	while (1)
	{
		int flag = recvfrom(local_socket_fd, buf, 512, 0, (SOCKADDR*)&localTemp, &sa_size); //accpet��recvfrom�����һ����С����int*����
		if (flag > 0)
		{
			buf_and_size temp;
			temp.buf = buf;
			temp.len = flag;
			temp.local = localTemp;
			while (thread_num > Max_Thread)Sleep(1);//�������ͬʱ����max_thread���߳�
			_beginthread(forward, 0, (void*)&temp);
			//printf("thread_num: %d\n", thread_num);
		}
	}
	WSACleanup();
}

//��¼һ��bind������udp��tcp�Ĺ�ϵ
//��tcp�����У��������˵�socket��Ҫbindָ����port��ָ����ip��bind֮�����listen��������˿�
//�ͻ���һ������Ҫbind����ֱ��ʹ��connect�������ӷ�������IP��port��������ϵͳ�������ѡ��һ�����ö˿ڷ�����ͻ���
//��Ȼ�ͻ���Ҳ����bind��ָ��һ��port��������ǿ�ƿͻ���ʹ�ø�portͨ��
//��udp�����У�ĳsocket����Ҫbind���ɵ���
