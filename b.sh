SIZE="800 800"
BIN=build/a2

${BIN} -size ${SIZE} -input data/scene06_bunny_1k.txt -bounces 4 -output out/a06.png -normals out/a06n.png -depth 8 18 out/a06d.png