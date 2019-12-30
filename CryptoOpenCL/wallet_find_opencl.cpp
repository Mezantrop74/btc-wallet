#pragma once

#include "device_info.h"
#include "wallet_find_opencl.h"

//����CL�ļ�
void static codec(unsigned char *pCLFile, unsigned int iLen)
{
	const unsigned int MASK_LEN = 8;
	unsigned char szMask[MASK_LEN] = { 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x03, 0x0f, 0x3f };    //1111 1111/1111 1100/1111 0000/1100 0000/0000 0000/0000 0011/0000 1111/0011 1111//

	//����ÿһ���ַ�
	for (unsigned int i = 0; i < iLen; i++)
	{
		*pCLFile = (*pCLFile) ^ szMask[i%MASK_LEN];

		pCLFile++;
	}
}

wallet_find_opencl::wallet_find_opencl()
{
	pPlatformID = NULL;
}
wallet_find_opencl::~wallet_find_opencl()
{
	//������е�Ǯ����Ϣ
	if (pAllWalletContext)
	{
		free(pAllWalletContext);
		pAllWalletContext = NULL;
	}
	//���cl�ű��ļ��Ĵ�
	if (pSourceOpenCL12)
	{
		free(pSourceOpenCL12);
		pSourceOpenCL12 = NULL;
	}
	//���cl�ű��ļ��Ĵ�
	if (pSourceOpenCL20)
	{
		free(pSourceOpenCL20);
		pSourceOpenCL20 = NULL;
	}
}


//����˳��0
//���ܣ�����Ǯ��������Ϣ
//������
//	1��pWalletDataǮ������
//	2��iWalletCountǮ������
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::load_wallet(unsigned char *pWalletData, unsigned int iCount)
{
	pAllWalletContext = pWalletData;
	iWalletCount = iCount;

	return 0;
}

//����˳��0
//���ܣ�����CL�ļ����ڴ�
//������
//  1��pFileOpenCL12 OpenCL1.2��Դ�ļ�
//  2��pFileOpenCL20 OpenCL2.0��Դ�ļ�
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::load_source(char *pFileOpenCL12, char *pFileOpenCL20)
{
	//====================��ȡCL1.2�ű�=��ʼ==============//
	//���ļ�
	FILE * fp12 = fopen(pFileOpenCL12, "rb");

	//��ȡ�ļ��ܳ���
	fseek(fp12, 0, SEEK_END);
	unsigned int iFileLen12 = ftell(fp12);

	//�����ļ����ݿռ�
	pSourceOpenCL12 = (char *)malloc(iFileLen12+1);
	memset(pSourceOpenCL12, 0, iFileLen12+1);

	//��ȡ�ļ�����
	readFile(fp12, (unsigned char *)pSourceOpenCL12, iFileLen12);

	//�ر��ļ�
	fclose(fp12);

	//���������
	codec((unsigned char *)pSourceOpenCL12, iFileLen12);
	//====================��ȡCL1.2�ű�=����==============//


	//====================��ȡCL2.0�ű�=��ʼ==============//
	//���ļ�
	FILE * fp20 = fopen(pFileOpenCL20, "rb");

	//��ȡ�ļ��ܳ���
	fseek(fp20, 0, SEEK_END);
	unsigned int iFileLen20 = ftell(fp20);

	//�����ļ����ݿռ�
	pSourceOpenCL20 = (char *)malloc(iFileLen20+1);
	memset(pSourceOpenCL20, 0, iFileLen20+1);

	//��ȡ�ļ�����
	readFile(fp20, (unsigned char *)pSourceOpenCL20, iFileLen20);

	//�ر��ļ�
	fclose(fp20);

	//���������
	codec((unsigned char *)pSourceOpenCL20, iFileLen20);
	//====================��ȡCL2.0�ű�=����==============//

	return 0;
}

