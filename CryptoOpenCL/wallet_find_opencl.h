#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include "CL/cl.h"

#include "device_info.h"

class wallet_find_opencl
{
public:
	wallet_find_opencl();
	~wallet_find_opencl();

	//����˳��0
	//���ܣ�����Ǯ��������Ϣ
	//������
	//	1��pWalletDataǮ������
	//	2��iWalletCountǮ������
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int load_wallet(unsigned char *pWalletData, unsigned int iCount);

	//����˳��0
	//���ܣ�����CL�ļ����ڴ�
	//������
	//  1��pFileOpenCL12 OpenCL1.2��Դ�ļ�
	//  2��pFileOpenCL20 OpenCL2.0��Դ�ļ�
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int load_source(char *pFileOpenCL12, char *pFileOpenCL20);

	//����˳��0
	//���ܣ���ȡ�豸ID�����Ҵ����豸������
	//�޲�����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int find_platform();

	//����˳��1���ڷ���load_wallet��load_source��find_platform֮��
	//���ܣ�����Ӧ��ִ�е�Buff������Ӧ��ִ�еĲ���
	//������
	//	��
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int create_buffer();

	//����˳��2���ڷ���load_wallet��load_source��find_platform��create_buffer֮��
	//���ܣ���Host���ݼ��ص�Kernel
	//	1��pPublicKey��Կ���飬���ÿһ����ʼ�Ĺ�Կ
	//	2��pPrivateKey˽Կ���飬���ÿһ����ʼ��˽Կ
	//	3��iCalcCountÿ���̵߳ļ�����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int load_buffer(unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount);

	//����˳��3���ڷ���load_wallet��load_source��find_platform��create_buffer��load_buffer֮��
	//���ܣ�����Ŀ���������
	//������
	//	1��pOutput�������
	//	2��iOutputCount������ݵ�����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int crypt(unsigned char * pOutput, unsigned int &iOutputCount);

	//����˳��2���ڷ���find_platform֮�󼴿�
	//����Work_Item���������������̵߳�����
	unsigned int get_thread();

	//����˳��4���ڷ���create_buffer��crypt֮��
	//���ܣ����ս�����������
	void clean_buff();

	//����˳��5���ڷ���create_buffer��crypt��clean_buff֮��
	//���ܣ����ս�����������OpenCL����
	void clean_context();

private:
	cl_uint iPlatformCount           = 0;       //ƽ̨����
	cl_platform_id *pPlatformID      = NULL;    //ƽ̨ID�б�
	std::list<device_info *> lstDeviceInfo;     //�����豸��Ϣ�б�

	unsigned int iWalletCount        = 0;       //���Ǯ��������
	unsigned char *pAllWalletContext = NULL;    //������е�Ǯ����Ϣ
	char *pSourceOpenCL12            = NULL;    //���OPenCL1.2�ű��ļ��Ĵ�
	char *pSourceOpenCL20            = NULL;    //���OPenCL2.0�ű��ļ��Ĵ�

	//secp256k1�����������
	cl_mem buffer_public_key;      //��Կ
	cl_mem buffer_private_key;     //˽Կ
	cl_mem buffer_calc_count;      //ÿ���̵߳ļ�����
	cl_mem buffer_wallet_data;     //����Ǯ��������Buff
	cl_mem buffer_wallet_count;    //����Ǯ��������
	cl_mem buffer_output;          //����������
	cl_mem buffer_output_count;    //����������

private:
	void setJobID(unsigned int iID)
	{
		iJobID = iID;
	}
	void setTaskID(unsigned int iID)
	{
		iTaskID = iID;
	}
	void setPrivateCount(unsigned int iCount)
	{
		iPrivateCount = iCount;
	}
	unsigned int getJobID()
	{
		return iJobID;
	}
	unsigned int getTaskID()
	{
		return iTaskID;
	}
	unsigned int getPrivateCount()
	{
		return iPrivateCount;
	}
	bool isFinish()
	{
		return true;
	}

	//���ܣ���ȡOpenCL�ļ�
	//������
	//  1��fp��fopen�򿪵��ļ����
	//  2��pSourceContext����ļ���Ŀ��Buff
	//  3��iLen�ļ����Ȼ�Buff����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int readFile(FILE *fp, unsigned char * pSourceContext, unsigned int iLen);

	//���ܣ���ȡƽ̨�������豸��������ÿ���豸ID�Ļ����ϴ���������
	//������
	//  1��iPlatformIDƽ̨ID
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int find_device(cl_platform_id iPlatformID);

	//���ܣ�����Ӧ��ִ�е�Buff������Ӧ��ִ�еĲ���
	//������
	//  1��oContext�豸��������
	//  2��oKernel�豸�ϴ�����Kernel
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int create_buffer_inner(cl_context oContext, cl_kernel oKernel);

	//���ܣ���Host���ݼ��ص�Queue
	//	1��oQueue�������
	//	2��pPublicKey��Կ���飬���ÿһ����ʼ�Ĺ�Կ
	//	3��pPrivateKey˽Կ���飬���ÿһ����ʼ��˽Կ
	//  4��iCalcCountÿ���̵߳ļ�����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int load_buffer_inner(cl_command_queue oQueue, unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount);

	//���ܣ�����Ŀ���������
	//������
	//  1��oCommandQueue�������
	//  2��oKernel�ں�
	//	3��pOutput�������
	//	4��iOutputCount������ݵ�����
	//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
	unsigned int crypt_inner(cl_command_queue oCommandQueue, cl_kernel oKernel, unsigned char * pOutput, unsigned int &iOutputCount);
private:
	unsigned int iJobID = 0;
	unsigned int iTaskID = 0;
	unsigned int iPrivateCount = 0;
};
