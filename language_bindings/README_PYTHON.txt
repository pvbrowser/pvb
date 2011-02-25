In order to use python for pvbrowser development,
you have to create the language binding first.
(if the binding is not already available)

For this run:
./build_python_interface.sh <swig>
from within this directory

Create a new pvserver with pvdevelop and "Use Python".
Within your pvserver please adjust "pvs_init.py".
You will have to adjust the path to the python lib within the project file.

Windows users will have to:
SET PYTHONPATH=%PVBDIR%\win\bin;%PYTHONPATH%
or
SET PYTHONPATH=%PVBDIR%\win-mingw\bin;%PYTHONPATH%

In the project file you may have to adjust the version number of your python library.

