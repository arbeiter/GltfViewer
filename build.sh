if [[ ! -d bin ]]; then
  exit
fi
cd bin
cmake -DCMAKE_BUILD_TYPE=Debug ..; make
