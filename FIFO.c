# include "FIFO.h"

void* FIFOMemcpy(void* dst, const void* src, unsigned int len)
{
	if (0 == dst || 0 == src) {
		return 0;
	}

	void* ret = dst;

	if (dst <= src || (char*)dst >= (char*)src + len) {
		//没有内存重叠，从低地址开始复制
		while (len--) {
			*(char*)dst = *(char*)src;
			dst = (char*)dst + 1;
			src = (char*)src + 1;
		}
	}
	else {
		//有内存重叠，从高地址开始复制
		src = (char*)src + len - 1;
		dst = (char*)dst + len - 1;
		while (len--) {
			*(char*)dst = *(char*)src;
			dst = (char*)dst - 1;
			src = (char*)src - 1;
		}
	}
	return ret;
}

unsigned char FIFOInit(FIFO_S* FIFO, void* Addr, unsigned int Capacity, unsigned short TypeSize)
{
	if (Capacity == 0 || (Capacity % TypeSize != 0)) return 0;
	FIFO->Pointer = Addr;
	FIFO->Capacity = Capacity;
	FIFO->TypeSize = TypeSize;
	FIFO->PRead = 0;
	FIFO->PWrite = 0;
	return 1;
}

void FIFOFlush(FIFO_S* FIFO)
{
	FIFO->PRead = FIFO->PWrite;
}

void FIFOReset(FIFO_S* FIFO)
{
	FIFO->PRead = 0;
	FIFO->PWrite = 0;
}

unsigned int FIFOGetReadByteSize(FIFO_S* FIFO)
{
	unsigned int Available[2] = { 0,0 };
	unsigned int TPWrite = FIFO->PWrite;
	unsigned int TPRead = FIFO->PRead;
	if (TPWrite >= TPRead)
	{
		Available[0] = TPWrite - TPRead;
	}
	else if (TPWrite < TPRead)
	{
		Available[0] = TPWrite;
		Available[1] = FIFO->Capacity - TPRead;
	}
	return Available[0] + Available[1];
}

unsigned int FIFOGetWriteByteSize(FIFO_S* FIFO)
{
	return FIFO->Capacity - FIFOGetReadByteSize(FIFO);
}

unsigned int FIFORead(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength)
{
	unsigned int ReadCount = FIFOGetReadByteSize(FIFO) / FIFO->TypeSize;
	unsigned int C = 0;
	unsigned short RemainGranularity;
	int NewPRead;
	if (ReadCount == 0 || DataLength == 0) return 0;
	else if (ReadCount < DataLength)DataLength = ReadCount;
	for (C = 0; C < DataLength; C++)
	{
		if (FIFO->PRead + FIFO->TypeSize > FIFO->Capacity)
		{
			RemainGranularity = FIFO->TypeSize + FIFO->PRead - FIFO->Capacity;
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize, (unsigned char*)FIFO->Pointer + FIFO->PRead, FIFO->TypeSize - RemainGranularity);
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize + FIFO->TypeSize - RemainGranularity, (unsigned char*)FIFO->Pointer, RemainGranularity);
			NewPRead = RemainGranularity;
		}
		else
		{
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize, (unsigned char*)FIFO->Pointer + FIFO->PRead, FIFO->TypeSize);
			NewPRead = FIFO->PRead + FIFO->TypeSize;
			if (NewPRead >= FIFO->Capacity)  NewPRead = 0;
		}
		//移动读指针
		FIFO->PRead = NewPRead;
	}

	return DataLength;
}

unsigned int FIFOVirtualRead(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength,unsigned int DataOffset)
{
	unsigned int ReadCount = FIFOGetReadByteSize(FIFO) / FIFO->TypeSize;
	unsigned int C = 0;
	unsigned short RemainGranularity;
	int NewPRead;
	unsigned int VirtualPread = FIFO->PRead;
	if (ReadCount == 0 || DataLength == 0) return 0;
	else if (ReadCount < (DataLength + DataOffset)) DataLength = ReadCount - DataOffset;
	if (DataLength == 0) return 0;

	if (VirtualPread + DataOffset* FIFO->TypeSize >= FIFO->Capacity) VirtualPread = VirtualPread + DataOffset * FIFO->TypeSize - FIFO->Capacity;
	else VirtualPread += DataOffset * FIFO->TypeSize;

	for (C = 0; C < DataLength; C++)
	{
		if (VirtualPread + FIFO->TypeSize > FIFO->Capacity)
		{
			RemainGranularity = FIFO->TypeSize + VirtualPread - FIFO->Capacity;
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize, (unsigned char*)FIFO->Pointer + VirtualPread, FIFO->TypeSize - RemainGranularity);
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize + FIFO->TypeSize - RemainGranularity, (unsigned char*)FIFO->Pointer, RemainGranularity);
			NewPRead = RemainGranularity;
		}
		else
		{
			FIFOMemcpy((unsigned char*)DataBuffer + C * FIFO->TypeSize, (unsigned char*)FIFO->Pointer + VirtualPread, FIFO->TypeSize);
			NewPRead = VirtualPread + FIFO->TypeSize;
			if (NewPRead >= FIFO->Capacity)  NewPRead = 0;
		}
		VirtualPread = NewPRead;
	}

	return DataLength;
}

