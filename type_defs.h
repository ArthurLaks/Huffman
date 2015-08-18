/*
This file contains type definitions that are used elsewhere in the program.  It defines types to represent sequences
of bits, maps between sequences of bits and chars, and sets of sequences of bits.  In order to create a hash table
of deques of bits, it defines a functor to serve as a hash function for a sequence of bits.
*/
#ifndef TYPE_DEFS_H_
#define TYPE_DEFS_H_
#include <functional>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <vector>
typedef std::vector<bool> bitstring;
typedef std::unordered_map<short int,bitstring> encoding_t;

//A functor that defines a function for bitstrings.  It converts the sequence of bits as if it was a binary number
//except that it insert a 1 at the beginning.
namespace std{
template <>
struct hash<bitstring>{
    size_t operator() (const bitstring& element) const{
        //For each bit, shift the previous bit over by one and add the current bit to it.
        return std::accumulate(element.begin(),element.end(),1,
            [](unsigned acc,bool bit){return (acc << 1) | bit;});
    }
};
}
typedef std::unordered_map<bitstring,short int>  decoding_t;
typedef std::unordered_set<bitstring> bitstring_set;
#endif // TYPE_DEFS_H_
