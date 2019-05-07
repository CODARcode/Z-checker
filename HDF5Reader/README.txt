#COMPILATION

1. Install HDF5 to make sure h5cc works.
2. Linux: make -f Makefile.linux
   OSX:   make -f Makefile.osx

If you want to run HDF5 together with SZ and ZFP, you need to use Makefile.linux2. 
For simplicity, installing z-checker-installer would be more convenient. 
http://github.com/CODARcode/z-checker-installer

After downloading z-checker-installer, please use z-checker-installer.sh to install it, and then read testHDF5/README.txt for details.