unsigned char FIFOWrite(FIFO_S* FIFO, void* DataBuffer, unsigned int DataLength)
{
	unsigned int Temp, Temp2, Count;
	unsigned short RemainGranularity;
	int NewPWrite;
	//先计算缓冲区剩余空间是否够用 -1防止W追上R
	if (DataLength * FIFO->TypeSize + FIFOGetReadByteSize(FIFO) > FIFO->Capacity - 1) return 0;
	//计算新的Write写入点
	for (Count = 0; Count < DataLength; Count++)
	{

		if (FIFO->PWrite + FIFO->TypeSize > FIFO->Capacity)
		{
			RemainGranularity = FIFO->TypeSize + FIFO->PWrite - FIFO->Capacity;
			FIFOMemcpy((unsigned char*)FIFO->Pointer + FIFO->PWrite, (unsigned char*)DataBuffer + Count * FIFO->TypeSize, FIFO->TypeSize - RemainGranularity);
			FIFOMemcpy((unsigned char*)FIFO->Pointer, (unsigned char*)DataBuffer + Count * FIFO->TypeSize + FIFO->TypeSize - RemainGranularity, RemainGranularity);
			NewPWrite = RemainGranularity;
		}
		else
		{
			FIFOMemcpy((unsigned char*)FIFO->Pointer + FIFO->PWrite, (unsigned char*)DataBuffer + Count * FIFO->TypeSize, FIFO->TypeSize);
			NewPWrite = FIFO->PWrite + FIFO->TypeSize;
			if (NewPWrite >= FIFO->Capacity)NewPWrite = 0;
		}
		FIFO->PWrite = NewPWrite;
	}



	return 1;
}

unsigned char FIFOReadByte(FIFO_S* FIFO, void* Byte)
{
	unsigned int ReadCount = FIFOGetReadByteSize(FIFO);
	unsigned int C = 0;
	if (ReadCount == 0) return 0;

	(*(unsigned char*)Byte) = ((unsigned char*)(FIFO->Pointer))[FIFO->PRead];

	//移动读指针
	if (FIFO->PRead + 1 == FIFO->Capacity) FIFO->PRead = 0;
	else (FIFO->PRead)++;

	return 1;
}

unsigned char FIFOVirtualReadByte(FIFO_S* FIFO, void* Byte, unsigned int ByteOffset)
{
	unsigned int ReadCount = FIFOGetReadByteSize(FIFO);
	unsigned int C = 0;
	unsigned int VirtualPRead = FIFO->PRead + (ByteOffset % (FIFO->Capacity));


	if (ReadCount == 0) return 0;

	if (ByteOffset > ReadCount) return 0;

	if (VirtualPRead >= FIFO->Capacity) VirtualPRead -= FIFO->Capacity;

	(*(unsigned char*)Byte) = ((unsigned char*)(FIFO->Pointer))[VirtualPRead];

	return 1;
}

unsigned char FIFOMoveReadPoint(FIFO_S* FIFO, unsigned int ByteLength)
{
	unsigned int ReadCount = FIFOGetReadByteSize(FIFO);
	unsigned int C = 0;

	if (ReadCount == 0) return 0;
	if (ByteLength > ReadCount) return 0;

	if (FIFO->PRead + ByteLength >= FIFO->Capacity) FIFO->PRead = (FIFO->PRead) + ByteLength - FIFO->Capacity;
	else (FIFO->PRead) += ByteLength;

	return 1;
}

unsigned char FIFOWriteByte(FIFO_S* FIFO, void* Byte)
{
	//先计算缓冲区剩余空间是否够用 -1防止W追上R
	if (1 + FIFOGetReadByteSize(FIFO) > FIFO->Capacity - 1) return 0;
	//计算新的Write写入点
	int NewPWrite = FIFO->PWrite + 1;

	if (NewPWrite >= FIFO->Capacity)
	{
		NewPWrite = 0;
		((unsigned char*)(FIFO->Pointer))[FIFO->PWrite] = (*(unsigned char*)Byte);
	}
	else
	{
		((unsigned char*)(FIFO->Pointer))[FIFO->PWrite] = (*(unsigned char*)Byte);
	}
	FIFO->PWrite = NewPWrite;
	return 1;
}





