#pragma once

#include "CL/cl.h"

class device_info
{
public:
	//�豸���ͳ���
	static const unsigned int DEVICE_TYPE_CPU = 0;
	static const unsigned int DEVICE_TYPE_GPU = 1;
	static const unsigned int DEVICE_TYPE_OTHER = 2;

	//�豸OpenCL�汾�ų���
	static const unsigned int PLATFORM_DEVICE_VER_12 = 0;
	static const unsigned int PLATFORM_DEVICE_VER_20 = 1;
	static const unsigned int PLATFORM_DEVICE_VER_OTHER = 2;

	//�豸��֧�ִ�β�˻���Сβ��
	static const unsigned int DEVICE_ENDIAN_LITTLE = 0;    //Сβ��
	static const unsigned int DEVICE_ENDIAN_BIG    = 1;    //��β��

	//�豸�ĵ�ַλ
	static const unsigned int DEVICE_32 = 0;    //32λ�豸
	static const unsigned int DEVICE_64 = 1;    //64λ�豸

public:
	device_info() {};
	~device_info() {};

	//�����豸�Ǵ�β�˻���Сβ��
	void setEndian( unsigned int iEndian)
	{
		this->iEndian = iEndian;
	}
	unsigned int getEndian()
	{
		return iEndian;
	}

	//�豸֧�ֵ�OpenCL�汾��
	void setOpenCLVer(unsigned int iVer)
	{
		iOpenCLVer = iVer;
	}
	unsigned int getOpenCLVer()
	{
		return iOpenCLVer;
	}

	//�豸����Ѳ�������
	void setParallel(unsigned int iParallelX,unsigned int iParallelY, unsigned int iParallelZ)
	{
		this->iParallelX = iParallelX;
		this->iParallelY = iParallelY;
		this->iParallelZ = iParallelZ;
	}
	unsigned int getParallelX()
	{
		return iParallelX;
	}
	unsigned int getParallelY()
	{
		return iParallelY;
	}
	unsigned int getParallelZ()
	{
		return iParallelZ;
	}

	//�豸���ʱ��Ƶ��
	void setClockFreq( unsigned int iFreq)
	{
		iClockFreq = iFreq;
	}
	unsigned int getClockFreq()
	{
		return iClockFreq;
	}

	//�豸������λ��
	void setBit(unsigned int iBit)
	{
		iDeviceBit = iBit;
	}
	unsigned int getBit()
	{
		return iDeviceBit;
	}

	//�豸����
	void setDeviceType(unsigned int iType)
	{
		iDeviceType = iType;
	}
	unsigned int getDeviceType()
	{
		return iDeviceType;
	}

	//�豸ID
	void setDeviceID(cl_device_id iID)
	{
		iDeviceID = iID;
	}
	cl_device_id getDeviceID()
	{
		return iDeviceID;
	}

	//�豸ID��Ӧ��������
	void setContext(cl_context oCon )
	{
		oContext = oCon;
	}
	cl_context getContext()
	{
		return oContext;
	}

	//oProgram;
	void setProgram(cl_program oPro)
	{
		oProgram = oPro;
	}
	cl_program getProgram()
	{
		return oProgram;
	}
	
	//kernel
	void setKernel(cl_kernel oKer)
	{
		oKernel = oKer;
	}
	cl_kernel getKernel()
	{
		return oKernel;
	}

	//CommandQueue
	void setCommand(cl_command_queue oCommand)
	{
		oCommandQueue = oCommand;
	}
	cl_command_queue getCommand()
	{
		return oCommandQueue;
	}
private:
	//�豸��֧�ִ�β�˻���Сβ��
	unsigned int iEndian     = DEVICE_ENDIAN_LITTLE;
	//�豸֧�ֵ�OpenCL�汾��
	unsigned int iOpenCLVer  = PLATFORM_DEVICE_VER_20;
	//�豸����Ѳ�������(���������ֳ�����)
	unsigned int iParallelX = 0;
	unsigned int iParallelY = 0;
	unsigned int iParallelZ = 0;
	//�豸�����ʱ��Ƶ��(MHz)
	unsigned int iClockFreq  = 0;
	//�豸�ĵ�ַλ
	unsigned int iDeviceBit  = DEVICE_32;

	//�豸����
	unsigned int iDeviceType = DEVICE_TYPE_GPU;
	//�豸ID
	cl_device_id iDeviceID;

	//�豸Context
	cl_context oContext;

	cl_program oProgram;
	cl_kernel  oKernel;
	cl_command_queue oCommandQueue;
};
