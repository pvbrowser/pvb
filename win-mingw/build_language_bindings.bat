cd ..\language_bindings
call build_python_interface.bat
call build_lua_interface.bat  
copy *.py                              ..\win-mingw\bin\
copy release\*.dll                     ..\win-mingw\bin\
copy lua\pvslua\release\pvslua.exe     ..\win-mingw\bin\
copy lua\pvapplua\release\pvapplua.exe ..\win-mingw\bin\
cd ..\win-mingw
