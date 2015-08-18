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
    char character;
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
encoding_t create_encoding(unordered_map<char,size_t> frequencies){
    //The priority queue is sorted by frequency, so the node less frequency will be on top.
    priority_queue<Node*,std::vector<Node*>,std::function<bool(Node*,Node*)> > frequency_queue(
            [](Node* a,Node* b){return *b < *a;},
            std::vector<Node*>());

    //Create nodes for each character with the character and its frequency.
    for(auto cFrequency:frequencies){
        frequency_queue.push(new Node(cFrequency.first,cFrequency.second));
    }

    //Reduce the size of the priority queue to 1 by removing the least frequent two nodes, combining them under one
    //parent, and inserting that into the queue, thereby constructing a tree.
    while(frequency_queue.size() > 1){
        Node* nodeA = frequency_queue.top();
        frequency_queue.pop();
        Node* nodeB = frequency_queue.top();
        frequency_queue.pop();

        //The new node's frequency should be the sum of the frequency of its children.
        Node* parent = new Node(nodeA->frequency + nodeB->frequency,nodeA,nodeB);
        frequency_queue.push(parent);
    }
    //The remaining node in the heap is the root of the tree.  Transverse the tree in order to obtain the encoding.
    Node* tree = frequency_queue.top();
    encoding_t encoding =  transverse(tree);
    //Since transverse calls push_back rather than push_front, the bitstrings have to be reversed.
    //Since transverse calls push_back rather than push_front, the bitstrings have to be reversed.
    for(auto iter = encoding.begin();iter != encoding.end();++iter){
        std::reverse(iter->second.begin(),iter->second.end());
    }
    return encoding;
}

//This function transverses the tree, constructing an encoding based on which ancestors are one branches and which
//are zero branches. It deallocates nodes as it goes.
encoding_t transverse(Node* node){
    //For a branch node, recursively call this function to obtain the hashtable for both of its children, then append
    //1 to the bitstrings from the one branch and 0 to the bitstrings from the zero branch, then merge the branches
    //by inserting the map for the one branch into the one for the zero branch.

    if(node->one){  //If one of the children is not empty then it is a branch.  one was chosen arbitrarily.
            auto zeroes = transverse(node->zero);
            for(auto iter = zeroes.begin();iter != zeroes.end();++iter){
                //The zero should have been appended to the beginning, but vector is used bitstring for greater
                //efficiency.  The create_encoding function will reverse the bitstrings.
                iter->second.push_back(false);
            }
            auto ones = transverse(node->one);
            for(auto iter = ones.begin();iter != ones.end();++iter){
                iter->second.push_back(true);
            }
            zeroes.insert(ones.begin(),ones.end());
            delete node;
            return zeroes;
    //If the node is a leaf then make a new map with the character and an empty bitstring.
    }else{
        encoding_t retval({std::make_pair(node->character,bitstring())});

        delete node;
        return retval;
    }
}

