cd ..\language_bindings
call build_python_interface.bat
call build_lua_interface.bat  
copy *.py                      ..\win-mingw\bin\
copy *.dll                     ..\win-mingw\bin\
copy lua\pvslua\pvslua.exe     ..\win-mingw\bin\
copy lua\pvapplua\pvapplua.exe ..\win-mingw\bin\
cd ..\win-mingw
