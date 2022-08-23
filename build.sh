if [[ ! -d bin ]]; then
  exit
fi
cd bin
#rm -rf *; -DCMAKE_BUILD_TYPE=Debug ..
cmake ; make
./bin/src/MinimalOpenGL 1
