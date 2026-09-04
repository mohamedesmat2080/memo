#include <IFSlotWithHelp.h>
#include <GInterface.h>

void CIFSlotWithHelp::sub_686C50(DWORD32 a2) const {

    reinterpret_cast<void(__thiscall*)(const CIFSlotWithHelp*, DWORD32 a2)>(0x686C50)(this,a2);
}
void CIFSlotWithHelp::sub_686DB0() const {

    reinterpret_cast<void(__thiscall*)(const CIFSlotWithHelp*)>(0x686DB0)(this);
}

void CIFSlotWithHelp::sub_66B460(DWORD32 a2) const {

    reinterpret_cast<void(__thiscall*)(const CIFSlotWithHelp*, DWORD32 a2)>(0x66B460)(this, a2);

}
void CIFSlotWithHelp::sub_67CFB0() const
{
    reinterpret_cast<void(__thiscall*)(const CIFSlotWithHelp*)>(0x0067CFB0)(this);
}
float CIFSlotWithHelp::sub_6871d0(int a2) const
{
    return reinterpret_cast<float(__thiscall*)(const CIFSlotWithHelp*,int)>(0x006871d0)(this,a2);
}
char CIFSlotWithHelp::sub_66B460(int a2) const
{
    return reinterpret_cast<char(__thiscall*)(const CIFSlotWithHelp*,int)>(0x0066B460)(this,a2);
}
void CIFSlotWithHelp::SetType(int value)
{
    this->Type = value;
}

int CIFSlotWithHelp::GetType()
{
    return this->Type;
}
