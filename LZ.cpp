//
// Created by aidenne on 04/04/2020.
//

#include "LZ.h"

void LZ::writeByte(char &byte, fstream &fout, int *offset, unsigned int num, int top) {

    std::bitset<32> bs = std::bitset<32>(num);
    for (int i = top - 1; i >= 0; i--) {
        byte = byte | (bs[i] << (7 - *offset));
        *offset = *offset + 1;
        if (*offset == 8) {
            fout.write(&byte, sizeof(byte));
            *offset = 0;
            byte = 0;
        }
    }
}

void LZ::compress(fstream &fout, unsigned int n, string &str) {
    unsigned int i = 0;
    unsigned int histStart, bufEnd;
    unsigned int start, length;
    char new_char;
    vector<Triple *> triples;

    while (i < n) {
        histStart = i - history > i ? 0 : i - history;
        bufEnd = min(n, i + buf);
        length = 0;
        start = 0;
        new_char = str[i];

        for (unsigned int j = histStart; j < i; ++j) {
            unsigned int equal = i;
            unsigned int search = j;
            while (equal < bufEnd && str[equal] == str[search]) {
                equal++;
                search++;
            }

            if (length < equal - i) {
                length = equal == bufEnd ? equal - i - 1 : equal - i;
                start = i - j;
                new_char = equal == bufEnd ? str[equal - 1] : str[equal];
            }
        }

        triples.push_back(new Triple(start, length, new_char));
        i += max(1u, length + 1);
    }

    unsigned int size = triples.size();
    fout.write((char *) &size, sizeof(size));

    int offset = 0;
    char byte = 0;

    char start_top = std::log2(history) + 1;
    char length_top = std::log2(window) + 1;

    fout.write(&start_top, sizeof(start_top));
    fout.write(&length_top, sizeof(length_top));

    for (Triple *t : triples) {
        writeByte(byte, fout, &offset, t->start, start_top);
        writeByte(byte, fout, &offset, t->length, length_top);
        writeByte(byte, fout, &offset, t->ch, 8);
    }
    fout.write(&byte, sizeof(byte));
}

unsigned int LZ::readByte(fstream &fin, long top, int *offset, char &ch) {
    int counter = 0;
    unsigned int digit = 0;
    int temp;

    while (counter != top) {
        temp = (ch & (1 << (7 - *offset)));
        if (temp)
            digit |= 1 << (top - 1 - counter);
        *offset = *offset + 1;
        counter++;
        if (*offset == 8) {
            *offset = 0;
            fin.read(&ch, sizeof(ch));
        }
    }
    return digit;
}

void LZ::decompress(fstream &fin, fstream &fout) {
    string str;
    unsigned int start, length;
    char ch;
    char byte;
    char start_top, length_top;
    unsigned int n;
    int offset = 0;

    fin.read((char *) &n, sizeof(n));
    fin.read(&start_top, sizeof(start_top));
    fin.read(&length_top, sizeof(length_top));

    unsigned int pos = 0;
    fin.read(&ch, sizeof(ch));

    for (unsigned int j = 0; j < n; j++) {
        start = readByte(fin, start_top, &offset, byte);
        length = readByte(fin, length_top, &offset, byte);
        ch = (char) readByte(fin, 8, &offset, byte);

        for (unsigned int i = pos - start; i < pos - start + length; ++i) {
            str += str[i];
            fout.write(&str[i], sizeof(str[i]));
        }

        str += ch;
        fout.write(&ch, sizeof(ch));

        pos += length + 1;
    }
}


