mkdir -p src/ obj/ bin/
for file in src/*.cpp
do
	outfile="obj/$(basename $file).o"
	g++ -c $file -o $outfile -Wall -Wextra -Wpedantic
done

g++ obj/*.cpp.o -o bin/zatmos -lraylib -Wall -Wextra -Wpedantic
