#pragma once
#include "IFDecoratedStatic.h"

#define GUIDE_FLORIAN0 13378
#define GUIDE_CHEST 13396
class CIFflorian0Guide : public CIFDecoratedStatic
{
	GFX_DECLARE_DYNCREATE(CIFflorian0Guide)

public:
	bool OnCreate(long ln) override;
	int OnMouseLeftUp(int a1, int x, int y) override;
	void OnCIFReady() override;
};
