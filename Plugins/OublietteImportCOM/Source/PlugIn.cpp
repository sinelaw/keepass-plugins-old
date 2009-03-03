/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plugin
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2009 Sebastian Schuberth <sschuberth@gmail.com>
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
#include "PasswordDialog.h"

#include "Oubliette/OublietteFile.h"

#include "../../PluginFramework/CreatePlugin.h"

#define PLUGIN_NAME _T("Oubliette Import Plugin")

#define PLUGIN_ICON 49+1 // Opened folder
#define PLUGIN_MENU _T("&Oubliette OUB File...")

/*
 * Mandatory symbols
 */

extern std_string const g_PluginName(PLUGIN_NAME);

KP_EXPORT HRESULT KP_API KP_I_CREATEINSTANCE_DECL(REFIID riid,void** ppvObject,IKpUnknown* pAPI)
{
    return PluginFramework::createPlugin<COublietteImportPlugin>(riid,ppvObject,pAPI);
}

/*
 * Menu items (under "Tools")
 */

static KP_MENU_ITEM s_MenuItems[]={
//    dwFlags             dwState   dwIcon        lpCommandString   dwCommandID   dwReserved
    { KPMIF_POPUP_START , 0       , 0           , PLUGIN_NAME     , 0           , 0          }
,   { 0                 , 0       , PLUGIN_ICON , PLUGIN_MENU     , 0           , 0          }
,   { KPMIF_POPUP_END   , 0       , 0           , PLUGIN_NAME     , 0           , 0          }
};

STDMETHODIMP_(DWORD) COublietteImportPlugin::GetMenuItemCount()
{
    return _countof(s_MenuItems);
}

STDMETHODIMP_(KP_MENU_ITEM*) COublietteImportPlugin::GetMenuItems()
{
    return s_MenuItems;
}

/*
 * Message handling
 */

STDMETHODIMP_(BOOL) COublietteImportPlugin::OnMessage(DWORD dwCode,LPARAM lParamW,LPARAM lParamL)
{
    switch (dwCode) {
        case KPM_DIRECT_CONFIG: {
            MessageBox(
                m_PluginAPI->GetMainWindowHandle()
            ,   _T("This plugin does not have any options to configure.")
            ,   PLUGIN_NAME
            ,   MB_OK|MB_ICONINFORMATION
            );

            break;
        }

        case KPM_PLUGIN_INFO: {
            static TCHAR buffer[512]={'\0'};

            _stprintf_s(
                buffer
            ,   _countof(buffer)-1
            ,   "This is a plugin to import Oubliette OUB files (see http://oubliette.sf.net/),\n"
                "it requires KeePass 1.x with x >= 15.\n"
                "Written by Sebastian Schuberth <sschuberth@gmail.com>.\n\n"
                "Usage note: You need to first create or open a KeePass database file\n"
                "to import to before you can choose to import from an Oubliette file."
            );

            MessageBox(
                m_PluginAPI->GetMainWindowHandle()
            ,   buffer
            ,   PLUGIN_NAME
            ,   MB_OK|MB_ICONINFORMATION
            );

            break;
        }

        case KPM_INIT_MENU_POPUP: {
            // Disable the "Tools" menu items if no file is loaded to import to.
            if (m_PluginAPI->IsFileOpen()) {
                for (size_t i=0;i<_countof(s_MenuItems);++i) {
                    s_MenuItems[i].dwFlags&=~KPMIF_DISABLED;
                }
            }
            else {
                for (size_t i=0;i<_countof(s_MenuItems);++i) {
                    s_MenuItems[i].dwFlags|=KPMIF_DISABLED;
                }
            }

            break;
        }

        case KPM_WND_INIT_POST: {
            // As KeePass assigns the dwCommandID, we cannot insert the "File"
            // -> "Import From" menu item any earlier than this.
            m_PluginAPI->AddImportMenuItem(s_MenuItems[1].lpCommandString,s_MenuItems[1].dwCommandID,s_MenuItems[1].dwIcon);

            break;
        }

        case KPM_DIRECT_EXEC: {
            // Check if is is our menu item that is executed.
            if (lParamW!=static_cast<LPARAM>(s_MenuItems[1].dwCommandID)) {
                break;
            }

            // This should not happen as the menu items should disabled if no
            // file is open.
            if (!m_PluginAPI->IsFileOpen()) {
                MessageBox(
                    m_PluginAPI->GetMainWindowHandle()
                ,   _T("You need to first create or open a KeePass database file to import to.")
                ,   PLUGIN_NAME
                ,   MB_OK|MB_ICONWARNING
                );

                break;
            }

            // Open a file dialog and import the chosen file.
            BOOL modal=m_PluginAPI->IsDisplayingDialog();
            m_PluginAPI->SetDisplayDialog(TRUE);

            static TCHAR buffer[MAX_PATH]={'\0'};
            if (m_Utilities->ShowFileDialog(TRUE,_T("oub"),buffer,_countof(buffer)-1)==S_OK) {
                ImportOublietteFile(buffer);
            }

            m_PluginAPI->SetDisplayDialog(modal);

            break;
        }
    }

    return TRUE;
}

PW_TIME COublietteImportPlugin::ConvertDateTime(DateTime const& dt)
{
    PW_TIME t;

    t.shYear=static_cast<USHORT>(dt.getYear());
    t.btMonth=static_cast<BYTE>(dt.getMonth());
    t.btDay=static_cast<BYTE>(dt.getDay());
    t.btHour=static_cast<BYTE>(dt.getHour());
    t.btMinute=static_cast<BYTE>(dt.getMinute());
    t.btSecond=static_cast<BYTE>(dt.getSecond());

    return t;
}

bool COublietteImportPlugin::ImportOublietteFile(LPCTSTR name)
{
    OublietteFile file(name);
    if (!file) {
        MessageBox(
            m_PluginAPI->GetMainWindowHandle()
        ,   file.getLastErrorMessage().c_str()
        ,   PLUGIN_NAME
        ,   MB_OK|MB_ICONWARNING
        );
        return false;
    }

    // TODO: Maybe use m_PluginAPI->ShowDialog() here in the future, when it is
    // implemented.

    CWnd parent;
    parent.Attach(m_PluginAPI->GetMainWindowHandle());

    CPasswordDialog dialog(&parent);
    if (dialog.DoModal()!=IDOK) {
        return false;
    }

    return true;

#if 0
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
        g.uImageId=account.category;

        // Add a "Subgroup" for every used Oubliette category.
        CString category=file.getCategoryName(account.category).c_str();
        if (category.IsEmpty()) {
            category.Format(_T("Category%i"),account.category+1);
            GetPrivateProfileString("Categories",category,category,buffer,size,ini);
            g.pszGroupName=buffer;
        }
        g.pszGroupName=category.GetBuffer();

        // Create each category only once; empty categories are not created.
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

        std::string additional;

        if (!account.email.empty())
            additional+="Email:\n"+account.email;

        if (!account.note.empty()) {
            if (!additional.empty())
                additional+="\n\n";
            additional+="Note:\n"+account.note;
        }

        if (!account.memo.empty()) {
            if (!additional.empty())
                additional+="\n\n";
            additional+="Memo:\n"+account.memo;
        }

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
#endif
}