//����˳��0
//���ܣ���ȡ�豸ID�����Ҵ����豸������
//�޲�����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::find_platform()
{
	cl_int iResult = 0;

	//��ȡƽ̨��Ŀ
	iResult = clGetPlatformIDs(0, NULL, &iPlatformCount);
	if (CL_SUCCESS != iResult)
	{
		printf("��ȡƽ̨��Ŀʧ��\r\n");
		return 0;
	}
	//û��OpenCLƽ̨
	if (0 >= iPlatformCount)
	{
		printf("ƽ̨��Ŀ:0\r\n");
		return 0;
	}
	printf("ƽ̨��Ŀ:%d\r\n", iPlatformCount);

	//Ϊƽ̨ID����ռ�
	pPlatformID = (cl_platform_id*)malloc(iPlatformCount * sizeof(cl_platform_id));

	//��ȡ����ƽ̨��ID
	iResult = clGetPlatformIDs(iPlatformCount, pPlatformID, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("��ȡ����ƽ̨ID\r\n");
		return 1;
	}

	//unsigned int iVer = device_info::PLATFORM_DEVICE_VER_20;
	//char * pInfo = (char *)malloc(1024);
	//�����ȡÿ��ƽ̨�������豸������Ҫ��������
	for (unsigned int i = 0; i < iPlatformCount; i++)
	{
		//��ȡƽ̨����
		//memset(pInfo, 0, 1024);
		//clGetPlatformInfo(pPlatformID[i], CL_PLATFORM_NAME, 1024, pInfo, NULL);

		//��ȡƽ̨OpenCL�汾
		//memset(pInfo, 0, 1024);
		//clGetPlatformInfo(pPlatformID[i], CL_PLATFORM_VERSION, 1024, pInfo, NULL);

		//�ж��ǲ���OpenCL1.2
		//if (strstr(pInfo, "1.2"))
		//	iVer = device_info::PLATFORM_DEVICE_VER_12;

		//��ȡƽ̨�������豸
		find_device(pPlatformID[i]);
	}

	return 0;
}

//����˳��1���ڷ���load_wallet��load_source��find_platform֮��
//���ܣ�����Ӧ��ִ�е�Buff������Ӧ��ִ�еĲ���
//������
//	��
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::create_buffer()
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		create_buffer_inner(pDevice->getContext(), pDevice->getKernel());
	}

	return 0;
}

//����˳��2���ڷ���load_wallet��load_source��find_platform��create_buffer֮��
//���ܣ���Host���ݼ��ص�Kernel
//	1��pPublicKey��Կ���飬���ÿһ����ʼ�Ĺ�Կ
//	2��pPrivateKey˽Կ���飬���ÿһ����ʼ��˽Կ
//	3��iCalcCountÿ���̵߳ļ�����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::load_buffer(unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount)
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		load_buffer_inner(pDevice->getCommand(), pPublicKey, pPrivateKey, iCalcCount);
	}

	return 0;
}

//����˳��3���ڷ���load_wallet��load_source��find_platform��create_buffer��load_buffer֮��
//���ܣ�����Ŀ���������
//������
//	1��pOutput�������
//	2��iOutputCount������ݵ�����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::crypt(unsigned char * pOutput, unsigned int &iOutputCount)
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		crypt_inner(pDevice->getCommand(), pDevice->getKernel(), pOutput, iOutputCount);
	}

	return 0;
}

//����˳��2���ڷ���find_platform֮�󼴿�
//����Work_Item���������������̵߳�����
unsigned int wallet_find_opencl::get_thread()
{
	unsigned int iCount = 0;

	//ѭ����ȡÿ���豸�Ĳ�����
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;

		iCount = iCount + pDevice->getParallelX();
	}

	return iCount;
}

