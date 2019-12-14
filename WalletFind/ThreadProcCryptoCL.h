#pragma once

#include <thread>
#include <list>
#include <mutex>

#include "WalletResultItem.h"
#include "WalletResult.h"
#include "WalletTask.h"

#include "../CryptoC/find-wallet-bin.h"
#include "../CryptoC/ecmult_gen_impl.h"
#include "../CryptoC/rmd160.h"
#include "../CryptoC/sha256.h"

#include "../CryptoOpenCL/wallet_find_opencl.h"

//ҵ�������߳�
//���ڼ������֤һ��˽Կ
class ThreadProcCryptoCL
{
public:
	ThreadProcCryptoCL()
	{
		//����OpenCL�ļ�
		pFileOpenCL12 = (char *)malloc( 512 );
		pFileOpenCL20 = (char *)malloc( 512 );

		//��ʼ��OpenCL����
		pWalletFind = new wallet_find_opencl();
		//��ʼ��OpenCLƽ̨
		pWalletFind->find_platform();

		//��ʼ����Բ����
		pContext = (secp256k1_ecmult_gen_context*)malloc(sizeof(secp256k1_ecmult_gen_context));
		secp256k1_ecmult_gen_context_build(pContext);

		//���ؽ������
		pWalletResult = new WalletResult();
		//�����������
		pWalletTask = new WalletTask;
	};
	~ThreadProcCryptoCL()
	{
		free(pFileOpenCL12);
		free(pFileOpenCL20);

		//���OpenCLƽ̨
		pWalletFind->clean_buff();
		delete pWalletFind;

		free(pContext);

		//�ͷŽ������
		delete pWalletResult;
		//�ͷ��������
		delete pWalletTask;
	};

	//�����߳�
	void start()
	{
		//���ÿ�ʼ��־
		iFlagExit = 1;


		//���������߳�
		oMainWorkThd = std::thread(&ThreadProcCryptoCL::run, this);
		oMainWorkThd.detach();
	};
	//ֹͣ�߳�
	void stop()
	{
		//�����˳���־
		iFlagExit = 0;

		//���OpenCLƽ̨
		pWalletFind->clean_buff();
		delete pWalletFind;
	};

	//����OpenCL�ļ�
	//������
	//  1��OpenCL�ļ�������·�����ļ�������������С��500�ֽڣ�
	void setFile(char *pOpenCL12, char *pOpenCL20)
	{
		if (pOpenCL12)
			strcpy(pFileOpenCL12, pOpenCL12);
		if (pOpenCL20)
			strcpy(pFileOpenCL20, pOpenCL20);

		//������Դ
		int iResult = pWalletFind->load_source(pFileOpenCL12, pFileOpenCL20);
	};
	//����Ǯ������
	//������
	//  1��pWalletDataǮ�����ݣ�
	//  2��iWalletCountǮ������
	void setWalletData(unsigned char *pWalletData, unsigned int iWalletCount)
	{
		this->pWalletData  = pWalletData;
		this->iWalletCount = iWalletCount;

		//����Ǯ������
		int iResult = pWalletFind->load_wallet(pWalletData, iWalletCount);
	};

	//���ҵ���̵߳�����Data���������
	//������
	//  1��JobID��
	//  2������ID��
	//  3����ʼ˽Կ��
	//  4��ÿ�̵߳ļ�������
	void addTaskData(unsigned int iJobID, unsigned int iTaskID, unsigned char *szPrivateKey, unsigned int iCalcCount)
	{
		WalletTaskItem * pTaskItem = new WalletTaskItem();
		pTaskItem->setJobID(iJobID);
		pTaskItem->setTaskID(iTaskID);
		pTaskItem->setPrivateStart(szPrivateKey);
		pTaskItem->setPrivateCount(iCalcCount);

		pWalletTask->pushItem(pTaskItem);
	};

	//�ж��߳��Ƿ�������
	bool isRun()
	{
		return (1 == iFlagExit);
	};

