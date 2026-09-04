#include "IFGIFCreator.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>
#include <ctime>
#include <sstream>

GFX_IMPLEMENT_DYNCREATE(CIFGIFCreator, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFGIFCreator, CIFWnd)
GFX_END_MESSAGE_MAP()

time_t lastUpdate = clock();

CIFGIFCreator::CIFGIFCreator(void)
{
}
CIFGIFCreator::~CIFGIFCreator(void)
{
}
bool CIFGIFCreator::OnCreate(long ln)
{
    CIFWnd::OnCreate(ln);

    RECT m_wheelRect = { 0,0,GetSize().width,GetSize().height };
    m_ddj = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_wheelRect, 0, 0);

    return true;
}
void CIFGIFCreator::OnUpdate()
{
    if(operational)
    {
        if(float(clock() - lastUpdate) / CLOCKS_PER_SEC > ddjMinSpeed)
        {
            lastUpdate = clock();

            std::ostringstream temp;
            temp << ddjCurrentIndex;
            std::string num = temp.str();

            std::string currentDDJ = ddjPath + num + ".ddj";

            m_ddj->TB_Func_13(currentDDJ.c_str(),0,0);

            ddjCurrentIndex++;

            if(ddjCurrentIndex == ddjCount)
            {
                ddjCurrentIndex = 0;

                if(!autoRepeat)
                    ddjMinSpeed = ddjMinSpeed + ddjSpeedDelay;
            }

            if(!autoRepeat && ddjMinSpeed >= ddjMaxSpeed)
            {
                if(wheelChoosenIndex > 0)
                {
                    if(ddjCurrentIndex == wheelChoosenIndex)
                        operational = false;
                }
                else
                    operational = false;
            }
        }
    }
}