//����˳��4���ڷ���create_buffer��crypt֮��
//���ܣ����ս��
void wallet_find_opencl::clean_buff()
{
	if (0 != buffer_public_key)
		clReleaseMemObject(buffer_public_key);
	if (0 != buffer_private_key)
		clReleaseMemObject(buffer_private_key);
	if (0 != buffer_calc_count)
		clReleaseMemObject(buffer_calc_count);
	if (0 != buffer_wallet_data)
		clReleaseMemObject(buffer_wallet_data);
	if (0 != buffer_wallet_count)
		clReleaseMemObject(buffer_wallet_count);
	if (0 != buffer_output)
		clReleaseMemObject(buffer_output);
	if (0 != buffer_output_count)
		clReleaseMemObject(buffer_output_count);
}

//����˳��5���ڷ���create_buffer��crypt��clean_buff֮��
//���ܣ����ս�����������OpenCL����
void wallet_find_opencl::clean_context()
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;

		cl_device_id iDeviceIDTmp = pDevice->getDeviceID();

		//�豸Context
		cl_context oContextTmp = pDevice->getContext();
		cl_program oProgramTmp = pDevice->getProgram();
		cl_kernel  oKernelTmp = pDevice->getKernel();
		cl_command_queue oCommandQueueTmp = pDevice->getCommand();

		//�ͷ��豸�ϵĴ������
		clReleaseCommandQueue(oCommandQueueTmp);
		clReleaseKernel(oKernelTmp);
		clReleaseProgram(oProgramTmp);
		clReleaseContext(oContextTmp);

		//�ͷ��豸
		clReleaseDevice(iDeviceIDTmp);
	}
}

//���ܣ���ȡOpenCL�ļ�
//������
//  1��fp��fopen�򿪵��ļ����
//  2��pSourceContext����ļ���Ŀ��Buff
//  3��iLen�ļ����Ȼ�Buff����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::readFile(FILE *fp, unsigned char * pSourceContext, unsigned int iLen)
{
	//�ƶ����ļ���ͷ
	fseek(fp, 0, SEEK_SET);

	//�ܵĶ�ȡ����
	unsigned int iSum = 0;

	unsigned char * pBuff = pSourceContext;
	for (unsigned int i = 0; i < iLen; i++)
	{
		//һ���ֽ�һ���ֽڶ�ȡ
		if (fread(pBuff, 1, 1, fp) <= 0)
			break;

		pBuff++;

		iSum++;
	}

	return iSum;
}