	//��ȡ����б�
	WalletResult * getResult()
	{
		return pWalletResult;
	};
	//��ȡ����
	WalletTask * getTask()
	{
		return pWalletTask;
	};
private:
	//������ͷ�Ĺ�Կ
	unsigned int genPublicKeyStart(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char *pPrivateStart)
	{
		//ʹ��˽Կ�����㹫ԿΪÿ��GPU�̼߳��㹫Կ
		for (unsigned int i = 0; i < iThreadCount; i++)
		{
			//Ŀ��˽Կ
			ProcPrivateAddInt(pPrivateKey, pPrivateStart, iCalcCount * i);

			//Ŀ��˽Կ��Ӧ�Ĺ�Կ
			secp256k1_ecmult_gen(pContext, pPublicKey, pPrivateKey);

			//Ŀ��˽Կ�͹�Կ�Ĵ洢λ��
			pPrivateKey = pPrivateKey + 32;
			pPublicKey  = pPublicKey  + 66;
		}
		return 0;
	};
	//���������Ĺ�Կ
	unsigned int genPublicKeyOther(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char * pOutput, unsigned int &iResultCount, unsigned int iJob, unsigned int iTask )
	{
		//unsigned char pPublicKey222[66] = { 0x04,0x4d,0xd2,0x58,0xcc,0x3e,0x05,0x0b,0x57,0x02,0x99,0xef,0x45,0xde,0x5d,0x96,0xe5,0x24,0x05,0x10,0x96,0xa2,0xa9,0xae,0x52,0xd2,0x2b,0xa8,0x92,0x7b,0x16,0x7f,0xce,0xf2,0x97,0xf3,0x5a,0x0d,0xe8,0xb7,0xc5,0x78,0x92,0x64,0xd2,0xde,0x85,0x8d,0xc8,0x58,0x2c,0x39,0x36,0x8c,0x39,0x9f,0xd9,0x1d,0xc5,0xa9,0x2c,0x33,0xd8,0x5a,0xa1,0x0 };

		//�м䷵��ֵ
		int iResult = 0;
		//����Buff
		iResult = pWalletFind->create_buffer();
		if (0 != iResult)
			return iResult;
		//��������
		iResult = pWalletFind->load_buffer(pPublicKey, pPrivateKey, iCalcCount);
		if (0 != iResult)
			return iResult;
		//������
		iResult = pWalletFind->crypt(pOutput, iResultCount);
		if (0 != iResult)
			return iResult;
		return iResult;
	};
	//����˽Կ �� 32λ�������
	int ProcPrivateAddInt(unsigned char *pPrivateDest, unsigned char * szPrivateKey, unsigned int iNum)
	{
		unsigned char iFirst  = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iSecond = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iThird  = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iFourth = iNum % 0x100; iNum = iNum / 0x100;

		//�Ȱ�ȫ�����ݷŵ�Ŀ��ռ�
		memcpy(pPrivateDest, szPrivateKey, 32);

		//ǰ28λԭ������
		int iTmp = szPrivateKey[31] + iFirst; pPrivateDest[31] = szPrivateKey[31] + iFirst;
		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[30] + iSecond + 1; pPrivateDest[30] = szPrivateKey[30] + iSecond + 1;
		}
		else
		{
			iTmp = szPrivateKey[30] + iSecond; pPrivateDest[30] = szPrivateKey[30] + iSecond;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[29] + iThird + 1; pPrivateDest[29] = szPrivateKey[29] + iThird + 1;
		}
		else
		{
			iTmp = szPrivateKey[29] + iThird; pPrivateDest[29] = szPrivateKey[29] + iThird;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[28] + iFourth + 1; pPrivateDest[28] = szPrivateKey[28] + iFourth + 1;
		}
		else
		{
			iTmp = szPrivateKey[28] + iFourth; pPrivateDest[28] = szPrivateKey[28] + iFourth;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[27] + 1; szPrivateKey[27] = szPrivateKey[27] + 1;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[26] + 1; szPrivateKey[26] = szPrivateKey[26] + 1;
		}
		return iTmp;
	};

