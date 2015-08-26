#include <algorithm>
#include "ibitstream.h"

using std::ifstream;
using std::set;
ibitstream::ibitstream(ifstream& source,const bitstring_set& valid_strings)
    :buffer(0),
    position(0),
    read_from(source),
    valid_strings(valid_strings),
    //Find the length of the longest string.
        max_length(std::accumulate(valid_strings.begin(),valid_strings.end(),0,
        [](bitstring::size_type a,const bitstring& b){return std::max(a,b.size());})),
        //bad_bit should be set if the underlying stream is invalid.
        bad_bit(!read_from)
    {
    fill_buffer();
}

bitstring ibitstream::extract(){
    //Read bits from the buffer, refilling it when necessary, until a valid bitstring
    //is read or the string was determined to be invalid.
    bitstring retval;
    //If the bad_bit is set then return an empty bitstring.
    if(bad_bit)
       return retval;
    //Extract bits from the file until those bits form a valid bitstring or the bitstring is longer than the
    //longest valid bitstring (which means that it is invalid).
    while(!valid_strings.count(retval) && retval.size() <= max_length){
        //Push the next bit into the bitstring.  Increment position after the bit is extracted.
        retval.push_back(buffer[position++]);
        //If the entire buffer was read then refill the buffer.
        if(position == BUFFER_SIZE){
            //If the end of the file was reached then activate the bad bit and an invalid sequence
            //was encountered.
            bad_bit = read_from.eof();
            if(bad_bit){
                retval.clear();
                return retval;
            }
            fill_buffer();

            position = 0;
        }
    }
    //If the loop was stopped because the bitstring was too long.
    if(retval.size() > max_length){
        bad_bit = true;
        retval.clear();
    }
    return retval;
}

//Read more from the file.
void ibitstream::fill_buffer(){
    int64_t read_to;
    read_from.read(reinterpret_cast<char*>(&read_to),sizeof(read_to));
    buffer = read_to;
}

ibitstream::operator bool(){
    return !(bad_bit);
}
