//
// Created by Kurama on 1/5/2022.
//
#pragma once

#include "IFFrame.h"
#include "ClassLink.h"
#include "IFDragableArea.h"
#include "IFStatic.h"
#include "IFButton.h"
#include "IFEdit.h"
#include "IFTextBox.h"
#include "IFStretchWnd.h"
#include "IFNormalTile.h"

enum BoxSections : char {
    MSGBOX_INIF = 1,
    MSGBOX_STORE = 2,
    MSGBOX_STORE_MONEY = 3,
    MSGBOX_INSERTMSG = 4,
    MSGBOX_DIVIDE_COUNT = 5,
    MSGBOX_STORE_CONFIRM = 6,
    MSGBOX_INSERT_COUPON_NUMBER = 7,
    MSGBOX_MENTOR_REGISTER = 8,
    MSGBOX_SIMPLE = 9,
    MSGBOX_TAX_MODIFY = 10,
    MSGBOX_TAX_LEVY = 11,
    MSGBOX_MAKE_ITEM = 12,
    MSGBOX_MAKE_ITEM_CNANEL = 13,
    MSGBOX_BLOCK_CONFIRM = 15,
    MSGBOX_LEARN_ERROR = 16
};

class CIFMessageBox : public CIFFrame {
GFX_DECLARE_DYNAMIC_EXISTING(CIFMessageBox, 0x00ee95a0)

    GFX_DECLARE_DYNCREATE_FN(CIFMessageBox)

GFX_DECLARE_MESSAGE_MAP(CIFMessageBox)
public:
    CIFMessageBox() {
        FUN_0063b4d0();
    };

    void OnTimer(int timerId) override;

    bool OnCreate(long ln) override;

    void OnUpdate() override;

    void SetGWndSize(int width, int height) override;

    void ShowGWnd(bool bVisible) override;

    bool SetText(const wchar_t *src) override;

public:
    void MakeMsgBox(BoxSections sectionId);

    void SetEditMsgBoxHandler(char Id);

    void SetMsgBoxHandler(int Id, int invSlot);

private:
    void FUN_0063b4d0();

    void UpdateLayout();

private: //Sections!
    void MsgBoxINIF();

    void MsgBoxStore();

    void MsgBoxStoreMoney();

    void MsgBoxInsertMsg();

    void MsgBoxDivideCount();

    void MsgBoxStoreConfirm();

    void MsgBoxInsertCouponNumber();

    void MentorMatchRegister();

    void MsgBoxSimple();

    void MsgBoxTaxModify();

    void MsgBoxTaxLevy();

    void MsgBoxMakeItem();

    void MsgBoxMakeItemCancel();

    int FUNC_63B420();

public:
    char pad_007b0[0x07c0 - 0x07b0]; //0x07B0 //class link ksmk!
private:
    char pad_07C0[4]; //0x07C0
    CIFDragableArea *m_Draghandler; //0x07C4
    CIFStatic *m_title; //0x07C8
    int m_SectionBoxId; //0x07AC
    char pad_07D0[48]; //0x07D0
    CIFButton *N00000258[4]; //0x0800
    char pad_0810[4]; //0x0810
    CIFStatic *N0000025D[2]; //0x0814
    char pad_081c[28]; //0x081C
    int m_NormalBoxHandlerId; //0x0838
    int N0000026D; //0x083C
    CIFStatic *N0000026E[5]; //0x0840
    char pad_0854[4]; //0x0854
    int N00000274; //0x0858
    char pad_085c[40]; //0x085C
    CIFEdit *N0000027F; //0x0884
    char pad_0888[4]; //0x0888
    CIFEdit *N00000281; //0x088C
    CIFStatic *N00000282; //0x0890
    CIFStretchWnd *N00000283; //0x0894
    CIFNormalTile *N00000284; //0x0898
    CIFStatic *N00000285[2]; //0x089C
    char pad_08A4[24]; //0x08A4
    int N0000028D; //0x08BC
    int N0000028E; //0x08C0
    char pad_08C4[60]; //0x08C4
    int m_EditBoxHnadlerId; //0x0900
    CIFEdit *N0000029F; //0x0904
    CIFStatic *N000002A0; //0x0908
    char pad_090C[4]; //0x090C
    CIFTextBox *N000002A2; //0x0910
    char pad_0914[920]; //0x0914

private:
BEGIN_FIXTURE()
        ENSURE_SIZE(0xCAC)

    END_FIXTURE()

    RUN_FIXTURE(CIFMessageBox)
};