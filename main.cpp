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
#include <numeric>
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
using std::pair;
using std::istreambuf_iterator;
using std::string;

const char MAGIC_NUMBER[] = "huff";
const size_t MG_LEN = sizeof(MAGIC_NUMBER) - 1;
void compress_file(ifstream& input_file,ofstream& output_file);
bool decompress_file(ifstream& input_file,ofstream& output_file);
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
        if(!input_file){
            cerr << "Cannot read file " << argv[2] << endl;
            return 1;
        }
        ofstream output_file(argv[3],ios::out | ios::binary);
        //Write magic number to file.
        output_file.write(MAGIC_NUMBER,MG_LEN);
        compress_file(input_file,output_file);
    //If the user choose to decompress a file
    }else{
        ifstream input_file(argv[2],ios::binary | ios::in);
        if(!input_file){
            cerr << "Cannot read file " << argv[2] << endl;
            return 1;
        }
        ofstream output_file(argv[3],ios::out);
        //Determine if the magic number is correct.
        char mg_buffer[MG_LEN];
        input_file.read(mg_buffer,MG_LEN);
	//If the magic number is incorrect.
        if(memcmp(mg_buffer,MAGIC_NUMBER,MG_LEN) != 0){
            cerr << "Invalid file type. " << endl;
            return 1;
        }
	//If the decompress_file function failed then the file is corrupt.
        if(!decompress_file(input_file,output_file)){
            cerr << "The file is corrupt.";
            return 2;
	}
    }
    return 0;
}

void compress_file(ifstream& input_file,ofstream& output_file){
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

    //Find the least frequent character in the file, and use that as the EOF character.
    char eof_char;
    size_t freq = -1;   //-1 is the highest value that can be stored in an unsigned type.
    for(auto& c_pair:frequencies){
        if(c_pair.second < freq){
            eof_char = c_pair.first;
            freq = c_pair.second;
        }
    }

    //Write the encoding table to the file.
    write_table(output_file,encoding);

    //Write the eof character to the file, between the table and the contents
    output_file.put(eof_char);

    obitstream output_file_stream(output_file);
    //Compress the file and write it to the output file.
    //Go back to the beginning of the file
    input_file.clear(); //Clear the status flags in order to clear the eof bit.
    input_file.seekg(0,ios::beg);

    //Read every character from the input file and write its encoding to the output file
    for(istreambuf_iterator<char> input_file_iter(input_file);
            input_file_iter != istreambuf_iterator<char>();++input_file_iter){
        char c_char = *input_file_iter;
        //If the character is the same as is used to mark the end of the file, write it twice to escape it.
        if(c_char == eof_char){
            output_file_stream.insert(encoding[c_char]);
        }
        output_file_stream.insert(encoding[c_char]);
    }
    //Insert the eof character to mark the end of the file.
    output_file_stream.insert(encoding[eof_char]);
}

bool decompress_file(ifstream& input_file,ofstream& output_file){

    //Read the encoding table from the file.
    //The read_table function returns an unordered_map mapping sequences of bits to characters
    auto encoding = read_table(input_file);

    //The eof character is the one after the table.
    char eof_char = input_file.get();
    //The ibitstream class requires a set of valid bitstrings to know when to return the bitstring and when to
    //extract more bits and when it encounters an invalid sequence.
    bitstring_set valid_bitstrings;
    //Build the valid_bitstrings set from the map by inserting the first members of the key-value pairs
    // into the set.
    std::transform(encoding.begin(),encoding.end(),std::inserter(valid_bitstrings,valid_bitstrings.begin()),
        [](const map<bitstring,char>::value_type& kvp){return kvp.first;});
    ibitstream input_file_stream(input_file,valid_bitstrings);
    //The ibitstream class defines an implicit conversion to bool that returns true as long as there is valid
    //to read.
    while(input_file_stream){
        char current_char = encoding[input_file_stream.extract()];
        //If the character is the end of file character and it was not escaped then the end of the file was reached.
        if(current_char == eof_char && (!input_file_stream || encoding[input_file_stream.extract()] != eof_char)){
            return true;
        }
        output_file << current_char;
    }
    return false;
}
