With start_pvbapp you can start pvbrowser + pvserver like a normal application

usage: start_pvbapp inifile

start_pvbapp will
- read the inifile
- start the pvserver in the background
- start pvbrowser

See: example.ini.linux

For example you may write the following line in your ~/.bashrc

alias pvdemo='start_pvbapp /opt/pvb/start_pvbapp/example.ini.linux &'

