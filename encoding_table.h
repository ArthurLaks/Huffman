#ifndef ENCODING_TABLE_H_
#define ENCODING_TABLE_H_
#include <fstream>
#include <map>
#include "type_defs.h"

//Writes the table to the file.
void write_table(std::ofstream& destination,const encoding_t& encoding);
//Returns the table read from the file.
decoding_t read_table(std::ifstream& source);

#endif // ENCODING_TABLE_H_
