set mpich_path=C:\MPICH2\bin
set qt_path=C:\Qt\5.14.0\msvc2015_64\bin
set path=%mpich_path%;%qt_path%;%path%

start "" "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" build\x64\DistributedSimulatedAnnealing.sln

