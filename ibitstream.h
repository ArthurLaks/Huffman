#ifndef IBITSTREAM_H_
#define IBITSTREAM_H_
#include <fstream>
#include <set>
#include <bitset>
#include "type_defs.h"
class ibitstream{
public:
    ibitstream(std::ifstream&,const bitstring_set&);
    bitstring extract();
    //Determines if the end of the file was reached.
    operator bool();

private:
    const static unsigned int BUFFER_SIZE = 64;
    std::bitset<BUFFER_SIZE> buffer;
    unsigned int position;
    std::ifstream& read_from;
    const bitstring_set& valid_strings;
    unsigned int max_length;
    bool bad_bit;
    void fill_buffer();
};


#endif // IBITSTREAM_H_
