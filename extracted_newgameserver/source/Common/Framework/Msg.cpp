//
// Created by Kurama on 12/25/2022.
//
#include <exception>
#include "Msg.h"

// todo : need to use the jmx exception

void CMsg::AddSizeToMsg(WORD wSize) {
    if (wSize > (~(-MSG_ENC_MASK)))
        throw std::exception("Ooops");

    // Lets Enc it back
    *m_wpMsgSize = (wSize | (*m_wpMsgSize & MSG_ENC_MASK));
}
void CMsg::Read(void* dest, __int16 count)
{
    __asm pushad;
    __asm pushfd;

    //Setup arguments.
    __asm push dest;
    __asm mov esi, this;
    __asm mov di, count;

    //Call the function.
    __asm mov edx, 0x00404C20;
    __asm call edx;

    __asm popfd;
    __asm popad;
}
void CMsg::ReadBytes(void *pOut, size_t cbSize) {
    if ((m_wReadDataArrayPos + cbSize) > m_wWriteDataArrayPos)
        throw std::exception("Ooops");

    memcpy(pOut, &m_pMsgBuffer[m_wReadDataArrayPos], cbSize);
    m_wReadDataArrayPos += cbSize;
}

void CMsg::ReadBytesReverse(const void *pOut, size_t cbSize) {
    if ((m_wWriteDataArrayPos - MSG_HEADER_SIZE) < cbSize)
        throw std::exception("Ooops");

    m_wWriteDataArrayPos -= cbSize;
    pOut = &m_pMsgBuffer[m_wWriteDataArrayPos];

    AddSizeToMsg((m_wWriteDataArrayPos - MSG_HEADER_SIZE));
}

void CMsg::WriteBytes(const void *pIn, size_t cbSize) {
    if (m_wWriteDataArrayPos + cbSize > m_dwArrayDataSize) {
        pIn = NULL;
        return;
    }

    memcpy(&m_pMsgBuffer[m_wWriteDataArrayPos], pIn, cbSize);
    m_wWriteDataArrayPos += cbSize;
    AddSizeToMsg((m_wWriteDataArrayPos - MSG_HEADER_SIZE));
}

void CMsg::WriteString(const char *pChar) {
    size_t cbLength = 0;

    if (pChar != NULL)
        cbLength = ::lstrlenA(pChar);

    if ((m_wWriteDataArrayPos + cbLength) >= m_dwArrayDataSize)
        cbLength = (m_dwArrayDataSize - m_wWriteDataArrayPos) - sizeof(char);

    (*this) << (WORD) cbLength;
    if (cbLength != 0)
        WriteBytes(pChar, cbLength);
}

void CMsg::WriteString(const std::string &str) {
    size_t cbLength = str.length();

    if ((m_wWriteDataArrayPos + cbLength) >= m_dwArrayDataSize)
        cbLength = (m_dwArrayDataSize - m_wWriteDataArrayPos) - sizeof(char);

    (*this) << (WORD) cbLength;
    if (cbLength != 0)
        WriteBytes(str.c_str(), cbLength);
}
void CMsg::WriteStringW(const std::wstring& str)
{
    this->Write<WORD>(str.length());
    this->Write(str.c_str(), str.length() * 2);
}

void CMsg::ReadStringW(std::wstring& str)
{
    short sLength = 0;
    (*this) >> sLength;

    str.resize(sLength * 2);
    ReadBytes(&str[0], sLength * 2);
}
std::n_wstring CMsg::ReadNStringW()
{
    WORD len = Read<WORD>();

    wchar_t* buffer = new wchar_t[len + 1]; // +1 for null terminator
    Read(buffer, len * sizeof(wchar_t));
    buffer[len] = L'\0'; // Null terminate the string

    std::n_wstring result(buffer);

    delete[] buffer; // Don't forget to release memory

    return result;
}
void CMsg::ReadString(std::string &str) {
    short sLength = 0;
    (*this) >> sLength;

    str.resize(sLength);
    ReadBytes(&str[0], sLength);
}

void CMsg::Write(const void* src, size_t count)
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
    __asm mov edx, 0x00404090;
    __asm call edx;

    __asm popfd;
    __asm popad;
}