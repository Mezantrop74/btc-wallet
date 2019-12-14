#pragma once

class WalletTaskItem
{
public:
	WalletTaskItem()
	{
		pPrivateStart = (unsigned char *)malloc( 32 );
		pPublicStart  = (unsigned char *)malloc( 66 );

		iCurrent = (unsigned int *)malloc(8);
		memset(iCurrent, 0, 8);
	};
	~WalletTaskItem()
	{
		free(pPrivateStart);
		free(pPublicStart);

		free(iCurrent);
	};

public:
	void setJobID(unsigned int iID)
	{
		iJobID = iID;
	};
	void setTaskID(unsigned int iID)
	{
		iTaskID = iID;
	};
	void setPrivateCount(unsigned int iCount)
	{
		iPrivateCount = iCount;
	};
	void setPrivateStart(unsigned char *pPrivate)
	{
		memcpy(pPrivateStart, pPrivate, 32);
	};
	void setPublicStart(unsigned char *pPublic)
	{
		memcpy(pPublicStart, pPublic, 66);
	};
	void setCurrent(unsigned int *iCurrent)
	{
		this->iCurrent = iCurrent;
	};

	unsigned int getJobID()
	{
		return iJobID;
	};
	unsigned int getTaskID()
	{
		return iTaskID;
	};
	unsigned int getPrivateCount()
	{
		return iPrivateCount;
	};
	unsigned char * getPrivateStart()
	{
		return pPrivateStart;
	};
	unsigned char * getPublicStart()
	{
		return pPublicStart;
	};
	unsigned int * getCurrent()
	{
		return iCurrent;
	};

	void setState(unsigned int iState)
	{
		this->iState = iState;
	};
	unsigned int getState()
	{
		return iState;
	};
private:
	unsigned char *pPrivateStart = NULL;
	unsigned char *pPublicStart = NULL;
	unsigned int iJobID = 0;
	unsigned int iTaskID = 0;
	unsigned int iPrivateCount = 0;

	//��ǰ���ڴ��������
	unsigned int *iCurrent = NULL;

	unsigned int iState = 0;    //0:��ʼ״̬��1:����״̬��2:���״̬��3: ɾ��״̬
};

