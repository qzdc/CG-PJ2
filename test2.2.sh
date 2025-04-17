mkdir ./build
mkdir ./out
cd ./build
cmake ..
make
cd ..

SIZE="800 800"
BIN=./build/a2

${BIN} -size ${SIZE} -input data/scene01_plane.txt  -output out/a01.png -normals out/a01n.png -depth 8 18 out/a01d.png
${BIN} -size ${SIZE} -input data/scene02_cube.txt   -output out/a02.png -normals out/a02n.png -depth 8 18 out/a02d.png
