#include "MsgStreamBuffer.h"
#include "MemoryUtility.h"
//TODO: Update for shard.
#define MSG_WRITE_BYTES_FN_OFFSET		0x00402DF0
#define MSG_READ_BYTES_FN_OFFSET		0x00403DF0

WORD CMsgStreamBuffer::GetID() const
{
	void* pAddr = MEMUTIL_READ_BY_PTR_OFFSET(this, 0x1050, void*);
	return *(WORD*)(pAddr);
}

WORD CMsgStreamBuffer::GetReadPos() const
{
	return MEMUTIL_READ_BY_PTR_OFFSET(this, 0x103C, WORD);
}

WORD CMsgStreamBuffer::GetWritePos() const
{
	return MEMUTIL_READ_BY_PTR_OFFSET(this, 0x103E, WORD);
}

void CMsgStreamBuffer::SetMsgID(WORD wMsgID)
{
	void* pAddr = MEMUTIL_READ_BY_PTR_OFFSET(this, 0x1050, void*);
	*(WORD*)pAddr = wMsgID;
}

void CMsgStreamBuffer::SetReadPos(WORD wPos)
{
	MEMUTIL_WRITE_BY_PTR_OFFSET(this, 0x103C, WORD, wPos);
}

void CMsgStreamBuffer::SetWritePos(WORD wPos)
{
	MEMUTIL_WRITE_BY_PTR_OFFSET(this, 0x103E, WORD, wPos);
}

void CMsgStreamBuffer::Read(void* dest, __int16 count)
{
	__asm pushad;
	__asm pushfd;

	//Setup arguments.
	__asm push dest;
	__asm mov esi, this;
	__asm mov di, count;

	//Call the function.
	__asm mov edx, MSG_READ_BYTES_FN_OFFSET;
	__asm call edx;

	__asm popfd;
	__asm popad;
}

std::string CMsgStreamBuffer::ReadStringA()
{
	WORD len = Read<WORD>();

	std::string str(len, 0);
	Read(&str[0], len);

	return str;
}



//__int16 __userpurge DEC_WritePacketBytes_sub_404090@<ax>(unsigned __int16 size_2@<ax>, srCMsgStreamBuffer_data *packet@<ecx>, void *Src)
void CMsgStreamBuffer::Write(const void* src, size_t count)
{
	__asm pushad;
	__asm pushfd;

	//const void* data
	__asm push src;

	//MsgStreamBuffer* msg
	__asm mov ecx, this;

	//Count of bytes to write, on ax but we don't really care.
	__asm mov eax, count;

	//Addr for call.
	__asm mov edx, MSG_WRITE_BYTES_FN_OFFSET;
	__asm call edx;

	__asm popfd;
	__asm popad;
}

void CMsgStreamBuffer::WriteStringA(const std::string& str)
{
	this->Write<WORD>(str.length());
	this->Write(str.c_str(), str.length());
}
