/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#include "stdafx.h"
#include "WinUtil.h"
#include <cassert>

extern const std_string g_PluginName;  // Define this global variable in another source file, using extern keyword.

#pragma warning (push)
#ifdef NDEBUG
#pragma warning( disable : 4189 )  // local variable is initialized but not referenced
#endif

namespace PluginFramework {

std_string formatLastSystemError(const DWORD errorCode) {
    LPTSTR lpMsgBuf = 0;

    const BOOL succeeded = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,  // FormatMessage() parameter type specification seems wrong!
        0,
        0);

    assert(succeeded);
    const std_string result = lpMsgBuf;
    LocalFree(lpMsgBuf);
    return result;
}

void displayMessage(const std_string& message, HWND hWnd, const UINT type) {    
    const int button = MessageBox(hWnd, message.c_str(), g_PluginName.c_str(), type);    
    assert(button==IDOK);
}

}

#pragma warning (pop)