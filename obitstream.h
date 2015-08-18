#ifndef OBIT_STREAM_H_
#define OBIT_STREAM_H_
#include <bitset>
#include <fstream>
#include "create_encoding.h"
//This class writes bitstrings to a file.
using std::ofstream;
const unsigned BUFFER_SIZE = 64;
class obitstream{
public:
    explicit obitstream(ofstream&);
    //Insert a bitstring into the stream.
    void insert(const bitstring&);
    //Inserts the contents of the buffer into the file.
    void flush();
    //Flushes the buffer if necessary.
    ~obitstream();
private:
    ofstream& write_to;
    std::bitset<BUFFER_SIZE> buffer;
    //The position within the buffer.  It goes up from 0 to BUFFER_SIZE.
    unsigned int position;
};


#endif // OBIT_STREAM_H_
