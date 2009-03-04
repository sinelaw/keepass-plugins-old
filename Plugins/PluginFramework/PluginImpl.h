/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "UnknownImpl.h"

namespace PluginFramework {

class PluginImpl : public UnknownImpl {
public:
    explicit PluginImpl(IKpUnknown* const pAPI) : m_PluginAPI(pAPI) {}

    // Default implementations for IKpPlugin interface:
    // Override as needed.
	STDMETHODIMP_(LPCTSTR) GetProperty(LPCTSTR lpName);
	STDMETHODIMP SetProperty(LPCTSTR lpName, LPCTSTR lpValue);

	STDMETHODIMP_(DWORD) GetMenuItemCount();
	STDMETHODIMP_(KP_MENU_ITEM*) GetMenuItems();

    // No reasonable default implementation.  Define in derived class.
    //STDMETHODIMP_(BOOL) OnMessage(DWORD dwCode, LPARAM lParamW, LPARAM lParamL);

protected:
    PluginAPI m_PluginAPI;
};

}