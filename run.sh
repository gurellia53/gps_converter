rm test_docs/OutDoc0.txt
rm test_docs/OutDoc1.txt
cd build
g++ -Wall ../main.cpp -o gps_converter
cd ../
./gps_converter test_docs/InDoc0.txt test_docs/OutDoc0.txt 42.029006 -93.628679
./gps_converter test_docs/InDoc1.txt test_docs/OutDoc1.txt 42.029006 -93.628679
