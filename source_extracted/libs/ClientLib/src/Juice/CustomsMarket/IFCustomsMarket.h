#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
class CIFCustomsMarket : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFCustomsMarket)
GFX_DECLARE_MESSAGE_MAP(CIFCustomsMarket)

private:
    CIFCustomsMarket(void);
    ~CIFCustomsMarket(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    void OnBuyTitle();
    void OnBuyTitleColor();
    void OnBuyNameColor();
private:
    CIFFrame* m_frame;
    CIFFrame* m_frame2;
    CIFFrame* m_frame3;
    CIFNormalTile* m_tile;
    CIFNormalTile* m_tile2;
    CIFNormalTile* m_tile3;
    CIFNormalTile* m_tile4;
    CIFStatic* m_head1;
    CIFStatic* m_head2;
    CIFStatic* m_head3;
    CIFStatic* m_fee1;
    CIFStatic* m_fee2;
    CIFStatic* m_fee3;
    CIFStatic* m_info1;
    CIFEdit* m_edit;
public:
    bool showColorPicker;
    int colorPicker1;
    int colorPicker2;
    CIFStatic* m_example1;
    CIFStatic* m_example2;
    CIFButton* m_buy1;
    CIFButton* m_buy2;
    CIFButton* m_buy3;
};