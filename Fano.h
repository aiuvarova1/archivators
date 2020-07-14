//
// Created by aidenne on 04/04/2020.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <bitset>
#include <cmath>

using namespace std;

#ifndef KDZ_FANO_H
#define KDZ_FANO_H


class Fano {

    class Node {
    public:
        Node *left;
        Node *right;
        char value;
        bool empty = true;


        ~Node() {
            delete left;
            delete right;
        }
    };

    map<char, vector<int>> codes;
    Node *root;
    vector<pair<char, long>> probabilities;
    vector<long> sums;

    static bool comparator(const pair<char, long> &a,
                           const pair<char, long> &b) {
        return (a.second > b.second);
    }


    unsigned int findMedian(unsigned int start, unsigned int end);

    ///Fano without implicit tree (compression)
    void fano(unsigned int start, unsigned int end);

    ///Fano with explicit tree (decompression)
    void fano(unsigned int start, unsigned int end, Node *parent);

    void clear();

    void encode(map<char, long> *asciiTable);

    void start();

    void decode(fstream &fin);


public:
    void compress(fstream &fin, fstream &fout, unsigned int n, map<char, long> *asciiTable);

    void decompress(fstream &fin, fstream &fout);

    ~Fano() {

        delete root;
    }
};


#endif //KDZ_FANO_H
