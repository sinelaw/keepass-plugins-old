/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#include "stdafx.h"
#include "UnknownImpl.h"
#include <assert.h>

namespace PluginFramework {

STDMETHODIMP_(ULONG) UnknownImpl::AddRef() {return ++m_ReferenceCount;} 

STDMETHODIMP_(ULONG) UnknownImpl::Release() {
    switch(m_ReferenceCount) {
        case 0:  assert(false); return 0;
        case 1:  delete this;   return 0;
        default: return --m_ReferenceCount;
        }; 
}

template<typename Interface> inline bool UnknownImpl::supportInterface(REFIID supported_iid, REFIID riid, void** ppvObject) { 
	if(riid!=supported_iid) return false;
    *ppvObject = static_cast<Interface*>(this);
    AddRef(); 
    return true; 
}

STDMETHODIMP UnknownImpl::QueryInterface(REFIID riid, void** ppvObject) {
    assert(m_ReferenceCount);
    if(supportInterface<IKpUnknown>(IID_IKpUnknown, riid, ppvObject)) return S_OK;
    if(supportInterface<IKpPlugin> (IID_IKpPlugin,  riid, ppvObject)) return S_OK;
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

}