for file in src/*.cpp
do
	outfile="obj/$(basename $file).o"
	g++ -c $file -o $outfile
done

g++ obj/*.cpp.o -o zatmos -lraylib
