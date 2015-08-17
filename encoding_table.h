#ifndef ENCODING_TABLE_H_
#define ENCODING_TABLE_H_
#include <fstream>
#include <map>
#include "type_defs.h"

void write_table(std::ofstream& destination,const encoding_t& encoding);
decoding_t read_table(std::ifstream& source);

#endif // ENCODING_TABLE_H_
