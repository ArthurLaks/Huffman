#include "encoding_table.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <utility>
#include <iostream>
#include <unordered_map>

void write_bitstring(std::ofstream& dest,size_t max_bytes,const bitstring& str);
void write_table(std::ofstream& destination,const encoding_t& encoding){
    //Determine the longest bit string in the encoding.
    unsigned max_bits = std::accumulate(encoding.begin(),encoding.end(),0,
        [](const bitstring::size_type acc,const std::pair<char,bitstring>& c_elem){return std::max(acc,c_elem.second.size());}) + 1;
    char max_bytes = ceil(max_bits / 8.0);
    //Prefix the file with the number of bytes used to represent each string
    destination.write(&max_bytes,1);

    for(auto map_iter = encoding.begin();map_iter != encoding.end();++map_iter){
        //Save the EOF character for the end of the table.
        if(map_iter->first == EOF){
            continue;
        }
        destination.put(static_cast<unsigned char>(map_iter->first));
        //Write the character and then the sequence of bits used to represent the character.
        write_bitstring(destination,max_bytes,map_iter->second);

    }
    //Mark the end of the table by repeating the first character in it.
    destination.put(static_cast<unsigned char>(encoding.begin()->first));
    write_bitstring(destination,max_bytes,encoding.at(EOF));

}
void write_bitstring(std::ofstream& dest,size_t max_bytes,const bitstring& str){
    char* buffer = new char[max_bytes]();

    //Write the sequence of bits backwards, starting from the most significant bit of the last byte of the
    //buffer
    int position = max_bytes * 8 - 1;
    for(auto deque_iter = str.rbegin();deque_iter != str.rend();++deque_iter,--position){
        buffer[position / 8] |= static_cast<unsigned char>(*deque_iter) << (position % 8);
    }
    //Insert the extra 1.
    buffer[position / 8] |= static_cast<unsigned char>(1) << (position % 8);
    dest.write(buffer,max_bytes);
    delete [] buffer;
}

bitstring read_bitstring(std::ifstream& source,size_t max_length);
std::unordered_map<bitstring,short int> read_table(std::ifstream& source){
    decoding_t retval;

    //The first character of the file is the number of bits in each encoding.
    char max_length;
    source.read(&max_length,1);
    //The first character in the table is the signal that will be used for the end of the table.
    unsigned char first_char = source.peek();
    //For each entry in the table, write the character and the sequence of bits to the map.
    while(true){
        unsigned char character = source.get();

        //The repetition of the first character from the table is the signal that the table is over.
        if(!retval.empty() && character == first_char){
            break;
        }
        retval.insert(std::make_pair(read_bitstring(source,max_length),static_cast<unsigned int>(character)));
    }
    retval.insert(std::make_pair(read_bitstring(source,max_length),EOF));
    return retval;
}

bitstring read_bitstring(std::ifstream& source,size_t max_length){
    char* buffer = new char[max_length]();
    source.read(buffer,max_length);
    bitstring retval;
    bool found_one = false;
    for(unsigned position = 0;position < max_length * 8;++position){
        if(!found_one){
            found_one = buffer[position / 8] & static_cast<unsigned>(1) << (position % 8);
        }else{
            retval.push_back(buffer[position / 8] & static_cast<unsigned>(1) << (position % 8));
        }
    }
    delete [] buffer;
    return retval;
}
