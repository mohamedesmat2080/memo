#pragma once

union uregion {
	struct {
		char y;
		char x;
	} single;
	short r;
};

#define g_CurrentIfUnderCursor (*(CGWndBase**)0x0110F608)

#define g_Region (*(uregion*)0xEEF68C)

#define CLIENT_SCREEN_WIDTH (*(int*)0x00ED7CB8)
#define CLIENT_SCREEN_HEIGHT (*(int*)0x00ED7CBC)

#define SendMsg(x) reinterpret_cast<void (__cdecl *)(CMsgStreamBuffer&)>(0x008418D0)(x)
class CDummyScene;
#define g_CurrentScene (*((CDummyScene**)0x00EECEA4))
#define SendMsgClient(a) reinterpret_cast<void (__thiscall *)(void* pSender,CMsgStreamBuffer&)>(0x00BA8F70)(g_CurrentScene,a)
