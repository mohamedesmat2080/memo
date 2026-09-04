#pragma once

#include "BSLib/BSLib.h"
#include <Game.h>
#include <IFSlotWithHelp.h>

void Setup();

std::n_wstring getFullMsg(int indexOffset);
std::string getCharName(std::n_wstring fullMsg);
std::string getFullMsgA(std::n_wstring fullMsg);
int getEmptySlotsCount();
void disableItemLog(bool disable);
bool DoesFileExists(const std::string &name);
void patchAlchemy();
void InstallRuntimeClasses(CGame *);

void RegisterObject(const CGfxRuntimeClass *);

typedef void(*overrideFnPtr)();

extern std::vector<overrideFnPtr> override_objects;

template<typename T, int address>
void OverrideRtClassAt() {
    CGfxRuntimeClass *rt = (CGfxRuntimeClass *) address;

    rt->m_pfnCreateObject = T::CreateObject;
    rt->m_pfnDeleteObject = T::DeleteObject;
}

template<typename T, int address>
void OverrideObject() {
    override_objects.push_back(&OverrideRtClassAt<T, address>);
}
template<typename dst_type, typename src_type>
dst_type union_cast(src_type src)
{
    union {
        src_type s;
        dst_type d;
    }u;
    u.s = src;
    return u.d;
}