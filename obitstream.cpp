#include "obitstream.h"
#include <cmath>
obitstream::obitstream(ofstream& stream)
    :write_to(stream),
    buffer(),
    position(0)
        {}

void obitstream::insert(const bitstring& to_insert){
    for(unsigned int bits_written = 0;bits_written < to_insert.size();++bits_written){
        buffer[position++] = to_insert[bits_written];
        if(position == BUFFER_SIZE){
	    auto as_long = buffer.to_ulong();
            write_to.write(reinterpret_cast<char*>(&as_long),BUFFER_SIZE / 8);
            buffer.reset();
            position = 0;
        }
    }
}

void obitstream::flush(){
    auto as_long = buffer.to_ulong();
    write_to.write(reinterpret_cast<char*>(&as_long),static_cast<int>(ceil(position / 8.0)));
    position = 0;
    buffer.reset();
}

obitstream::~obitstream(){
    if(buffer.any())
        flush();
}
