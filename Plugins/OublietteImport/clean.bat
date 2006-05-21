@echo off
rmdir /s /q Binary Debug Release
attrib -h *.suo
del *.aps *.ncb *.opt *.plg *.suo *.user
