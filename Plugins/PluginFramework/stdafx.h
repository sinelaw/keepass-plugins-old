/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN          // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400         // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT          // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400   // Change this to the appropriate value to target Windows 2000 or later.
#endif

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#include <windows.h>
#include <tchar.h>