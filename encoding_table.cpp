#include "encoding_table.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <utility>
#include <iostream>
#include <unordered_map>

void write_table(std::ofstream& destination,const encoding_t& encoding){
    //Determine the longest bit string in the encoding.
    unsigned max_bits = std::accumulate(encoding.begin(),encoding.end(),0,
        [](const bitstring::size_type acc,const std::pair<char,bitstring>& c_elem){return std::max(acc,c_elem.second.size());}) + 1;
    char max_bytes = ceil(max_bits / 8.0);
    //Prefix the file with the number of bytes used to represent each string
    destination.write(&max_bytes,1);

    char* buffer = new char[max_bytes];
    for(auto map_iter = encoding.begin();map_iter != encoding.end();++map_iter){
        memset(buffer,0,max_bytes);
        destination.put(map_iter->first);
        //Write the character and then the sequence of bits used to represent the character.


        //Write the sequence of bits backwards, starting from the most significant bit of the last byte of the
        //buffer
        int position = max_bytes * 8 - 1;
        //Start from the most significant bit of the last byte and end at the least significant bit of the first byte.
        for(auto deque_iter = map_iter->second.rbegin();deque_iter != map_iter->second.rend();++deque_iter,--position){
            //Set the position % 8 least significant bit in the position / 8 byte to the previous position in the
            //bitstring.
            buffer[position / 8] |= static_cast<unsigned char>(*deque_iter) << (position % 8);
        }
        //Insert the extra 1.
        buffer[position / 8] |= static_cast<unsigned char>(1) << (position % 8);
        destination.write(buffer,max_bytes);
        }
    //Mark the end of the table by repeating the first character in it.
    destination.put(static_cast<unsigned char>(encoding.begin()->first));
    delete [] buffer;
}


decoding_t read_table(std::ifstream& source){
    decoding_t retval;

    //The first character of the file is the number of bits in each encoding.
    char max_length;
    source.read(&max_length,1);
    //The first character in the table is the signal that will be used for the end of the table.
    unsigned char first_char = source.peek();
    char* buffer = new char[max_length];
    //For each entry in the table, write the character and the sequence of bits to the map.
    while(true){
        char character = source.get();

        //The repetition of the first character from the table is the signal that the table is over.
        if(!retval.empty() && character == first_char){
            break;
        }

        //Read the buffer from the file and write its content to a bitstring.
        source.read(buffer,max_length);
        bitstring representation;   //The representation of the character.
        //Go through the buffer, from the least significant bit of the first byte to the most significant
        //bit of the last byte, until encountering the 1 that marks the boundary between the padding and
        //the bitstring.  After it is encountered, push the remaining bits to the bitstring.
        bool found_one = false;
        for(int position = 0;position < max_length * 8;++position){
            if(!found_one){
                found_one = buffer[position / 8] & static_cast<unsigned>(1) << (position % 8);
            }else{
                representation.push_back(buffer[position / 8] & static_cast<unsigned>(1) << (position % 8));
            }
        }
        //Insert the character and its representation into the encoding.
        retval.insert(std::make_pair(representation,character));
    }
    delete [] buffer;
    return retval;
}


