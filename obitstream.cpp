#include "obitstream.h"
#include <cmath>
obitstream::obitstream(ofstream& stream)
    :write_to(stream),
    buffer(),
    position(0)
        {}

void obitstream::insert(const bitstring& to_insert){
    //For each bit in the bitstring, write it to the buffer.  When the buffer gets
    //full, write its contents to the file.
    for(unsigned int bits_written = 0;bits_written < to_insert.size();++bits_written){
        //Increment position after the bit is written.
        buffer[position++] = to_insert[bits_written];
        //If the buffer is full.
        if(position == BUFFER_SIZE){
	    auto as_long = buffer.to_ulong();
            //Write the contents of the buffer to the file.
            write_to.write(reinterpret_cast<char*>(&as_long),BUFFER_SIZE / 8);
            buffer.reset();
            position = 0;
        }
    }
}

//Write the contents of the buffer to the file and reset position.
void obitstream::flush(){
    auto as_long = buffer.to_ulong();
    //The number of bytes written to the file should be just enough to hold the
    // entire buffer, rounding up if necessary. This can cause the file to be
    //padded with zeroes.
    write_to.write(reinterpret_cast<char*>(&as_long),static_cast<int>(ceil(position / 8.0)));
    position = 0;
    buffer.reset();
}

//If bits remain in the buffer, write them to the file.
obitstream::~obitstream(){
    if(position < BUFFER_SIZE)
        flush();
}
