/*
  Copyright (c) 2008-2009, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "TestPlugin.h"
#include "../../Tools/PluginFramework/CreatePlugin.h"
#include "../../Tools/PluginFramework/WinUtil.h"

// Optional definitions:  Define them only if you need them for something.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	UNREFERENCED_PARAMETER(hinstDLL);
	UNREFERENCED_PARAMETER(fdwReason);
	UNREFERENCED_PARAMETER(lpvReserved);

	return TRUE;
}

KP_EXPORT HRESULT KP_API KP_I_INITIALIZELIB_DECL(IKpUnknown* pAPI)
{
	UNREFERENCED_PARAMETER(pAPI);
	return S_OK;
}

KP_EXPORT HRESULT KP_API KP_I_RELEASELIB_DECL(IKpUnknown* pAPI)
{
	UNREFERENCED_PARAMETER(pAPI);
	return S_OK;
}

// Required definitions:

extern const std_string g_PluginName(_T("Test Plugin"));  // Referenced in WinUtil.cpp

KP_EXPORT HRESULT KP_API KP_I_CREATEINSTANCE_DECL(REFIID riid, void** ppvObject, IKpUnknown* pAPI)
{return PluginFramework::createPlugin<CTestPluginImpl>(riid, ppvObject, pAPI);}

CTestPluginImpl::CTestPluginImpl(IKpUnknown* pAPI) 
  : PluginFramework::PluginImpl(pAPI),

    // Optional interfaces:  (include only the ones you need)
    m_CommandLine      (m_PluginAPI),
    m_CommandLineOption(m_PluginAPI, std_string(_T("test."))),
    m_Config           (m_PluginAPI, true),
    m_Database         (m_PluginAPI),
    m_FullPathName     (m_PluginAPI, std_string(_T("interesting path name here"))),
    m_Utilities        (m_PluginAPI)
{}

STDMETHODIMP_(BOOL) CTestPluginImpl::OnMessage(DWORD dwCode, LPARAM lParamW,
	LPARAM lParamL)
{
	UNREFERENCED_PARAMETER(lParamL);

	switch(dwCode)
	{
	case KPM_DIRECT_CONFIG:
        PluginFramework::displayMessage(_T("This plugin doesn't have any options."), 
            reinterpret_cast<HWND>(lParamW), MB_OK | MB_ICONINFORMATION);
		break;

	case KPM_PLUGIN_INFO:
        PluginFramework::displayMessage(_T("Just a sample plugin to demonstrate how a basic KeePass plugin looks."),
            reinterpret_cast<HWND>(lParamW), MB_OK | MB_ICONINFORMATION);
		break;

	default:
		break;
	}

	return TRUE;
}