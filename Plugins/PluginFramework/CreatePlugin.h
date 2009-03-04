/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "../../KeePassLibCpp/SDK/KpSDK.h"
#include "COMException.h"
#include <tchar.h>

namespace PluginFramework {

// Create instance of KeePass plugin:

template<class PluginType> HRESULT createPlugin(REFIID riid, void** ppvObject, IKpUnknown* pAPI) {
	*ppvObject = NULL;
    const bool iidIsPlugin = (riid==IID_IKpPlugin)!=0;  // Goofy operator== returns int instead of bool!

    try {
        if(!iidIsPlugin && riid!=IID_IKpUnknown) throw Exception(_T("API"), E_NOINTERFACE, riid, zero);
        IKpPlugin* const plugin = new PluginType(pAPI);  // Throws if failure
        *ppvObject = iidIsPlugin?plugin:static_cast<IKpUnknown*>(plugin);
	    return S_OK;
        }

    catch(const Exception& exception) {
        displayMessage(exception, _T("\nPlugin will not be loaded."));
        return exception.getHRESULT();
        }
}

}