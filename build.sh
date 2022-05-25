if [[ ! -d bin ]]; then
  exit
fi
cd bin
rm -rf *
cmake ..; make
./OpenGLTestApp
