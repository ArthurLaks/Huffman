FLAGS = -std=c++11 -Wall -Wextra -O3 -ggdb
huffman: main.o 
	g++ $(FLAGS) -o huffman main.o create_encoding.o obitstream.o \
ibitstream.o encoding_table.o
main.o: main.cpp create_encoding.o obitstream.o ibitstream.o encoding_table.o
	g++ $(FLAGS) -c -o main.o  main.cpp
create_encoding.o: create_encoding.cpp create_encoding.h type_defs.h
	g++ $(FLAGS) -c -o create_encoding.o create_encoding.cpp

obitstream.o: obitstream.cpp obitstream.h type_defs.h
	g++ $(FLAGS) -c -o obitstream.o obitstream.cpp
ibitstream.o: ibitstream.h ibitstream.cpp create_encoding.h type_defs.h
	g++ $(FLAGS) -c -o ibitstream.o ibitstream.cpp

encoding_table.o: encoding_table.cpp encoding_table.h type_defs.h
	g++ $(FLAGS) -c -o encoding_table.o encoding_table.cpp

clear:
	rm -f *.o *~ 
