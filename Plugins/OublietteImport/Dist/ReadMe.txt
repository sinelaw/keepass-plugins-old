NOTE: You have to rename the KeePass executables to "KeePass.exe" before
testing. The plug-in DLL names can stay as they are. If you wonder why the
"KeePass.exe" from the original distribution is larger than "KeePass-VC60.exe",
this is only because it has been compressed by UPX (http://upx.sf.net/).

+---------------------+------------------------+---------------------+------------------------+--------------------------------+
| KeePass-VC60.exe    | KeePass-VC60-Debug.exe | KeePass-VC71.exe    | KeePass-VC71-Debug.exe |                                |
+---------------------+------------------------+---------------------+------------------------+--------------------------------+
| Loads but crashes   | Prevents KeePass       | Loads only in Extra | Prevents KeePass       | OublietteImport-VC60.dll       |
| password dialog     | startup, failed assert | menu, disabled      | startup, failed assert |                                |
+---------------------+------------------------+---------------------+------------------------+--------------------------------+
| Loads only in Extra | Loads but crashes      | Loads only in Extra | Prevents KeePass       | OublietteImport-VC60-Debug.dll |
| menu, disabled      | password dialog        | menu, disabled      | startup, failed assert |                                |
+---------------------+------------------------+---------------------+------------------------+--------------------------------+
| Loads only in Extra | Prevents KeePass       | Works perfectly *)  | Prevents KeePass       | OublietteImport-VC71.dll       |
| menu, enabled *)    | startup, failed assert |                     | startup, failed assert |                                |
+---------------------+------------------------+---------------------+------------------------+--------------------------------+
| Loads only in Extra | Prevents KeePass       | Loads only in Extra | Works perfectly *)     | OublietteImport-VC71-Debug.dll |
| menu, disabled      | startup, failed assert | menu, disabled      |                        |                                |
+---------------------+------------------------+---------------------+------------------------+--------------------------------+

*) Only these combination actually work. Well, at least sort of :-)
