#include <BSLib/_internal/_BSDynamic.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFCharRankSlot : public CIFWnd
{
    GFX_DECLARE_DYNCREATE(CIFCharRankSlot)
public:
    CIFCharRankSlot(void);
    ~CIFCharRankSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
    CIFStatic* m_ranktext;
    CIFStatic* m_gradetext;
    CIFStatic* m_charnametext;
    CIFStatic* m_leveltext;
    CIFStatic* m_pointstext;
    CIFStatic* m_guildtext;
};