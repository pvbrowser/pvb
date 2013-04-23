REM
REM clean.bat will delete all files created during compilation
REM

FOR /F "tokens=*" %%G IN ('DIR /B /S *.a') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.py') DO DEL "%%G"

cd ..

FOR /F "tokens=*" %%G IN ('DIR /B /S *~') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.o') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S moc_*') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S qrc_*') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S ui_*') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S Makefile.*') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.Release') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.pdb') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.exe') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.obj') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.dll') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *object_script.*') DO DEL "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *debug*') DO RMDIR /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *release*') DO RMDIR /S /Q "%%G"

cd win-mingw

