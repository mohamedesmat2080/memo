#include "MsgStreamBuffer.h"

CMsgStreamBuffer::FN_CONSTRUCTOR CMsgStreamBuffer::s_pfnConstructor = NULL;
CMsgStreamBuffer::FN_READ CMsgStreamBuffer::s_pfnRead = NULL;
CMsgStreamBuffer::FN_WRITE CMsgStreamBuffer::s_pfnWrite = NULL;
CMsgStreamBuffer::FN_SEND CMsgStreamBuffer::s_pfnSend = NULL;

#define CONSTRUCTOR_OFFSET	0x0053CEA0
#define READ_OFFSET			0x004F7220
#define WRITE_OFFSET		0x00508FE0
#define SEND_OFFSET			0x008418D0

void CMsgStreamBuffer::Initialize()
{
    s_pfnConstructor = reinterpret_cast<FN_CONSTRUCTOR>(CONSTRUCTOR_OFFSET);
    s_pfnRead = reinterpret_cast<FN_READ>(READ_OFFSET);
    s_pfnWrite = reinterpret_cast<FN_WRITE>(WRITE_OFFSET);
    s_pfnSend = reinterpret_cast<FN_SEND>(SEND_OFFSET);
}

CMsgStreamBuffer::CMsgStreamBuffer(uint16_t id)
{
    s_pfnConstructor(this, id);
}

size_t CMsgStreamBuffer::GetOffset() const
{
    return m_offset;
}

size_t CMsgStreamBuffer::GetLength() const
{
    return m_length;
}

size_t CMsgStreamBuffer::GetRemainRead() const
{
    return (GetLength() - GetOffset());
}

uint16_t CMsgStreamBuffer::GetID() const
{
    return m_id;
}

SMsgStreamNode* CMsgStreamBuffer::GetFrontNode() const
{
    return m_pFrontNode;
}

SMsgStreamNode* CMsgStreamBuffer::GetCurrentNode() const
{
    return m_pCurNode;
}

size_t CMsgStreamBuffer::GetNodeCount() const
{
    int count = (GetLength() / MSG_NODE_BUFFER_SIZE);
    int tail = (GetLength() % MSG_NODE_BUFFER_SIZE);
    if (tail > 0)
        ++count;
    return count;
}

size_t CMsgStreamBuffer::Read(void* dest, size_t count)
{
    return s_pfnRead(this, dest, count);
}

std::string CMsgStreamBuffer::ReadStringA()
{
    uint16_t len = Read<uint16_t>();

    std::string str;
    str.resize(len);
    Read(&str[0], len);

    return str;
}

std::wstring CMsgStreamBuffer::ReadStringW()
{
    uint16_t len = Read<uint16_t>();

    std::wstring str;
    str.resize(len * 2);
    Read(&str[0], len * 2);

    return str;
}

void* CMsgStreamBuffer::Write(const void* src, size_t count)
{
    return s_pfnWrite(this, src, count);
}

void CMsgStreamBuffer::WriteStringA(const std::string& str)
{
    Write<uint16_t>(str.length());
    Write(&str[0], str.length());
}

void CMsgStreamBuffer::WriteStringW(const std::wstring& str)
{
    Write<uint16_t>(str.length());
    Write(&str[0], str.length() * 2);
}

void CMsgStreamBuffer::Seek(size_t offset)
{
    m_offset = offset;
}

void CMsgStreamBuffer::Skip(size_t count)
{
    m_offset += count;
}

void CMsgStreamBuffer::Send()
{
    s_pfnSend(this);
}