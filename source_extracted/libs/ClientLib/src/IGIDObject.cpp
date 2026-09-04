#include "IGIDObject.h"
#include "GameDataExt.h"

const std::n_wstring &CIGIDObject::GetName() const {
    return m_name;
}

const CCharacterData::SData *CIGIDObject::GetSData() const {
    return m_characterData;
}

const int CIGIDObject::GetUniqueId() const {
    return m_uniqueId;
}

void CIGIDObject::UpdateNameColor(UINT32 color) {
    std::map<std::n_wstring, int>::iterator findCharNameColor =  m_CharNameColor.find(this->GetName().c_str());
    if(findCharNameColor != m_CharNameColor.end() && color != 0xFF62F5B1)
    {
        reinterpret_cast<void(__thiscall*)(CIGIDObject*, UINT32)>(0x009C1920)(this, findCharNameColor->second);
    }
    else
        reinterpret_cast<void(__thiscall*)(CIGIDObject*, UINT32)>(0x009C1920)(this, color);
}

