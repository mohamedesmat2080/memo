#pragma once
#include <string>
#include <vector>

class HWIDGenerator {
public:
    std::string GenerateHWID();
    bool IsVirtualMachine();
private:
    static std::string GetBIOSInfo();
    static std::string GetDiskInfo();
    static std::string GetCPUInfo();
};
