This is a pvbrowser plugin for firefox ...

### Installation ###

Linux:   ################################################################
cp libpvbrowser.so /usr/lib/browser-plugins/            # 32 bit openSUSE
cp libpvbrowser.so /usr/lib64/browser-plugins/          # 64 bit openSUSE

Comment: the plugin has good performance on linux

Windows: ################################################################
copy %pvbdir%\win-mingw\bin\*.dll "%PROGRAMS%\Mozilla Firefox\plugins\"

Comment: the plugin works very slow on windows. 
It seems that the QThread monitoring the network gets littel CPU time.
When constantly moving the mouse over the widget it becomes faster.

OS X:    ################################################################

### Example ###

firefox pvbrowserplugin-example.html