private:
	//�ͷ�ÿһ���Ѿ���Ϊɾ��������
	static void ReleaseFinish(WalletTask * pWalletTask)
	{
		//�������
		WalletTaskItem * pTaskItem = NULL;
		//��ȡ��һ��Item
		pTaskItem = pWalletTask->getItemBegin();

		//ѭ���ͷ�ÿһ���Ѿ���Ϊɾ��������
		while (true)
		{
			//����Ҳ�����������ȥ
			if (NULL == pTaskItem)
			{
				break;
			}

			//����Ѿ�����Ϊɾ��״̬����ɾ����
			if (3 == pTaskItem->getState())
			{
				//�������嵥ɾ��
				pWalletTask->popItem(pTaskItem);
				//�ͷſռ�
				delete pTaskItem;
				pTaskItem = NULL;
			}

			//��ȡ��һ������
			pTaskItem = pWalletTask->getItemNext();
		}
	}
	//�߳�������
	static void run(ThreadProcCryptoCL * pThread)
	{
		if (NULL == pThread)
			return;

		//�������
		WalletTask * pWalletTask = pThread->pWalletTask;
		WalletTaskItem * pTaskItem = NULL;

		while (pThread->iFlagExit)
		{
			//���Һ��ͷ��Ѿ�ɾ��������
			ReleaseFinish(pWalletTask);

			//Ѱ��δ���������Item
			pTaskItem = pWalletTask->getItemPending();
			//����Ҳ������͵�һ������
			if (!pTaskItem)
			{
				Sleep(100);
				continue;
			}

			//����Ϊ���ڴ���״̬
			pTaskItem->setState(1);

			//�����񽻸�������������
			pThread->proc_task(pTaskItem);

			//����Ϊ���״̬
			pTaskItem->setState(2);
		}
		//�˳�״̬
		pThread->iFlagExit = 0;
	};
	void proc_task(WalletTaskItem * pTaskItem)
	{
		//��ʼ˽Կ
		unsigned char szPrivateStart[33] = { 0 };
		memcpy(szPrivateStart, pTaskItem->getPrivateStart(), 32);
		//JobID
		unsigned int iJobID = pTaskItem->getJobID();
		//TaskID
		unsigned int iTaskID = pTaskItem->getTaskID();
		//˽Կ����
		unsigned int iPrivateCount = pTaskItem->getPrivateCount();

		//����ÿ���׶η��صĽ��
		unsigned int iResult = 0;

		//������߳��������ϲ���˵���̣߳��˴�ָGPU�Ĳ��м�������		//��ȡGPU����Ѳ�������
		unsigned int iThreadCount = pWalletFind->get_thread();
		//ÿ���̵߳ļ�����
		unsigned int iCalcCount = iPrivateCount / iThreadCount + 1;

		//����˽Կ�͹�Կ�Ŀռ�
		unsigned char * pPrivateKey = (unsigned char *)malloc(32 * iThreadCount);
		unsigned char * pPublicKey  = (unsigned char *)malloc(66 * iThreadCount);

		//���ؽ���Լ����ؽ��������ÿ���߳����128������
		unsigned int iResultCount = 0;
		unsigned char * pOutput = (unsigned char *)malloc(32 * iThreadCount * 128);

		//���㿪ʼ��Կ����
		iResult = genPublicKeyStart(pPrivateKey, pPublicKey, iThreadCount, iCalcCount, szPrivateStart);

		//�������Ϊ0
		iResultCount = 0;

		//����GPU����ʣ��ļ�������
		iResult = genPublicKeyOther(pPrivateKey, pPublicKey, iThreadCount, iCalcCount, pOutput, iResultCount, iJobID, iTaskID);

		//����OpenCL�����е������ζ������������ɣ����ý������ʾ����
		unsigned int * iTmp = pTaskItem->getCurrent();
		*iTmp = iPrivateCount;

		//��ȡÿ�����ؽ��
		for (unsigned int i = 0; i < iResultCount; i++)
		{
			WalletResultItem *pItem = new WalletResultItem();
			pItem->setPrivateKey(pOutput + i * 32);
			pItem->setJobID(iJobID);
			pItem->setTaskID(iTaskID);
			pWalletResult->pushItem(pItem);
		}

		//�ͷſռ�
		free(pPrivateKey);
		free(pPublicKey);
		free(pOutput);
	};

private:
	//�߳����к��˳���־: 0,�˳���1,����
	unsigned int iFlagExit   = 0;

	//���߳̾��
	std::thread oMainWorkThd;
	//������
	WalletResult * pWalletResult = NULL;
	//��������
	WalletTask * pWalletTask = NULL;

	//����OpenCL�ļ�������·��
	char *pFileOpenCL12 = NULL;
	char *pFileOpenCL20 = NULL;

	//Ǯ���ļ�����
	unsigned char *pWalletData = NULL;
	//Ǯ��������Ǯ��������
	unsigned int iWalletCount  = 0;

	//����GPU
	wallet_find_opencl * pWalletFind = NULL;

	//��Բ���߼������
	secp256k1_ecmult_gen_context * pContext = NULL;
};