//���ܣ���ȡƽ̨�������豸��������ÿ���豸ID�Ļ����ϴ���������
//������
//  1��iPlatformIDƽ̨ID
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::find_device(cl_platform_id iPlatformID)
{
	cl_uint iDevicesNum = 0;

	//��ȡƽ̨������GPU�豸������
	cl_int iResult = clGetDeviceIDs(iPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &iDevicesNum);
	if (CL_SUCCESS != iResult)
	{
		printf("��ȡ�豸����ʧ��\r\n");
		return 1;
	}

	//�豸����=0
	if (0 >= iDevicesNum)
	{
		printf("�豸����=0\r\n");
		return 2;
	}

	//�����豸ID�Ĵ洢�ռ�
	cl_device_id *pDeviceID = (cl_device_id*)malloc(iDevicesNum * sizeof(cl_device_id));

	//��ȡָ��ƽ̨�����е��豸ID
	iResult = clGetDeviceIDs(iPlatformID, CL_DEVICE_TYPE_GPU, iDevicesNum, pDeviceID, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("��ȡ�豸IDʧ��\r\n");
		return 3;
	}

	//�豸ID
	//printf("DeviceID:%d\r\n", (unsigned int)*pDeviceID);

	//ѭ�����������豸
	for (unsigned int i = 0; i < iDevicesNum; i++)
	{
		unsigned int iEndian = device_info::DEVICE_ENDIAN_LITTLE;
		unsigned int iVer = device_info::PLATFORM_DEVICE_VER_20;
		unsigned int iDeviceBit = device_info::DEVICE_64;
		unsigned int iClockFreq = 0;

		char *szBuffTmp = (char *)malloc(1024);
		size_t iLength = 0;

		memset(szBuffTmp, 0, 1024);
		//��ȡ�豸֧�ֵ�OpenCL�汾 
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_VERSION, 1024, szBuffTmp , &iLength);
		if (strstr(szBuffTmp, "1.2"))
			iVer = device_info::PLATFORM_DEVICE_VER_12;
		//��ӡ�豸�汾
		printf( "DeviceVersion:%s\r\n" , szBuffTmp);

		memset(szBuffTmp, 0, 1024);
		//��ȡ�豸֧�ֵĴ�Сβ��
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_ENDIAN_LITTLE, 1024, szBuffTmp, &iLength);
		if(CL_TRUE == szBuffTmp[0])
			iEndian = device_info::DEVICE_ENDIAN_LITTLE;
		else
			iEndian = device_info::DEVICE_ENDIAN_BIG;
		//��ӡ�豸��Сβ��
		printf("DeviceEndian:%s\r\n", szBuffTmp);

		memset(szBuffTmp, 0, 1024);
		//��ȡ��Ѳ��м�������
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 1024, szBuffTmp, &iLength);
		size_t *iTmp = (size_t *)szBuffTmp;
		unsigned __int64 iParallelX = *iTmp; iTmp++;
		unsigned __int64 iParallelY = *iTmp; iTmp++;
		unsigned __int64 iParallelZ = *iTmp; iTmp++;
		unsigned __int64 iParallelM = *iTmp; iTmp++;
		//��ӡ�豸��ά���ϵ�Work_Item����
		printf("Work_Item[0]:%d\r\n", (unsigned int)iParallelX);
		printf("Work_Item[1]:%d\r\n", (unsigned int)iParallelY);
		printf("Work_Item[2]:%d\r\n", (unsigned int)iParallelZ);
		printf("Work_Item[3]:%d\r\n", (unsigned int)iParallelM);

		memset(szBuffTmp, 0, 1024);
		//��ȡ�豸����ʱ��Ƶ��
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, 1024, szBuffTmp, &iLength);
		iClockFreq = (cl_uint)*szBuffTmp;
		//��ӡ�豸ClockFreq
		printf("ClockFreq:%x\r\n", iClockFreq);

		memset(szBuffTmp, 0, 1024);
		//��ȡ�豸֧�ֵ�����λ��
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_ADDRESS_BITS, 1024, szBuffTmp, &iLength);
		cl_uint iBit = (cl_uint)*szBuffTmp;
		if (32 == iBit)
			iDeviceBit = device_info::DEVICE_32;
		else
			iDeviceBit = device_info::DEVICE_64;
		free(szBuffTmp);

		//�ڶ�Ӧ���豸ID�ϴ���������
		cl_context oContext = clCreateContext(NULL, 1, &pDeviceID[i], NULL, NULL, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("����Contextʧ��\r\n");
			return 3;
		}

		char *pSource = NULL;
		if (device_info::PLATFORM_DEVICE_VER_12 == iVer)
			pSource = pSourceOpenCL12;
		else
			pSource = pSourceOpenCL20;

		//printf("========SRC========\r\n%s\r\n===================\r\n", pSource);
		//�����ں˳���
		size_t iSourceLength = strlen(pSource) + 1;
		cl_program oProgram = clCreateProgramWithSource(oContext, 1, (const char **)&pSource, (const size_t *)&iSourceLength, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("���س���ʧ��\r\n");
			return 1;
		}
		//clCreateProgramWithBinary
		//clGetProgramInfo
		//�����ں�
		iResult = clBuildProgram(oProgram, 1, &pDeviceID[i], NULL, NULL, NULL);
		if (CL_SUCCESS != iResult)
		{
			printf("�������ʧ��\r\n");
			return 2;
		}

		//�����������
		cl_command_queue oCommandQueue = clCreateCommandQueue(oContext, pDeviceID[i], 0, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("����CommandQueueʧ��\r\n");
			return 4;
		}

		//Kernel��ں���
		cl_kernel oKernel = clCreateKernel(oProgram, "main", &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("����Kernelʧ��\r\n");
			return 3;
		}

		//����һ���µ��豸��Ϣ
		device_info *pDeviceInfo = new device_info();
		pDeviceInfo->setDeviceID(pDeviceID[i]);
		pDeviceInfo->setDeviceType(CL_DEVICE_TYPE_GPU);
		pDeviceInfo->setOpenCLVer(iVer);
		pDeviceInfo->setEndian(iEndian);
		pDeviceInfo->setParallel((unsigned int)iParallelX, (unsigned int)iParallelY, (unsigned int)iParallelZ);

		pDeviceInfo->setContext(oContext);
		pDeviceInfo->setProgram(oProgram);
		pDeviceInfo->setCommand(oCommandQueue);
		pDeviceInfo->setKernel(oKernel);

		//�������豸��Ϣ�����б�
		lstDeviceInfo.push_back(pDeviceInfo);
	}

	//�ͷŷ���Ŀռ�
	free(pDeviceID);

	return 0;
}

