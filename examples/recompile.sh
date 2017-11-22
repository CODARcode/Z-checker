cd ..
#make clean;
make;make install

cd examples
make clean -f Makefile.bk
make -f Makefile.bk

