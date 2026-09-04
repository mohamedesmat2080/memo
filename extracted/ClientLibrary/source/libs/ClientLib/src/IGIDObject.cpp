#include "IGIDObject.h"

const std::n_wstring &CIGIDObject::GetName() const {
    return m_name;
}

const SCommonData *CIGIDObject::GetCommonData() const {
    return m_commonData;
}

const int CIGIDObject::GetUniqueId() const {
    return m_uniqueId;
}


void CIGIDObject::ChangeTitleColor(D3DCOLOR Color)
{
    fonttexture_title.SetColor(Color);
}


void CIGIDObject::ChangeName(std::n_wstring Name)
{
    fonttexture_playername.sub_8B3B60(&Name);
}

void CIGIDObject::ChangeTitle(std::n_wstring Title)
{
    fonttexture_title.sub_8B3B60(&Title);
}


bool CIGIDObject::IsChinese()
{
    return this->GetCommonData()->RefObjectId >= 1907 && this->GetCommonData()->RefObjectId <= 1932;
}
bool CIGIDObject::IsEurope()
{
    return this->GetCommonData()->RefObjectId >= 14875 && this->GetCommonData()->RefObjectId <= 14900;
}