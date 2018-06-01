#!/bin/bash
ln -sf /usr/local/Cellar/pvb/pvbrowser/pvbrowser                     /usr/local/bin/pvbrowser
ln -sf /usr/local/Cellar/pvb/pvdevelop/pvdevelop                     /usr/local/bin/pvdevelop
ln -sf /usr/local/Cellar/pvb/start_pvbapp/start_pvbapp               /usr/local/bin/start_pvbapp
ln -sf /usr/local/Cellar/pvb/rllib/rlsvg/rlsvgcat                    /usr/local/bin/rlsvgcat
ln -sf /usr/local/Cellar/pvb/rllib/rlfind/rlfind                     /usr/local/bin/rlfind
ln -sf /usr/local/Cellar/pvb/rllib/rlhistory/rlhistrory              /usr/local/bin/rlhistory
ln -sf /usr/local/Cellar/pvb/rllib/rlhtml2pdf/rlhtml2pdf             /usr/local/bin/rlhtml2pdf
ln -sf /usr/local/Cellar/pvb/language_bindings/lua/pvslua/pvslua     /usr/local/bin/pvslua
ln -sf /usr/local/Cellar/pvb/language_bindings/lua/pvapplua/pvapplua /usr/local/bin/pvapplua
ln -sf /usr/local/Cellar/pvb/fake_qmake/fake_qmake                   /usr/local/bin/fake_qmake
export  real_qmake=$(find /usr/local/Cellar/qt -name qmake)
ln -sf $real_qmake                                                   /usr/local/bin/qmake
#ln -sf /usr/local/Cellar/qt/5.10.0/bin/qmake                        /usr/local/bin/qmake

ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.dylib
ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.dylib
ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.0.dylib
ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.0.0.dylib

ln -sf /usr/local/Cellar/pvb/pvserver/libpvsid.a  /usr/local/lib/libpvsid.a
ln -sf /usr/local/Cellar/pvb/pvserver/libpvsmt.a  /usr/local/lib/libpvsmt.a

ln -sf /usr/local/Cellar/pvb /opt

echo "INFO:"
echo "If setting the above links failed, this might be a problem with file permissions on your system."
echo "This problem can be fixed with:"
echo "sudo /usr/local/Cellar/pvb/spec/mac/set-links.sh"

if [ -f /usr/local/bin/pvbrowser ]; then
echo "Finished, please run:"
echo "pvbrowser pv://pvbrowser.org"
fi