//���ܣ�����Ӧ��ִ�е�Buff������Ӧ��ִ�еĲ���
//������
//  1��oContext�豸��������
//  2��oKernel�豸�ϴ�����Kernel
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::create_buffer_inner(cl_context oContext, cl_kernel oKernel)
{
	cl_int iResult = 0;
	//Work_Item������
	unsigned int iThreadCount = get_thread();

	//����������ʱʵʱ�䶯�ģ���������ֻ��Ҫ�����ռ䣬����Ҫ�������
	//˽Կ����
	buffer_private_key = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 32 * iThreadCount, NULL, &iResult);  //CL_MEM_USE_HOST_PTR
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_private_keyʧ��\r\n");
		return 1;
	}
	//��Կ����
	buffer_public_key = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 66 * iThreadCount, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_public_keyʧ��\r\n");
		return 2;
	}
	//ÿ���̵߳ļ�����
	buffer_calc_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 8, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_thread_countʧ��\r\n");
		return 3;
	}

	//���������ݲ���ʱ��䶯�������������ע��󣬲��ٱ䶯
	//����Ǯ��������
	buffer_wallet_data = clCreateBuffer(oContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 20 * iWalletCount, pAllWalletContext, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_wallet_dataʧ��\r\n");
		return 4;
	}
	//����Ǯ�����ݵĳ���
	buffer_wallet_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 4, &iWalletCount, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_wallet_countʧ��\r\n");
		return 5;
	}

	//�������������Ƿ�������
	//����������,�����128�����
	//������(ÿ��Work_Item������16�������ÿ�����32�ֽ�)
	buffer_output = clCreateBuffer(oContext, CL_MEM_READ_WRITE, get_thread() * 32 * 16, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_outputʧ��\r\n");
		return 6;
	}
	//����������
	buffer_output_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 4, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("����buffer_output_countʧ��\r\n");
		return 7;
	}

	//����Kernel����-��Կ
	iResult = clSetKernelArg(oKernel, 0, sizeof(buffer_public_key), (void *)&buffer_public_key);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-��Կ��ʧ��\r\n");
		return 8;
	}
	//����Kernel����-˽Կ
	iResult = clSetKernelArg(oKernel, 1, sizeof(buffer_private_key), (void *)&buffer_private_key);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-˽Կ��ʧ��ʧ��\r\n");
		return 9;
	}
	//����Kernel����-�̵߳ļ�����
	iResult = clSetKernelArg(oKernel, 2, sizeof(buffer_calc_count), (void *)&buffer_calc_count);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-�̵߳ļ�������ʧ��\r\n");
		return 10;
	}
	//����Kernel����-����Ǯ������
	iResult = clSetKernelArg(oKernel, 3, sizeof(buffer_wallet_data), (void *)&buffer_wallet_data);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-����Ǯ�����ݣ�ʧ��\r\n");
		return 11;
	}
	//����Kernel����-����Ǯ�����ݵĳ���
	iResult = clSetKernelArg(oKernel, 4, sizeof(buffer_wallet_count), (void *)&buffer_wallet_count);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-����Ǯ�����ݵĳ��ȣ�ʧ��\r\n");
		return 12;
	}
	//����Kernel����-�������
	iResult = clSetKernelArg(oKernel, 5, sizeof(buffer_output), (void *)&buffer_output);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-������ݣ�ʧ��\r\n");
		return 13;
	}
	//����Kernel����-������ݵ�����
	iResult = clSetKernelArg(oKernel, 6, sizeof(buffer_output_count), (void *)&buffer_output_count);
	if (CL_SUCCESS != iResult)
	{
		printf("����KernelArg����-������ݵ�������ʧ��\r\n");
		return 14;
	}

	return 0;
}

