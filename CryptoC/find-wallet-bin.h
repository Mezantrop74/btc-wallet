#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	//��ʼ������ By ��������ļ�
	void wallet_bin_init(unsigned char * pWalletData, unsigned int iWalletCount);

	//����Ŀ������
	int wallet_bin_find(unsigned char * pWallet, unsigned int iIndexStart, unsigned int iIndexEnd);

#ifdef __cplusplus
}
#endif