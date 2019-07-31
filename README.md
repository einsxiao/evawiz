The following componnents are required:
gcc/g++, nvidia-cuda-toolkit, python3, openmpi, fftw

working combination tested:
<gcc/g++-4.8, cuda-10.01>; <gcc/g++-5.4, cuda-10.01>

Steps to install (change ROOT to where you want push code):
```
ROOT=~/.evawiz/
mkdir $ROOT
cd $ROOT
git clone https://github.com/einsxiao/evawiz
./INSTALL
```