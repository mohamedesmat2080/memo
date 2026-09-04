#pragma once
#include <IFSelectableArea.h>
#include <IFCheckBox.h>
#include <IFScrollManager.h>
#include <IObject.h>
#include <IFEdit.h>
#include "d3dx9math.h"

class CIFMacroMenuPickFilter : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFMacroMenuPickFilter)
GFX_DECLARE_MESSAGE_MAP(CIFMacroMenuPickFilter)
private:
    CIFMacroMenuPickFilter(void);
    ~CIFMacroMenuPickFilter(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    int Func_4(int a2) override;
    void OnUnknownStuff();
    CIFSelectableArea** m_pTabsSecond;
    static const int numberOfTabs = 2;
    static const int tabWidth = 200;
    static const int tabHeight = 24;
    static const int tabFirstId = 300;
    undefined1 OnCloseWnd() override;
public:
    bool PetPickTimerIsRunning;
    void ActivateTabPage(BYTE page);
    void SelectPickPetOrSelf();
    bool Macro_PetFilter;
    CIFCheckBox* PickViaPetCheckBox;
    CIFCheckBox* PickViaCharCheckBox;
    CIFCheckBox* OnlyRareEquiptsCB;
    CIFCheckBox* DontPickGoldCB;
    CIFCheckBox* DontPickAlchemytablesCB;
    CIFCheckBox* DontPickAlchemyStonesCB;
    CIFCheckBox* DontPickElixirsCB;
    CIFCheckBox* DontPickArrowCB;
    CIFCheckBox* DontPickReturnCB;
    CIFCheckBox* DontPickTrashCB;

    CIFCheckBox* PickDg1CB;
    CIFCheckBox* PickDg2CB;
    CIFCheckBox* PickDg3CB;
    CIFCheckBox* PickDg4CB;
    CIFCheckBox* PickDg5CB;
    CIFCheckBox* PickDg6CB;
    CIFCheckBox* PickDg7CB;
    CIFCheckBox* PickDg8CB;
    CIFCheckBox* PickDg9CB;
    CIFCheckBox* PickDg10CB;
    CIFCheckBox* PickDg11CB;
    CIFCheckBox* PickDg12CB;
    CIFCheckBox* PickDg13CB;
    CIFCheckBox* PickDg14CB;
    CIFCheckBox* PickDg15CB;
    CIFCheckBox* PickDg16CB;
    CIFCheckBox* PickDg17CB;
    CIFCheckBox* PickDg18CB;
    CIFCheckBox* DontPickHpMp;
    CIFCheckBox* DontPickVigor;
    void PickWithPet();
    void CancelBtn();
    void SaveButton();
};