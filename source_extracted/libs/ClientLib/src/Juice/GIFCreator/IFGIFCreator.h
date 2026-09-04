#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFGIFCreator : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFGIFCreator)
GFX_DECLARE_MESSAGE_MAP(CIFGIFCreator)

private:
    CIFGIFCreator(void);
    ~CIFGIFCreator(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    CIFStatic* m_ddj;
    std::string ddjPath;
    int ddjCurrentIndex;
    int ddjCount;
    float ddjMinSpeed;
    float ddjMaxSpeed;
    float ddjSpeedDelay;
    bool operational;
    bool autoRepeat;
    int wheelChoosenIndex;
};