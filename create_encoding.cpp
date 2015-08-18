#include "create_encoding.h"
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>
#include <utility>
using std::pair;
using std::priority_queue;
using std::vector;

//Represents a node in the tree that is used to determine the encoding.  Is only used inside this file.
struct Node{
    short int character;
    //The combined frequency of all of its children
    size_t frequency;
    Node* zero;
    Node* one;
    bool operator<(const Node& rhs){
        return frequency < rhs.frequency;
    }
    Node(char value,size_t frequency)
        :character(value),frequency(frequency),zero(nullptr),one(nullptr)
        {}
    Node(size_t frequency,Node* zero,Node* one)
        :character('\0'),frequency(frequency),zero(zero),one(one)
        {}
};

encoding_t transverse(Node*);
encoding_t create_encoding(unordered_map<short int,size_t> frequencies){
    //The priority queue is sorted by frequency, so the node less frequency will be on top.
    priority_queue<Node*,std::vector<Node*>,std::function<bool(Node*,Node*)> > frequency_queue(
            [](Node* a,Node* b){return *b < *a;},
            std::vector<Node*>());

    //Create nodes for each character with the character and its frequency.
    for(auto cFrequency:frequencies){
        frequency_queue.push(new Node(cFrequency.first,cFrequency.second));
    }

    //Reduce the size of the priority queue to 1 by removing the least frequent two nodes, combining them under one
    //parent, and inserting that into the queue.
    while(frequency_queue.size() > 1){
        Node* nodeA = frequency_queue.top();
        frequency_queue.pop();
        Node* nodeB = frequency_queue.top();
        frequency_queue.pop();

        Node* parent = new Node(nodeA->frequency + nodeB->frequency,nodeA,nodeB);
        frequency_queue.push(parent);
    }
    Node* tree = frequency_queue.top();
    encoding_t encoding =  transverse(tree);
    for(auto iter = encoding.begin();iter != encoding.end();++iter){
        std::reverse(iter->second.begin(),iter->second.end());
    }
    return encoding;
}

encoding_t transverse(Node* node){
    if(node->one){
            auto zeroes = transverse(node->zero);
            for(auto iter = zeroes.begin();iter != zeroes.end();++iter){
                iter->second.push_back(false);
            }
            auto ones = transverse(node->one);
            for(auto iter = ones.begin();iter != ones.end();++iter){
                iter->second.push_back(true);
            }
            zeroes.insert(ones.begin(),ones.end());
            delete node;
            return zeroes;
    }else{
        encoding_t retval({std::make_pair(node->character,bitstring())});

        delete node;
        return retval;
    }
}

