/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#include "stdafx.h"
#include "COMException.h"
#include "WinUtil.h"
#include <sstream>
#include <iomanip>

namespace PluginFramework {

namespace {

bool isNonzero(REFGUID guid) {
    if(guid.Data1!=0 || guid.Data2!=0 || guid.Data3!=0) return true;
    for(int i = 0; i<8; ++i) if(guid.Data4[i]!=0) return true;
    return false;
}

std::ostream& operator<<(std::ostream& os, REFGUID guid) {
    using namespace std;
    const ios_base::fmtflags oldFlags = os.flags();
    os.setf(ios_base::hex, ios_base::basefield);
    os.setf(ios_base::uppercase);
    os.fill(_T('0'));
    os<<setw(8)<<guid.Data1<<_T("-")<<setw(4)<<guid.Data2<<_T("-")<<setw(4)<<guid.Data3<<_T("-")
      <<setw(2)<<static_cast<unsigned long>(guid.Data4[0])<<setw(2)<<static_cast<unsigned long>(guid.Data4[1])<<_T("-");
    for(int i = 2; i<8; ++i) os<<setw(2)<<static_cast<unsigned long>(guid.Data4[i]);
    os.flags(oldFlags);  // Restore old flags
    return os;
}

}

void displayMessage(const Exception& exception, const std_string effectOfError) {
    std::basic_ostringstream<TCHAR> os;
    os<<_T("Plugin failed to find KeePass ")<<exception.getType()<<_T(" interface.")
        _T("\nReason:  ")<<formatLastSystemError(exception.getHRESULT())
      #pragma warning (push)
      #pragma warning( disable : 4312 )
      <<_T("Error code:  ")<<reinterpret_cast<void*>(exception.getHRESULT());  // Display in hex
      #pragma warning (pop)

    if(isNonzero(exception.getIID()))   os<<_T("\nInterface ID:  ")   <<exception.getIID();
    if(isNonzero(exception.getCLSID())) os<<_T("\nClass ID:         ")<<exception.getCLSID();  // Attempt to line up variable pitch font.

    os<<_T('\n')<<effectOfError;
    displayMessage(os.str(), 0, MB_ICONEXCLAMATION);
}

}