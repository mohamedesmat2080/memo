#include "IFflorian0Guide.h"
#include "../../../DevKit_DLL/src/Util.h"
#include "GEffSoundBody.h"
#include "GInterface.h"
#include <BSLib/Debug.h>
#include <Juice/ItemChest/IFItemChest.h>

GFX_IMPLEMENT_DYNCREATE(CIFflorian0Guide, CIFDecoratedStatic)


bool CIFflorian0Guide::OnCreate(long ln)
{
	BS_DEBUG_LOW("> " __FUNCTION__ "(%d)", ln);
	CIFDecoratedStatic::OnCreate(ln);

	TB_Func_13("interface\\bimbum\\itemmall_1.ddj", 0, 0);
	sub_634470("interface\\bimbum\\itemmall_2.ddj");

	set_N00009BD4(20);
	set_N00009BD3(100);

	CGEffSoundBody::get()->PlaySound(L"snd_quest");

	return true;
}

int CIFflorian0Guide::OnMouseLeftUp(int a1, int x, int y)
{
    CIFItemChest* chestWnd = (CIFItemChest *) g_pCGInterface->GetWindowByUniqueID(13395);
    if(chestWnd)
    {
        chestWnd->MoveToCenter();

        chestWnd->ShowGWnd(true);

        chestWnd->m_spin->index = 1;
        chestWnd->m_spin->numStatic->SetText(L"1");

        CMsgStreamBuffer buf(0x180F);
        buf << byte(0x00) << byte(0x01);
        SendMsg(buf);
    }
	return 0;
}

void CIFflorian0Guide::OnCIFReady()
{
	BS_DEBUG_LOW("> " __FUNCTION__);

	CIFDecoratedStatic::OnCIFReady();
	sub_633990();

}
