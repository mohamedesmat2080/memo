//
// Created by YUMBUL on 19.03.2023.
//
#pragma once


#include <memory/hook.h>
#include <Test/Test.h>
#include "GObjNPC.h"
#include "GObjPC.h"

class CGObjSiegeStruct : public CGObjNPC
{
public:
    unsigned int SiegeStructDamageMeter(unsigned short a1, int a2);

public:
    int UnkID; //0x1CD8
    char pad_1CDC[12]; //0x1CDC
private:
BEGIN_FIXTURE()
        ENSURE_SIZE(0x1CE8)
        ENSURE_OFFSET(UnkID, 0x1CD8)

    END_FIXTURE()

    RUN_FIXTURE(CGObjSiegeStruct)


}; //Size: 0x1CE8