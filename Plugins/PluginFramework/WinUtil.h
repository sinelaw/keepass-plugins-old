/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "std_string.h"

namespace PluginFramework {

std_string formatLastSystemError(const DWORD errorCode);

void displayMessage(const std_string&, HWND, UINT type);

}