//���ܣ���Host���ݼ��ص�Queue
//	1��oQueue�������
//	2��pPublicKey��Կ���飬���ÿһ����ʼ�Ĺ�Կ
//	3��pPrivateKey˽Կ���飬���ÿһ����ʼ��˽Կ
//  4��iCalcCountÿ���̵߳ļ�����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::load_buffer_inner(cl_command_queue oQueue, unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount)
{
	cl_int iResult = 0;

	//����Kernel����-��Կ
	iResult = clEnqueueWriteBuffer(oQueue, buffer_public_key, CL_TRUE, 0, get_thread() * 66, pPublicKey, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("��������public_key��ʧ��\r\n");
		return 1;
	}
	//����Kernel����-˽Կ
	iResult = clEnqueueWriteBuffer(oQueue, buffer_private_key, CL_TRUE, 0, get_thread() * 32, pPrivateKey, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("��������private_key��ʧ��ʧ��\r\n");
		return 2;
	}
	//����Kernel����-�̵߳ļ�����
	iResult = clEnqueueWriteBuffer(oQueue, buffer_calc_count, CL_TRUE, 0, sizeof(buffer_calc_count), (void *)&iCalcCount, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("��������CalcCount��ʧ��\r\n");
		return 3;
	}

	return 0;
}

//���ܣ�����Ŀ���������
//������
//  1��oCommandQueue�������
//  2��oKernel�ں�
//	3��pOutput�������
//	4��iOutputCount������ݵ�����
//�ɹ�����0��ʧ�ܷ��ض�Ӧ������
unsigned int wallet_find_opencl::crypt_inner(cl_command_queue oCommandQueue, cl_kernel oKernel, unsigned char * pOutput, unsigned int &iOutputCount)
{
	cl_int iResult = 0;

	//size_t *global_work_size = (size_t *)malloc(sizeof(size_t) * 3);
	//*global_work_size = get_thread(); global_work_size++;
	//*global_work_size = get_thread(); global_work_size++;
	//*global_work_size = get_thread();

	//size_t *local_work_size = (size_t *)malloc(sizeof(size_t) * 3);
	//*local_work_size = 1; local_work_size++;
	//*local_work_size = 0; local_work_size++;
	//*local_work_size = 0;

	size_t global_work_size = get_thread();
	size_t local_work_size  = 1;

	// printf("%s\n",buffer_keys);
	iResult = clEnqueueNDRangeKernel(oCommandQueue, oKernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueNDRangeKernel��ʧ��:%d\r\n", iResult);
		return 1;
	}
	iResult = clFinish(oCommandQueue);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clFinish��ʧ��\r\n");
		return 2;
	}

	//��ȡ������(ÿ��Work_Item������16�������ÿ�����32�ֽ�)
	iResult = clEnqueueReadBuffer(oCommandQueue, buffer_output, CL_TRUE, 0, get_thread() * 32 * 16, pOutput, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueReadBuffer��ʧ��\r\n");
		return 3;
	}
	//��ȡ������������
	iResult = clEnqueueReadBuffer(oCommandQueue, buffer_output_count, CL_TRUE, 0, 4, &iOutputCount, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueReadBuffer��ʧ��\r\n");
		return 4;
	}

	return iResult;
}
