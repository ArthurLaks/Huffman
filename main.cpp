/*
 Contains the main function.  The main function will determine whether to compress or decompress a file based on
 the command line arguments.  If the user choose to compress a file it will perform the following steps:
1) Opens the input file as a text file
2) Count the occurences of each character,
3) uses the create_encoding function to generate the encoding
4) Opens the output file as a binary file
5) Uses the write_table function to write the encoding table to form the header of the output file
6) Goes back to the beginning of the input file, reads each character, and write the encoding of the character to the
output file (using the obitstream class).

If the user choose to decompress a file, it performs the following steps:
1) Opens the input file as a binary file
2) Uses the read_table function to read the encoding table from the file
3) Opens the output file as a text file
4) Uses the ibitstream class to read each valid sequence of bits from the input file, writing the decoding of those
bits to the output file
5) Stops when the ibitstream encounters an invalid sequence or reaches the end of the file.

*/
#include <iostream>
#include <utility>
#include <cstring>
#include <fstream>
#include <set>
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>
#include "type_defs.h"
#include "create_encoding.h"
#include "obitstream.h"
#include "ibitstream.h"
#include "encoding_table.h"
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;
using std::map;
using std::deque;
using std::set;
using std::make_pair;
using std::istreambuf_iterator;

const char MAGIC_NUMBER[] = "huff";
const size_t MG_LEN = sizeof(MAGIC_NUMBER) - 1;
int main(int argc,char* argv[]){
    //The program expects 3 arguments (besides for the name of the program): -c or -d for compress or decompress,
    //the name of the input file, and the name of the output file.
    if(argc < 4 || (strcmp(argv[1],"-c") != 0 && strcmp(argv[1],"-d") != 0)){
        std::cerr << "Expected usage: ./huffman -c | -d input_file output_file" << endl;
        return 1;
    }
    //If the user choose to compress a file.
    if(argv[1][1] == 'c'){
        ifstream input_file(argv[2],ios::in);   //The second argument is the name of the input file.

        //Count the frequency of each character in the file.
        unordered_map<char,size_t> frequencies;
        //For each character in the file, increment the frequency
        for(istreambuf_iterator<char> input_file_iterator(input_file);
                input_file_iterator != std::istreambuf_iterator<char>();++input_file_iterator){
            frequencies[*input_file_iterator]++;
        }
        //The create_encoding function returns an unordered_map of characters to deques of bools which represent
        //the encoding of the characters.
        auto encoding = create_encoding(frequencies);
        cout << "Here is the encoding: " << endl;
        for(auto cpair:encoding){

                cout << static_cast<char>(cpair.first) << '\t';
            for(unsigned counter = 0;counter < cpair.second.size();++counter){
                cout << cpair.second[counter];
            }
            cout << endl;
        }

        ofstream output_file(argv[3],ios::binary | ios::out);
        //Write magic number to file.
        output_file.write(MAGIC_NUMBER,MG_LEN);
        //Write the encoding table to the file.
        write_table(output_file,encoding);

        obitstream output_file_stream(output_file);
        //Compress the file and write it to the output file.
        //Go back to the beginning of the file
        input_file.clear(); //Clear the status flags in order to clear the eof bit.
        input_file.seekg(0,ios::beg);
        //Read every character from the input file and write its encoding to the output file
        for(istreambuf_iterator<char> input_file_iter(input_file);
                input_file_iter != istreambuf_iterator<char>();++input_file_iter){
            output_file_stream.insert(encoding[*input_file_iter]);
        }
        output_file_stream.insert(encoding[EOF]);
    //If the user choose to decompress a file
    }else{
        //Decompress the file.
        ifstream input_file(argv[2],ios::binary | ios::in);
        //Determine if the magic number is correct.
        char mg_buffer[MG_LEN];
        input_file.read(mg_buffer,MG_LEN);
        if(memcmp(mg_buffer,MAGIC_NUMBER,MG_LEN) != 0){
            cerr << "Invalid file format." << endl;
            return 1;
        }
        //Read the encoding table from the file.
        //The read_table function returns an unordered_map mapping sequences of bits to characters
        auto encoding = read_table(input_file);
        cout << "Here is the encoding:" << endl;
        for(auto c_pair:encoding){
            cout << static_cast<char>(c_pair.second) << ":\t";
            for(unsigned counter = 0;counter < c_pair.first.size();++counter){
                cout << c_pair.first[counter];
            }
            cout << endl;
        }
        //The ibitstream class requires a set of valid bitstrings to know when to return the bitstring and when to
        //extract more bits and when it encounters an invalid sequence.
        bitstring_set valid_bitstrings;
        //Build the valid_bitstrings set from the map by inserting the first members of the key-value pairs
        // into the set.
        std::transform(encoding.begin(),encoding.end(),std::inserter(valid_bitstrings,valid_bitstrings.begin()),
            [](const map<bitstring,char>::value_type& kvp){return kvp.first;});
        ibitstream input_file_stream(input_file,valid_bitstrings);
        ofstream output_file(argv[3],ios::binary | ios::out);
        //The ibitstream class defines an implicit conversion to bool that returns true as long as there is valid
        //to read.
        while(input_file_stream){
            char current_char = encoding[input_file_stream.extract()];
            output_file << current_char;
        }
    }
    return 0;
}
