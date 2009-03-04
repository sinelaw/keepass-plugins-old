/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#include "stdafx.h"
#include "PluginImpl.h"

namespace PluginFramework {

STDMETHODIMP_(LPCTSTR) PluginImpl::GetProperty(LPCTSTR lpName)
{
	UNREFERENCED_PARAMETER(lpName);
	return NULL;
}

STDMETHODIMP PluginImpl::SetProperty(LPCTSTR lpName, LPCTSTR lpValue)
{
	UNREFERENCED_PARAMETER(lpName);
	UNREFERENCED_PARAMETER(lpValue);
	return E_FAIL;
}

STDMETHODIMP_(DWORD) PluginImpl::GetMenuItemCount()
{
	return 0;
}

STDMETHODIMP_(KP_MENU_ITEM*) PluginImpl::GetMenuItems()
{
	return NULL;
}

}