#!/bin/bash
cp /usr/local/Cellar/pvb/pvbrowser/pvbrowser.app/Contents/MacOS/pvbrowser          /usr/local/Cellar/pvb/pvbrowser/
cp /usr/local/Cellar/pvb/pvdevelop/pvdevelop.app/Contents/MacOS/pvdevelop          /usr/local/Cellar/pvb/pvdevelop/
cp /usr/local/Cellar/pvb/start_pvbapp/start_pvbapp.app/Contents/MacOS/start_pvbapp /usr/local/Cellar/pvb/start_pvbapp/
cp /usr/local/Cellar/pvb/rllib/rlsvg/rlsvgcat.app/Contents/MacOS/rlsvgcat          /usr/local/Cellar/pvb/rllib/rlsvg/
cp /usr/local/Cellar/pvb/rllib/rlfind/rlfind.app/Contents/MacOS/rlfind             /usr/local/Cellar/pvb/rllib/rlfind/
cp /usr/local/Cellar/pvb/rllib/rlhistory/rlhistory.app/Contents/MacOS/rlhistory    /usr/local/Cellar/pvb/rllib/rlhistory/
cp /usr/local/Cellar/pvb/rllib/rlhtml2pdf/rlhtml2pdf.app/Contents/MacOS/rlhtml2pdf /usr/local/Cellar/pvb/rllib/rlhtml2pdf/
cp /usr/local/Cellar/pvb/language_bindings/lua/pvslua/pvslua.app/Contents/MacOS/pvslua        /usr/local/Cellar/pvb/language_bindings/lua/pvslua/
cp /usr/local/Cellar/pvb/language_bindings/lua/pvapplua/pvapplua.app/Contents/MacOS/pvapplua  /usr/local/Cellar/pvb/language_bindings/lua/pvapplua/
cp /usr/local/Cellar/pvb/fake_qmake/fake_qmake.app/Contents/MacOS/fake_qmake       /usr/local/Cellar/pvb/fake_qmake/

echo "INFO: rm *.app currently not done. Please avtivate in copy_apps.sh"
#rm -rf /usr/local/Cellar/pvb/pvbrowser/pvbrowser.app          
#rm -rf /usr/local/Cellar/pvb/pvdevelop/pvdevelop.app          
#rm -rf /usr/local/Cellar/pvb/start_pvbapp/start_pvbapp.app 
#rm -rf /usr/local/Cellar/pvb/rllib/rlsvg/rlsvgcat.app          
#rm -rf /usr/local/Cellar/pvb/rllib/rlfind/rlfind.app             
#rm -rf /usr/local/Cellar/pvb/rllib/rlhistory/rlhistory.app                                 
#rm -rf /usr/local/Cellar/pvb/rllib/rlhtml2pdf/rlhtml2pdf.app                               
#rm -rf /usr/local/Cellar/pvb/language_bindings/lua/pvslua/pvslua.app                              
#rm -rf /usr/local/Cellar/pvb/language_bindings/lua/pvapplua/pvsapplua.app                         
#rm -rf /usr/local/Cellar/pvb/fake_qmake/fake_qmake.app

echo "Now please run:"
echo "sudo /usr/local/Cellar/pvb/spec/mac/set-links.sh"

