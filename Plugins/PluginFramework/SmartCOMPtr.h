/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once
#include "../../KeePassLibCpp/SDK/KpSDK.h"
#include "std_string.h"
#include "Metaprograms.h"

namespace PluginFramework {

// Smart COM pointer to KeePass interface
// Non-default constructors throw if interface not found.

template<typename Type> class Interface {

    // Check for allowed interfaces:
    template<typename T1, typename T2 = void, typename T3 = void> class AllowedInterfaces {
        static const bool allowed = Metaprogram::is_same<Type, T1>::value
                                 || Metaprogram::is_same<Type, T2>::value
                                 || Metaprogram::is_same<Type, T3>::value;
        enum {junk = sizeof Metaprogram::ConditionalError<!allowed, Incorrect_constructor_for_this_interface>};
        };

public:
    Interface() : m_pInterface(0) {}

    // Use correct non-default constructor for each interface.  See AllowedInterfaces<> for each constructor.
    // If you use the wrong constructor for an interface, the following compile error occurs:
    class Incorrect_constructor_for_this_interface;

    explicit Interface(IKpUnknown* const pAPI) : m_pInterface(queryInterface(pAPI)) 
        {sizeof AllowedInterfaces<IKpAPI>;}

    explicit Interface(const Interface<IKpAPI>& pluginAPI) : m_pInterface(queryInstance(pluginAPI)) 
        {sizeof AllowedInterfaces<IKpCommandLine, IKpDatabase, IKpUtilities>;}

    Interface(const Interface<IKpAPI>& pluginAPI, const std_string path) : m_pInterface(createInstance(pluginAPI, path))
        {sizeof AllowedInterfaces<IKpCommandLineOption, IKpFullPathName>;}

    Interface(const Interface<IKpAPI>& pluginAPI, const bool readOnly) : m_pInterface(createInstance(pluginAPI, readOnly))
        {sizeof AllowedInterfaces<IKpConfig>;}

    ~Interface() {if(m_pInterface) m_pInterface->Release();}
    Type* operator->() const {return m_pInterface;}
    Type* operator*() const {return m_pInterface;}
    void swap(Interface<Type>& rhs) {std::swap(m_pInterface, rhs.m_pInterface);}

private:
    Type* queryInterface(IKpUnknown* pAPI) const;
    Type* queryInstance(const Interface<IKpAPI>&) const;
    Type* createInstance(const Interface<IKpAPI>&, const std_string) const;
    Type* createInstance(const Interface<IKpAPI>&, bool) const;

    Type* m_pInterface;
};

//                                      Interface types:
//                                      ----------------
typedef Interface<IKpAPI>               PluginAPI;
typedef Interface<IKpCommandLine>       CommandLine;
typedef Interface<IKpCommandLineOption> CommandLineOption;
typedef Interface<IKpConfig>            Config;
typedef Interface<IKpDatabase>          Database;
typedef Interface<IKpFullPathName>      FullPathName;
typedef Interface<IKpUtilities>         Utilities;

}