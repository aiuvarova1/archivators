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

#ifndef KDZ_LZZ_H
#define KDZ_LZZ_H


class LZ {
private:
    unsigned int window;
    unsigned int history;
    unsigned int buf;

    class Triple {

    public:
        unsigned int start;
        unsigned int length;
        char ch;

        Triple(unsigned int start, unsigned int length, char ch) {
            this->start = start;
            this->length = length;
            this->ch = ch;
        }
    };

    unsigned int readByte(fstream &fin, long top, int *offset, char &ch);

    void writeByte(char &byte, fstream &fout, int *offset, unsigned int num, int top);

public:

    LZ(unsigned int window, unsigned int history) {
        this->window = window * 1024;
        this->history = history * 1024;
        this->buf = this->window - this->history;
    }

    void compress(fstream &fout, unsigned int n, string &str);

    void decompress(fstream &fin, fstream &fout);
};


#endif //KDZ_LZZ_H
