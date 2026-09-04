#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/GIFCreator/IFGIFCreator.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFWheelFortune : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFWheelFortune)
GFX_DECLARE_MESSAGE_MAP(CIFWheelFortune)

private:
    CIFWheelFortune(void);
    ~CIFWheelFortune(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    void OnSpin();
private:
    CIFNormalTile* m_tile;
    CIFGIFCreator* m_wheel;
    int currentWheelIndex;
    CIFStatic* m_wheelspin;
    CIFButton* m_spin;
};