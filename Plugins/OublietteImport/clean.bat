@echo off
rmdir /s /q Binary Debug Release
attrib -h *.suo
del *.ncb *.opt *.plg *.suo *.aps