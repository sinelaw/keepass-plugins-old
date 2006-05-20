/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plug-in
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005 Sebastian Schuberth <sschuberth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#if _MSC_VER>1200 // Modern Visual Studios need a customized StdAfx.h file.
    #include "StdAfx.h"
#else
    #include "../../StdAfx.h"
#endif
#include "../../Resource.h"

#include "PlugIn.h"

#include "../../PwSafeDlg.h"
#include "../../NewGUI/TranslateEx.h"
#include "../../Util/WinUtil.h"

#include <tchar.h>

#include "Oubliette/OublietteFile.h"
#include "PasswordDialog.h"

#define PLUGIN_NAME "Oubliette Import Plug-In"
#define GROUP_NAME "Imported from Oubliette"

static KP_APP_INFO g_kpAppInfo;
static KP_MENU_ITEM g_menuItems[1];

KP_EXP BOOL KP_API KeePluginInit(const KP_APP_INFO* pAppInfo,KP_PLUGIN_INFO* pPluginInfo) {
    if (pAppInfo==NULL || pPluginInfo==NULL)
        return FALSE;

    // Copy the application info structure.
    memcpy(&g_kpAppInfo, pAppInfo, sizeof(KP_APP_INFO));

    // Fill out the plug-in info structure.
    ZeroMemory(pPluginInfo,sizeof(*pPluginInfo));

    //                          1.0.3.a
    pPluginInfo->dwForAppVer=0x01000301;
    //                          1.9.0.a
    pPluginInfo->dwPluginVer=0x01090001;

    _tcscpy(pPluginInfo->tszPluginName,_T(PLUGIN_NAME));
    _tcscpy(pPluginInfo->tszAuthor,_T("Sebastian Schuberth"));

    pPluginInfo->dwNumCommands=1;
    pPluginInfo->pMenuItems=(KP_MENU_ITEM*)g_menuItems;

    g_menuItems[0].lpCommandString="&Oubliette OUB File...";
    g_menuItems[0].dwIcon=26;

    return TRUE;
}

KP_EXP BOOL KP_API KeePluginExit(LPARAM lParamW,LPARAM lParamL) {
    UNREFERENCED_PARAMETER(lParamW);
    UNREFERENCED_PARAMETER(lParamL);

    // We have not dynamically allocated any resources, so no clean-up is
    // needed.
    return TRUE;
}

KP_EXP BOOL KP_API KeePluginCall(DWORD dwCode,LPARAM lParamW,LPARAM lParamL) {
    UNREFERENCED_PARAMETER(lParamW);
    UNREFERENCED_PARAMETER(lParamL);

    CPwSafeDlg *pDlg=(CPwSafeDlg*)g_kpAppInfo.pMainDlg;
    HWND w=pDlg?pDlg->m_hWnd:GetDesktopWindow();

    // Cases are in the order they are called from Keepass.
    switch (dwCode) {
        case KPM_WND_INIT_POST: {
            if (!pDlg)
                break;

            // Inject the menu item into the "File - Import From" sub-menu. This
            // will fail if Keepass was not compiled using the same version of
            // Visual Studio as the plug-in.
            BCMenu *m=(BCMenu*)pDlg->m_menu.GetSubMenu((TCHAR*)_TRL("&File"));
            if (!m)
                break;

            m=m->GetSubBCMenu((TCHAR*)_TRL("&Import From"));
            if (!m)
                break;

            m->AppendMenu(MF_SEPARATOR);
            m->AppendODMenu(
                g_menuItems[0].lpCommandString,
                MF_STRING|MF_OWNERDRAW,
                g_menuItems[0].dwCommandID,
                &pDlg->m_ilIcons,
                25
            );

            break;
        }
        case KPM_INIT_MENU_POPUP: {
            if (!pDlg)
                break;

            if (pDlg->m_bFileOpen)
                g_menuItems[0].dwFlags&=~KPMIF_DISABLED;
            else
                g_menuItems[0].dwFlags|=KPMIF_DISABLED;

            break;
        }
        case KPM_DIRECT_CONFIG: {
            MessageBox(w,"This plug-in does not have any options.",PLUGIN_NAME,MB_OK|MB_ICONINFORMATION);
            break;
        }
        case KPM_PLUGIN_INFO: {
            MessageBox(w,"This plug-in imports Oubliette OUB files (see http://oubliette.sf.net/).",PLUGIN_NAME,MB_OK|MB_ICONINFORMATION);
            break;
        }
        case KPM_DIRECT_EXEC: {
            if (!pDlg)
                break;

            if (!pDlg->m_bFileOpen)
                MessageBox(w,"You need to open a database to import to first.",PLUGIN_NAME,MB_OK|MB_ICONWARNING);

            LPTSTR p=new TCHAR[(MAX_PATH+1)*2];
            pDlg->m_bDisplayDialog=TRUE;

            CWnd window;
            window.Attach(w);

            if (WU_GetFileNameSz(TRUE,_T("oub"),p,MAX_PATH*2))
                ImportOublietteFile(p,window);

            window.Detach();

            pDlg->m_bDisplayDialog=FALSE;
            delete [] p;

            break;
        }
    }

    return TRUE;
}

