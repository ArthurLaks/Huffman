#ifndef OBIT_STREAM_H_
#define OBIT_STREAM_H_
#include <bitset>
#include <fstream>
#include "create_encoding.h"

using std::ofstream;
const unsigned BUFFER_SIZE = 64;
class obitstream{
public:
    explicit obitstream(ofstream&);
    void insert(const bitstring&);
    void flush();
    ~obitstream();
private:
    ofstream& write_to;
    std::bitset<BUFFER_SIZE> buffer;
    unsigned int position;
};


#endif // OBIT_STREAM_H_
