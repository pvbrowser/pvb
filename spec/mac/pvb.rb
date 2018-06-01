# Installing pvbrowser via homebrew on MacOS:
#
# Documentation: https://docs.brew.sh/Formula-Cookbook.html
#                http://www.rubydoc.info/github/Homebrew/brew/master/Formula
# PLEASE REMOVE ALL GENERATED COMMENTS BEFORE SUBMITTING YOUR PULL REQUEST!
#
##################################
# Comment from R. Lehrig follows #
##################################
# (setup tasks)
#   "install brew"    See: https://docs.brew.sh/Installation.html
#   "install xcode"   xcode-select --install
#   brew install qt
#   brew cask install xquartz
#
# (build tasks)
#   rm ~/Library/Caches/homebrew/pvb*.tar.gz
#   brew reinstall pvb
#   brew edit pvb
#   export HOMEBREW_PREFIX="/usr/local"
#   brew install pvb --debug --verbose
# 
#   ( now the binaries are available under /usr/local/Cellar/pvb ) 
#
# (howto link the binaries into MacOS : "run" source set-links.sh )
#   source set-links.sh
#
# Here is what set-links.sh does:
# ln -sf /usr/local/Cellar/pvb/pvbrowser/pvbrowser                     /usr/local/bin/pvbrowser
# ln -sf /usr/local/Cellar/pvb/pvdevelop/pvdevelop                     /usr/local/bin/pvdevelop
# ln -sf /usr/local/Cellar/pvb/start_pvbapp/start_pvbapp               /usr/local/bin/start_pvbapp
# ln -sf /usr/local/Cellar/pvb/rllib/rlsvg/rlsvgcat                    /usr/local/bin/rlsvgcat
# ln -sf /usr/local/Cellar/pvb/rllib/rlfind/rlfind                     /usr/local/bin/rlfind
# ln -sf /usr/local/Cellar/pvb/rllib/rlhistory/rlhistrory              /usr/local/bin/rlhistory
# ln -sf /usr/local/Cellar/pvb/rllib/rlhtml2pdf/rlhtml2pdf             /usr/local/bin/rlhtml2pdf
# ln -sf /usr/local/Cellar/pvb/language_bindings/lua/pvslua/pvslua     /usr/local/bin/pvslua
# ln -sf /usr/local/Cellar/pvb/language_bindings/lua/pvapplua/pvapplua /usr/local/bin/pvapplua
# ln -sf /usr/local/Cellar/pvb/fake_qmake/fake_qmake                   /usr/local/bin/fake_qmake
# export  real_qmake=$(find /usr/local/Cellar/qt -name qmake)
# ln -sf $real_qmake                                                   /usr/local/bin/qmake
# #ln -sf /usr/local/Cellar/qt/5.10.0/bin/qmake                        /usr/local/bin/qmake
# 
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.0.dylib
# ln -sf /usr/local/Cellar/pvb/rllib/lib/librllib.dylib /usr/local/lib/librllib.1.0.0.dylib
# 
# ln -sf /usr/local/Cellar/pvb/pvserver/libpvsid.a      /usr/local/lib/libpvsid.a
# ln -sf /usr/local/Cellar/pvb/pvserver/libpvsmt.a      /usr/local/lib/libpvsmt.a
#
# ln -sf /usr/local/Cellar/pvb                          /opt
#

# The formula pvb.rb follows. (At least the sha256 must be adjusted)
# openssl dgst -sha256 pvb-5.1.3.tar.gz
class Pvb < Formula
  desc "Installs pvbrowser. After installation run source /usr/local/Cellar/pvb/spec/mac/set-links.sh"
  homepage "https://pvbrowser.org"
  url "https://pvbrowser.de/pvbrowser/tar/pvb-5.1.3.tar.gz"
  sha256 "69f20358c3d07bb3f58629b67609b7121ec226f9c40ef3193a698649d8181da8"

  depends_on "qt"
  depends_on :xcode
  depends_on :x11

  def install
    system "./build.sh", "all", "homebrew"
    system "./build.sh", "all", "homebrew"
    system "./install-to.sh", "/usr/local/Cellar"
    system "./spec/mac/copy-apps.sh"
    system "./spec/mac/set-links.sh"
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! For Homebrew/homebrew-core
    # this will need to be a test that verifies the functionality of the
    # software. Run the test with `brew test pvb`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system "#{bin}/program", "do", "something"`.
    system "/usr/local/Cellar/pvb/pvbrowser/pvbrowser", "pv://pvbrowser.org:5050"
  end
end
