#include "HWIDGenerator.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <vector>

std::string HWIDGenerator::GenerateHWID() {
    std::string hwid = "";

    // Sabit bilgileri topla
    std::string biosInfo = GetBIOSInfo();
    std::string diskInfo = GetDiskInfo();
    std::string cpuInfo = GetCPUInfo();

    // Donanım bilgilerini birleştir
    hwid = biosInfo + diskInfo + cpuInfo;
    hwid = hwid + "1999";
    return hwid;
}

std::string HWIDGenerator::GetBIOSInfo() {
    std::string biosInfo = "";

    // BIOS Seri Numarasını al
    std::string serialNumber = "";
    DWORD dwSize = 0;
    GetSystemFirmwareTable('RSMB', 0, NULL, dwSize);
    if (dwSize != 0) {
        std::vector<BYTE> data(dwSize);
        if (GetSystemFirmwareTable('RSMB', 0, &data[0], dwSize) == dwSize) {
            serialNumber = std::string(reinterpret_cast<char*>(&data[0]) + 4, 20);
        }
    }
    biosInfo = serialNumber;

    return biosInfo;
}

std::string HWIDGenerator::GetDiskInfo() {
    std::string diskInfo = "";

    // Sabit Disk Seri Numarasını al
    DWORD dwVolumeSerialNumber;
    GetVolumeInformationA("C:\\", NULL, 0, &dwVolumeSerialNumber, NULL, NULL, NULL, 0);
    std::stringstream ss;
    ss << std::hex << dwVolumeSerialNumber;
    diskInfo = ss.str();

    return diskInfo;
}

std::string HWIDGenerator::GetCPUInfo() {
    std::string cpuInfo = "";

    unsigned long s1 = 0;
    unsigned long s2 = 0;
    unsigned long s3 = 0;
    unsigned long s4 = 0;
    __asm
    {
    mov eax, 00h
    xor edx, edx
    cpuid
    mov s1, edx
    mov s2, eax
    }
    __asm
    {
    mov eax, 01h
    xor ecx, ecx
    xor edx, edx
    cpuid
    mov s3, edx
    mov s4, ecx
    }

    static char buf[100];
    sprintf_s(buf, "%08X%08X%08X%08X", s1, s2, s3, s4);
    cpuInfo = buf;
    return cpuInfo;
}