#include "../../IFMainFrame.h"

class CIFMainMenu : public CIFWnd {

    GFX_DECLARE_DYNCREATE(CIFMainMenu)
    GFX_DECLARE_MESSAGE_MAP(CIFMainMenu)

public:
    CIFMainMenu(void);
    ~CIFMainMenu(void);

    bool OnCreate(long ln) override;
    void OnUpdate() override;

private:
    void OnToggle();
    void Toggle(bool show);
    void OnRank();
    void OnUnique();
    void OnEvent();
    void OnTitle();
    void OnMarket();
    void OnDaily();
private:
    CIFButton* toggleBtn;
    CIFStatic* mainMenu;
    CIFStatic* circlePic;
    CIFStatic* charType;
    CIFStatic* charName;
    CIFStatic* charGuild;
    CIFButton* rankBtn;
    CIFButton* uniqueBtn;
    CIFButton* scheduleBtn;
    CIFButton* titleBtn;
    CIFButton* marketBtn;
    CIFButton* dailyBtn;
    std::list<CIFWnd*> getControls;
};