//
// Created by Kurama on 12/14/2022.
//

#include "MainProcess.h"

#include "Game.h"

void CMainProcess::_OnProcessMessage(MSG_HANDLE) {
    if (g_pCGame == NULL)
        throw;

    g_pCGame->ProcessMessage(pMsg);
}
