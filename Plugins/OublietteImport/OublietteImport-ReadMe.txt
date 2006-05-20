This is an Oubliette (http://oubliette.sf.net/) import plug-in for KeePass
(http://keepass.sf.net/). The plug-in currently only works when compiled with
VC71 (although it compiles without errors in VC60). However, using a VC71
plug-in with KeePass (which is, by default, compiled with VC60) causes some
minor issues regarding the GUI integration:

- The plug-in is only visible under "Extras", not in the "File" -> "Import From"
  menu.

- The menu entry is always enabled, even if there is not database open to import
  to.

- The database to import to is not marked as modified after importing, so
  remember to save it :-)

I hope you find this plug-in still useful. Take care.

Sebastian Schuberth <sschuberth@gmail.com>
2005.nov.23