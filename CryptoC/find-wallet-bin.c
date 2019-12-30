#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//����Ǯ����ַ
unsigned char *szWallet = NULL;
//����ʵ�ʴ����
unsigned int iSumBinCount = 0;

//��ʼ������ By �������ļ�
void wallet_bin_init(unsigned char * pWalletData, unsigned int iWalletCount)
{
	szWallet     = pWalletData;
	iSumBinCount = iWalletCount;
}

//����Ǯ��
int wallet_bin_find(unsigned char * pWallet, unsigned int iIndexStart, unsigned int iIndexEnd)
{
	//������ҷ�Χ�ǳ�С����ôֱ�ӱ�������
	if ((iIndexEnd - iIndexStart) < 4)
	{
		for (unsigned int i = iIndexStart; i <= iIndexEnd; i++)
			if (0 == memcmp(pWallet, szWallet + (i * 20), 20))
				return 0;
		return 1;
	}

	//�м�λ��
	unsigned int iMiddle = (iIndexStart + iIndexEnd) / 2;
	//��������ݹ鴦��
	int iResult = memcmp(pWallet, szWallet + (iMiddle * 20), 20);
	if (0 == iResult)
		return 0;
	else if (0 > iResult)
		return wallet_bin_find(pWallet, iIndexStart, iMiddle);
	else
		return wallet_bin_find(pWallet, iMiddle, iIndexEnd);
}
