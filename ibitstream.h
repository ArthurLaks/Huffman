#ifndef IBITSTREAM_H_
#define IBITSTREAM_H_
#include <fstream>
#include <set>
#include <bitset>
#include "type_defs.h"
//This class reads bitstrings from a file.  It keeps on reading bits from the file
//until they form a valid bitstring, then returns the bitstring.
class ibitstream{
public:
    //Takes the file to read and the set of valid bitstrings.
    ibitstream(std::ifstream&,const bitstring_set&);
    //Will only return a bistring if it is a member of valid_strings.
    bitstring extract();
    //Determines if the end of the file or an invalid bitstring was reached.
    operator bool();

private:
    const static unsigned int BUFFER_SIZE = 64;
    std::bitset<BUFFER_SIZE> buffer;
    //The position within the buffer.  It starts at 0 and goes up to BUFFER_SIZE.
    unsigned int position;
    std::ifstream& read_from;
    const bitstring_set& valid_strings;
    //The length of the longest bitstring in valid_bitstrings.
    unsigned int max_length;
    bool bad_bit;
    void fill_buffer();
};


#endif // IBITSTREAM_H_
