/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#include "stdafx.h"
#include "SmartCOMPtr.h"
#include "COMException.h"

namespace PluginFramework {

// Helper function:
template<typename Type> inline Type* queryInst(const std_string type, const PluginAPI& pluginAPI, REFCLSID rclsid, REFIID riid) {
    Type* result = 0;
    const HRESULT hresult = (*pluginAPI)->QueryInstance(rclsid, riid, (void**)&result);
    const bool error = hresult!=S_OK;
    if(error || result==0) throw Exception(type, error?hresult:E_UNEXPECTED, riid, rclsid);
    return result; 
}

// KeePass API:
template<> IKpAPI* Interface<IKpAPI>::queryInterface(IKpUnknown* const pAPI) const {
    if(pAPI==0) throw Exception(_T("API"), RPC_X_NULL_REF_POINTER, zero, zero);
    IKpAPI* result = 0;
    const HRESULT hresult = pAPI->QueryInterface(IID_IKpAPI, (void**)&result);
    const bool error = hresult!=S_OK;
    if(error || result==0) throw Exception(_T("API"), error?hresult:E_UNEXPECTED, IID_IKpAPI, zero);
    return result;    
}

// CommandLine:
template<> IKpCommandLine* Interface<IKpCommandLine>::queryInstance(const Interface<IKpAPI>& pluginAPI) const 
{return reinterpret_cast<IKpCommandLine*>(queryInst<IKpCommandLine>(_T("CommandLine"), pluginAPI, SCLSID_KpCommandLine, IID_IKpCommandLine));}


// CommandLineOption:
template<> IKpCommandLineOption* Interface<IKpCommandLineOption>::createInstance(const Interface<IKpAPI>& pluginAPI, const std_string optionName) const {
    IKpCommandLineOption* result = 0;
    const HRESULT hresult = (*pluginAPI)->CreateInstanceEx(CLSID_KpCommandLineOption, IID_IKpCommandLineOption, (void**)&result, 0, reinterpret_cast<LPARAM>(optionName.c_str()));
    const bool error = hresult!=S_OK;
    if(error || result==0) throw Exception(_T("CommandLineOption"), error?hresult:E_UNEXPECTED, IID_IKpCommandLineOption, CLSID_KpCommandLineOption);
    return result;
}

// Config:
template<> IKpConfig* Interface<IKpConfig>::createInstance(const Interface<IKpAPI>& pluginAPI, const bool readOnly) const {
    IKpConfig* result = 0;
    const HRESULT hresult = (*pluginAPI)->CreateInstance(readOnly?CLSID_KpConfig_ReadOnly:CLSID_KpConfig, IID_IKpConfig, (void**)&result);
    const bool error = hresult!=S_OK;
    if(error || result==0) throw Exception(_T("Config"), error?hresult:E_UNEXPECTED, IID_IKpConfig, CLSID_KpConfig);
    return result;
}

// Database:
template<> IKpDatabase* Interface<IKpDatabase>::queryInstance(const Interface<IKpAPI>& pluginAPI) const 
{return reinterpret_cast<IKpDatabase*>(queryInst<IKpDatabase>(_T("Database"), pluginAPI, SCLSID_KpDatabase, IID_IKpDatabase));}

// FullPathName:
template<> IKpFullPathName* Interface<IKpFullPathName>::createInstance(const Interface<IKpAPI>& pluginAPI, const std_string path) const {
    IKpFullPathName* result = 0;
    const HRESULT hresult = (*pluginAPI)->CreateInstanceEx(CLSID_KpFullPathName, IID_IKpFullPathName, (void**)&result, 0, reinterpret_cast<LPARAM>(path.c_str()));
    const bool error = hresult!=S_OK;
    if(error || result==0) throw Exception(_T("FullPathName"), error?hresult:E_UNEXPECTED, IID_IKpFullPathName, CLSID_KpFullPathName);
    return result;
}

// Utilities:
template<> IKpUtilities* Interface<IKpUtilities>::queryInstance(const Interface<IKpAPI>& pluginAPI) const 
{return reinterpret_cast<IKpUtilities*>(queryInst<IKpUtilities>(_T("Utilities"), pluginAPI, SCLSID_KpUtilities, IID_IKpUtilities));}

}