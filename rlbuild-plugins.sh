#!/bin/bash
# 14feb2023 source rlsetup.sh
echo "copy qwt designer plugin to destination directory"
cp qwt/designer/plugins/designer/libqwt_designer_plugin.* designer/plugins/
# 14feb2023 cd pvbrowser
# 14feb2023 ../qmake.sh pvbrowser.pro
# 14feb2023 make $1
# 14feb2023 cd ..
# 24 Apr 2015  r.l. removed browser plugin from building
#cd browserplugin
#../qmake.sh pvpluginmain.pro
#make $1
#cd ..
echo "################################################################"
echo "# finished compiling rlbuild-plugins.sh !!!                    #"
echo "################################################################"
