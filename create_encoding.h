#ifndef CREATE_ENCODING_H_INCLUDED
#define CREATE_ENCODING_H_INCLUDED
#include <unordered_map>
#include <deque>
#include "type_defs.h"
using std::deque;
using std::unordered_map;

encoding_t create_encoding(unordered_map<short int,size_t>);


#endif // CREATE_ENCODING_H_INCLUDED
