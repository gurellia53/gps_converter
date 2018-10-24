rm test_docs/OutDoc0_win.txt
rm test_docs/OutDoc1_win.txt
cd build_win64
x86_64-w64-mingw32-g++ -static -Wall ../main.cpp -o gps_converter.exe
wine-stable gps_converter.exe ../test_docs/InDoc0.txt ../test_docs/OutDoc0_win.txt 42.029006 -93.628679
wine-stable gps_converter.exe ../test_docs/InDoc1.txt ../test_docs/OutDoc1_win.txt 42.029006 -93.628679
cd ../