static PW_TIME ConvertDateTime(const DateTime& dt) {
    PW_TIME t;
    t.shYear=(USHORT)dt.getYear();
    t.btMonth=(BYTE)dt.getMonth();
    t.btDay=(BYTE)dt.getDay();
    t.btHour=(BYTE)dt.getHour();
    t.btMinute=(BYTE)dt.getMinute();
    t.btSecond=(BYTE)dt.getSecond();
    return t;
}

void ImportOublietteFile(LPCTSTR p,CWnd& w) {
    OublietteFile file(p);
    if (!file) {
        MessageBox(w.m_hWnd,file.getLastErrorMessage().c_str(),PLUGIN_NAME,MB_OK|MB_ICONWARNING);
        return;
    }

    CPasswordDialog dialog(&w);
    if (dialog.DoModal()!=IDOK)
        return;

    const OublietteFile::Header *header=file.decryptData((const TCHAR*)dialog.GetPassword());
    if (!header) {
        MessageBox(w.m_hWnd,file.getLastErrorMessage().c_str(),PLUGIN_NAME,MB_OK|MB_ICONWARNING);
        return;
    }

    if (!header->isValid()) {
        MessageBox(w,file.getLastErrorMessage().c_str(),PLUGIN_NAME,MB_OK|MB_ICONWARNING);
        return;
    }

    CPwSafeDlg *pDlg=(CPwSafeDlg*)g_kpAppInfo.pMainDlg;
    CPwManager *pMgr=(CPwManager*)g_kpAppInfo.pPwMgr;

    // Create a top-level group to import to.
    PW_GROUP g;
    ZeroMemory(&g,sizeof(PW_GROUP));
    // DWORD uGroupId;
    g.uImageId=49;  // Open folder icon.
    g.pszGroupName=_T(GROUP_NAME);
    g.tCreation=ConvertDateTime(header->getCreationTime());
    g.tLastMod=ConvertDateTime(header->getModificationTime());
    g.tLastAccess=ConvertDateTime(header->getModificationTime());
    pMgr->_GetNeverExpireTime(&g.tExpire);
    // USHORT usLevel;
    pMgr->AddGroup(&g);

    // Try to find Oubliette's path in the registry to read category names.
    CString ini;

    TCHAR buffer[256];
    ULONG size=OI_ARRAY_LENGTH(buffer);
    ZeroMemory(buffer,sizeof(buffer));

#if _MSC_VER>1200
    CRegKey r;
    if (r.Open(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Oubliette_is1",KEY_READ)==ERROR_SUCCESS) {
        size=OI_ARRAY_LENGTH(buffer);
        r.QueryStringValue("Inno Setup: App Path",buffer,&size);
        ini=buffer;
        ini+="\\Oubliette.ini";
    }
#else
    HKEY key;
    RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Oubliette_is1",&key);
    RegQueryValueEx(key,"Inno Setup: App Path",NULL,NULL,(unsigned char*)buffer,&size);
    RegCloseKey(key);
    ini=buffer;
    ini+="\\Oubliette.ini";
#endif

    g.usLevel=1;

    // Loop over all entries and add them to Keepass.
    PW_ENTRY e;
    for (int i=0;i<header->getCount();++i) {
        OublietteFile::Account account=file.processNext();

        // Add a "Subgroup" for every Oubliette category.
        CString category;
        category.Format(_T("Category%i"),account.category+1);
        GetPrivateProfileString("Categories",category,category,buffer,size,ini);
        g.uImageId=account.category;
        g.pszGroupName=buffer;
        // Create each category only once.
        DWORD dwGroupId=pMgr->GetGroupId(g.pszGroupName);
        if (dwGroupId==DWORD_MAX) {
            pMgr->AddGroup(&g);
            dwGroupId=pMgr->GetGroupId(g.pszGroupName);
        }

        ZeroMemory(&e,sizeof(PW_ENTRY));
        // BYTE uuid[16];
        e.uGroupId=dwGroupId;
        e.uImageId=g.uImageId;
        e.pszTitle=(TCHAR*)account.name.c_str();
        e.pszURL=(TCHAR*)account.url.c_str();
        e.pszUserName=(TCHAR*)account.username.c_str();
        e.pszPassword=(TCHAR*)account.password.c_str();
        e.uPasswordLen=_tcslen(e.pszPassword);
        std::string additional
            ="Oubliette \"Email\":\n"+account.email
            +"\n\n"
            +"Oubliette \"Note\":\n"+account.note
            +"\n\n"
            +"Oubliette \"Memo\":\n"+account.memo;
        e.pszAdditional=(TCHAR*)additional.c_str();

        e.tCreation=ConvertDateTime(account.created);
        e.tLastMod=ConvertDateTime(account.created);
        e.tLastAccess=g.tLastAccess;
        if (account.expires.isValid())
            e.tExpire=ConvertDateTime(account.expires);
        else
            pMgr->_GetNeverExpireTime(&e.tExpire);

        // TCHAR *pszBinaryDesc;
        // BYTE *pBinaryData;
        // DWORD uBinaryDataLen;

        pMgr->AddEntry(&e);
    }

    pDlg->m_bModified=TRUE;
    pDlg->UpdateGroupList();
    pDlg->UpdatePasswordList();
    int nItems=pDlg->m_cList.GetItemCount();
    pDlg->m_cList.EnsureVisible(nItems-1,FALSE);
    pDlg->_UpdateToolBar();
}
