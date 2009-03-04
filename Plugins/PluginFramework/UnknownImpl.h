/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "SmartCOMPtr.h"

namespace PluginFramework {

class UnknownImpl : public IKpPlugin {
public:
    UnknownImpl() : m_ReferenceCount(1) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject); 
	STDMETHODIMP_(ULONG) AddRef(); 
    STDMETHODIMP_(ULONG) Release();

protected:
    virtual ~UnknownImpl(){}

private:
    template<typename Interface> bool supportInterface(REFIID supported_iid, REFIID riid, void** ppvObject);

    unsigned long m_ReferenceCount;    
};

}