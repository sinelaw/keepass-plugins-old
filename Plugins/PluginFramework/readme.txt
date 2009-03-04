                 Plugin Framework for KeePass
                 ============================

Copyright 2009 Bill Rubin <rubin@contractor.net>

Distributed under the Boost Software License, Version 1.0.
 
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) 


Description:
============          

Plugin Framework for KeePass (PFK) is a Visual Studio 2005 C++ project 
useful for developing plugins for KeePass Version 1.15 or later.  
With PFK, the KeePass plugin developer need not write or maintain 
large amounts of boilerplate code to interface the
plugin with Keepass, or to perform common plugin operations.  
With PFK, the plugin developer can focus on the application code 
specific to the plugin.  PFK features include:

1. Complete implementation of the IID_IKpUnknown interface.
2. Implementation of the IID_IKpPlugin interface, except for the 
   OnMessage member function, which is always application-specific.
3. Ability to get smart pointers to any other KeePass interface, 
   including IKpAPI, IKPCommandLine, IKpCommandLineOption, IKpConfig, 
   IKpDatabase, IKpFullPathName, and IKpUtilities.
4. Implementation of the KpCreateInstance function, which the plugin
   must export from its DLL.
5. Comprehensive error checking of COM handshaking for Items 3 and 4.
   If an error is found, PFK displays a message box containing all
   information about the error.  Without this feature, a plugin will,
   in most cases, silently fail to load.  In other cases, it will
   fail to perform its function.
6. Convenience utilities to display a message box, translate a
   Windows error code into a natural language string, and declare a
   standard string independently of character type.
7. Compile-time checking of constructor invocations for smart COM
   pointers.  Without this feature, it is difficult for a developer
   to interpret compiler error messages caused by using the wrong
   smart COM pointer constructor.
8. The PFK code avoids defining macros.  Instead templates, inline
   functions, typedefs, and other C++ constructs maintain safe 
   design practices, with no runtime penalty.


Installation Instructions:
==========================

1. Download the PFK zip file from the KeePass plugin development page.
2. Unzip the downloaded file into a PluginFramework directory 
   under the KeePass Plugins directory.
3. Add the PluginFramework Visual Studio project to the KeePass solution.
4. Build the PluginFramework project.  The output is a static library.
5. To make PFK accessible to your plugin project, add PluginFramework 
   as a reference in the common properties of your plugin project.  

To integrate PFK into your existing or new plugin, it is highly 
recommended to look at the source code of TestPlugin (from the 
KeePass plugin development page), Another Backup Plugin, or other 
plugins which already use PFK.



                            Release Notes
                            =============


Version 1.0 for KeePass 1.15 or later  31Jan2009
------------------------------------------------

     Initial release.


Known Issues
------------

     None.