@echo off

pushd %~dp0

rmdir /s /q Binary Debug Release
attrib -h *.suo
del *.aps *.ncb *.opt *.plg *.suo *.user

cd Test

rmdir /s /q Binary Debug Release
attrib -h *.suo
del *.aps *.ncb *.opt *.plg *.suo *.user

popd
