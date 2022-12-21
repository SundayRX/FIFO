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
*FIFO控制
*********************************************************************************/
//初始化FIFO控制结构体 容量Capacity必须为TypeSize的整数倍 返回值是否初始化成功
unsigned char FIFOInit(FIFO_S* FIFO, void* Addr, unsigned int Capacity, unsigned short TypeSize);
//清空FIFO内所有数据
void FIFOFlush(FIFO_S* FIFO);
//复位FIFO
void FIFOReset(FIFO_S* FIFO);

/*********************************************************************************
*FIFO状态
*********************************************************************************/
//获取FIFO内可读字节数
unsigned int FIFOGetReadByteSize(FIFO_S* FIFO);
//获取FIFO内可写字节数
unsigned int FIFOGetWriteByteSize(FIFO_S* FIFO);

/*********************************************************************************
*FIFO读写
*********************************************************************************/
//从FIFO内读取指定数量的数据(1个数据为设定的TypeSize大小) 返回成功读取的个数
unsigned int FIFORead(FIFO_S* FIFO, void* DataBuffer, unsigned int DataCount);
//向FIFO内写入指定数量的数据(1个数据为设定的TypeSize大小)  返回是否成功写入
unsigned char FIFOWrite(FIFO_S* FIFO, void* DataBuffer, unsigned int DataCount);

/*********************************************************************************
*TypeSize=1的FIFO 通信处理 协议栈粘包分析常用
*********************************************************************************/
//从FIFO内读取1个byte 返回是否成功读取
unsigned char FIFOReadByte(FIFO_S* FIFO, void* Data);
//从FIFO内在读指针的指定偏移位置处读取1个byte且不移动读指针 返回是否成功读取
unsigned char FIFOVirtualReadByte(FIFO_S* FIFO, void* Data, unsigned int PReadOffset);
//将读指针前进指定数量的字节数
unsigned char FIFOMoveReadPoint(FIFO_S* FIFO, unsigned int ByteSize);
//向FIFO内写入1个byte 返回是否成功写入
unsigned char FIFOWriteByte(FIFO_S* FIFO, void* Data);



void* FIFOMemcpy(void* dst, const void* src, unsigned int len);
#endif
