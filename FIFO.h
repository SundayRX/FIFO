#ifndef __FIFO_H
#define __FIFO_H 	


typedef struct
{
	void* Pointer;
	unsigned int TypeSize;
	unsigned int Capacity;
	volatile unsigned int PWrite;
	volatile unsigned int PRead;
}
FIFO_S;

/*********************************************************************************
*FIFO����
*********************************************************************************/
//��ʼ��FIFO���ƽṹ�� ����Capacity����ΪTypeSize�������� ����ֵ�Ƿ��ʼ���ɹ�
unsigned char FIFOInit(FIFO_S* FIFO, void* Addr, unsigned int Capacity, unsigned short TypeSize);
//���FIFO����������
void FIFOFlush(FIFO_S* FIFO);
//��λFIFO
void FIFOReset(FIFO_S* FIFO);

/*********************************************************************************
*FIFO״̬
*********************************************************************************/
//��ȡFIFO�ڿɶ��ֽ���
unsigned int FIFOGetReadByteSize(FIFO_S* FIFO);
//��ȡFIFO�ڿ�д�ֽ���
unsigned int FIFOGetWriteByteSize(FIFO_S* FIFO);

/*********************************************************************************
*FIFO��д ��TypeSizeΪ����
*********************************************************************************/
//��FIFO�ڶ�ȡָ������������(1������Ϊ�趨��TypeSize��С) ���سɹ���ȡ�ĸ���
unsigned int FIFORead(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength);
//��FIFO���ڶ�ָ���ָ������ƫ��λ�ô���ȡָ����������(1������Ϊ�趨��TypeSize��С)�Ҳ��ƶ���ָ�� �����Ƿ�ɹ���ȡ
unsigned int FIFOVirtualRead(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength, unsigned int DataOffset);
//��FIFO��д��ָ������������(1������Ϊ�趨��TypeSize��С)  �����Ƿ�ɹ�д��
unsigned char FIFOWrite(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength);

/*********************************************************************************
*TypeSize=1��FIFO ͨ�Ŵ��� Э��ջճ����������
*********************************************************************************/
//��FIFO�ڶ�ȡ1��byte �����Ƿ�ɹ���ȡ
unsigned char FIFOReadByte(FIFO_S* FIFO, void* Byte);
//��FIFO���ڶ�ָ���ָ��ƫ��λ�ô���ȡ1��byte�Ҳ��ƶ���ָ�� �����Ƿ�ɹ���ȡ
unsigned char FIFOVirtualReadByte(FIFO_S* FIFO, void* Byte, unsigned int ByteOffset);
//����ָ��ǰ��ָ���������ֽ��� �����Ƿ�����ɹ�
unsigned char FIFOMoveReadPoint(FIFO_S* FIFO, unsigned int ByteLength);
//��FIFO��д��1��byte �����Ƿ�ɹ�д��
unsigned char FIFOWriteByte(FIFO_S* FIFO, void* Byte);



void* FIFOMemcpy(void* dst, const void* src, unsigned int len);
#endif
