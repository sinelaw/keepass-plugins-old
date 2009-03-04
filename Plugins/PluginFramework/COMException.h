/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "std_string.h"

namespace PluginFramework {

#pragma warning (push)
#pragma warning( disable : 4512 )  // assignment operator could not be generated

class Exception {
public:
    Exception(const std_string type, const HRESULT hresult, REFIID riid, REFCLSID rclsid) 
        : m_type(type), m_hresult(hresult), m_riid(riid), m_rclsid(rclsid) {}
    const std_string getType() const {return m_type;}
    HRESULT getHRESULT() const {return m_hresult;}
    REFIID  getIID() const {return m_riid;}
    REFCLSID getCLSID() const {return m_rclsid;}

private:
    std_string m_type;
    HRESULT    m_hresult;
    REFIID     m_riid;
    REFCLSID   m_rclsid;
};

#pragma warning (pop)

// Helper function displays a MessageBox containing the exception:
void displayMessage(const Exception& exception, const std_string effectOfError);

// Helper data:
#pragma warning (push)
#pragma warning( disable : 4268 ) // 'const' static/global data initialized with compiler generated default constructor fills the object with zeros
const GUID zero;
#pragma warning (pop)

}