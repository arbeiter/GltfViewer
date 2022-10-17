if [[ ! -d bin ]]; then
  exit
fi
cd bin
cmake ..; make
./bin/src/MinimalOpenGL 1
