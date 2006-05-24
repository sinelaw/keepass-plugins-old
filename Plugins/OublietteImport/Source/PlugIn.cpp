/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plug-in
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2006 Sebastian Schuberth <sschuberth@gmail.com>
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

#include "StdAfx.h"
#include "PlugIn.h"

#include "Oubliette/OublietteFile.h"
#include "PasswordDialog.h"

#include "../../KeePassLibCpp/PwManager.h"
#include "../../WinGUI/Util/WinUtil.h"

// The first three digits of the plug-in version number match the KeePass
// version number for which the plug-in was compiled. The last digit marks
// the release number of the plug-in for this particular KeePass version.
#define OUB_IMP_FORAPP  0x01000501
#define OUB_IMP_VERSION 0x01000501

#define PLUGIN_NAME   _T("Oubliette Import Plug-In")
#define PLUGIN_AUTHOR _T("Sebastian Schuberth")

#define GROUP_NAME "Imported from Oubliette"

static KP_APP_INFO g_kpAppInfo;
static KP_MENU_ITEM g_menuItems[1];

KP_EXP BOOL KP_API KeePluginInit(const KP_APP_INFO* pAppInfo,KP_PLUGIN_INFO* pPluginInfo) {
    if (pAppInfo==NULL || pPluginInfo==NULL)
        return FALSE;

    // Copy the application info structure.
    memcpy(&g_kpAppInfo,pAppInfo,sizeof(g_kpAppInfo));

    // Fill out the plug-in info structure.
    ZeroMemory(pPluginInfo,sizeof(*pPluginInfo));

    pPluginInfo->dwForAppVer=OUB_IMP_FORAPP;
    pPluginInfo->dwPluginVer=OUB_IMP_VERSION;

    _tcscpy_s(pPluginInfo->tszPluginName,PLUGIN_NAME);
    _tcscpy_s(pPluginInfo->tszAuthor,PLUGIN_AUTHOR);

    pPluginInfo->dwNumCommands=1;
    pPluginInfo->pMenuItems=(KP_MENU_ITEM*)g_menuItems;

    g_menuItems[0].lpCommandString=_T("&Oubliette OUB File...");
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
    UNREFERENCED_PARAMETER(lParamL);

    // Cases are in the order they are called from KeePass.
    switch (dwCode) {
        case KPM_WND_INIT_POST: {
            KP_Call(KPC_INSERT_IMPORTFROM_ITEM,0,0,0);
            KP_Call(
                KPC_INSERT_IMPORTFROM_ITEM,
                (LPARAM)g_menuItems[0].lpCommandString,
                g_menuItems[0].dwCommandID,
                25
            );
            break;
        }
        case KPM_INIT_MENU_POPUP: {
            if (KP_Query(KPQ_FILEOPEN,0))
                g_menuItems[0].dwFlags&=~KPMIF_DISABLED;
            else
                g_menuItems[0].dwFlags|=KPMIF_DISABLED;
            break;
        }
        case KPM_DIRECT_CONFIG: {
            MessageBox(
                g_kpAppInfo.hwndMain,
                _T("This plug-in does not have any options."),
                PLUGIN_NAME,
                MB_OK|MB_ICONINFORMATION
            );
            break;
        }
        case KPM_PLUGIN_INFO: {
            MessageBox(
                g_kpAppInfo.hwndMain,
                _T("This plug-in imports Oubliette OUB files (see http://oubliette.sf.net/)."),
                PLUGIN_NAME,
                MB_OK|MB_ICONINFORMATION
            );
            break;
        }
        case KPM_DIRECT_EXEC: {
            if (lParamW!=(LPARAM)g_menuItems[0].dwCommandID)
                break;

            if (!KP_Query(KPQ_FILEOPEN,0)) {
                MessageBox(
                    g_kpAppInfo.hwndMain,
                    _T("You need to open a database to import to first."),
                    PLUGIN_NAME,
                    MB_OK|MB_ICONWARNING
                );
                break;
            }

            LPTSTR path=new TCHAR[(MAX_PATH+1)*2];

            KP_Call(KPC_DISPLAYDIALOG,TRUE,0,0);

            CWnd window;
            window.Attach(g_kpAppInfo.hwndMain);
            if (WU_GetFileNameSz(TRUE,_T("oub"),path,MAX_PATH*2))
                ImportOublietteFile(path,window);
            window.Detach();

            KP_Call(KPC_DISPLAYDIALOG,FALSE,0,0);

            delete [] path;

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

    CT2CA password(dialog.GetPassword());
    const OublietteFile::CipherTextHeader *header=file.decryptData(password.m_psz);
    if (!header) {
        MessageBox(w.m_hWnd,file.getLastErrorMessage().c_str(),PLUGIN_NAME,MB_OK|MB_ICONWARNING);
        return;
    }

    CPwManager *pMgr=(CPwManager*)g_kpAppInfo.pPwMgr;

    // Create a top-level group to import to.
    PW_GROUP g;
    ZeroMemory(&g,sizeof(PW_GROUP));
    // DWORD uGroupId;
    g.uImageId=49; // Open folder icon.
    g.pszGroupName=_T(GROUP_NAME);
    g.tCreation=ConvertDateTime(header->getCreationTime());
    g.tLastMod=ConvertDateTime(header->getModificationTime());
    g.tLastAccess=ConvertDateTime(header->getModificationTime());
    pMgr->GetNeverExpireTime(&g.tExpire);
    // USHORT usLevel;
    pMgr->AddGroup(&g);

    // Try to find Oubliette's path in the registry to read category names.
    CString ini;

    TCHAR buffer[256];
    ULONG size=OI_ARRAY_LENGTH(buffer);
    ZeroMemory(buffer,sizeof(buffer));

    CRegKey r;
    if (r.Open(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Oubliette_is1",KEY_READ)==ERROR_SUCCESS) {
        size=OI_ARRAY_LENGTH(buffer);
        r.QueryStringValue("Inno Setup: App Path",buffer,&size);
        ini=buffer;
        ini+="\\Oubliette.ini";
    }

    g.usLevel=1;

    // Loop over all entries and add them to KeePass.
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

        CA2CT pszTitle(account.name.c_str());
        e.pszTitle=(TCHAR*)pszTitle.m_psz;

        CA2CT pszURL(account.url.c_str());
        e.pszURL=(TCHAR*)pszURL.m_psz;

        CA2CT pszUserName(account.username.c_str());
        e.pszUserName=(TCHAR*)pszUserName.m_psz;

        CA2CT pszPassword(account.password.c_str());
        e.pszPassword=(TCHAR*)pszPassword.m_psz;
        e.uPasswordLen=(DWORD)_tcslen(e.pszPassword);

        std::string additional
            ="Oubliette \"Email\":\n"+account.email
            +"\n\n"
            +"Oubliette \"Note\":\n"+account.note
            +"\n\n"
            +"Oubliette \"Memo\":\n"+account.memo;
        CA2CT pszAdditional(additional.c_str());
        e.pszAdditional=(TCHAR*)pszAdditional.m_psz;

        e.tCreation=ConvertDateTime(account.created);
        e.tLastMod=ConvertDateTime(account.created);
        e.tLastAccess=g.tLastAccess;
        if (account.expires.isValid())
            e.tExpire=ConvertDateTime(account.expires);
        else
            pMgr->GetNeverExpireTime(&e.tExpire);

        // TCHAR *pszBinaryDesc;
        // BYTE *pBinaryData;
        // DWORD uBinaryDataLen;

        pMgr->AddEntry(&e);
    }

    KP_Call(KPC_MODIFIED,TRUE,0,0);
    KP_Call(KPC_UPDATE_GROUPLIST,0,0,0);
    KP_Call(KPC_UPDATE_PASSWORDLIST,0,0,0);
    int nItems=(int)KP_Query(KPQ_PWLIST_ITEMCOUNT,0);
    KP_Call(KPC_PWLIST_ENSUREVISIBLE,nItems-1,FALSE,0);
    KP_Call(KPC_UPDATE_TOOLBAR,0,0,0);
}
