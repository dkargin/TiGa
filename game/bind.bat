echo "executing SWIG script"

rem ..\LuaBox\swig.exe -c++ -lua -Fmicrosoft -l../game/bind.i -o ../game/bind.cpp -external-runtime ../game/bind.h

echo "Generating cpp binding"
..\LuaBox\swig.exe -c++ -v -addextern -lua -Fmicrosoft -o ../game/bind.cpp ../game/bind.i 

rem echo "Generating xmd export description"
rem ..\LuaBox\swig.exe -c++ -v -addextern -xml -Fmicrosoft ../game/bind.i