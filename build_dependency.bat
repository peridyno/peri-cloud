set BOOST_INCLUDE_DIR=D:\Dev\Library\boost_1_73_0
set BOOST_DIR=D:\Dev\Library\boost_1_73_0\stage\lib\cmake

mkdir dependency\build
cd dependency\build
cmake .. -DBoost_INCLUDE_DIR=%BOOST_INCLUDE_DIR% -DBoost_DIR=%BOOST_DIR%
cmake --build . --config Release 
pause