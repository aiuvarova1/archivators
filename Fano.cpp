//
// Created by aidenne on 04/04/2020.
//

#include "Fano.h"

void Fano::decompress(fstream &fin, fstream &fout) {
    unsigned int n;
    fin.read((char *) &n, sizeof(n));

    decode(fin);
    char ch;
    int offset = 0;
    int counter = 0;
    bool digit;

    Node *curNode = root;

    fin.read(&ch, sizeof(ch));
    while (counter != n) {
        digit = ch & (1 << (7 - offset));
        if (digit)
            curNode = curNode->right;
        else
            curNode = curNode->left;
        if (!curNode->empty) {
            fout.write(&curNode->value, sizeof(curNode->value));
            curNode = root;
            counter++;
        }
        offset++;
        if (offset == 8) {
            offset = 0;
            fin.read(&ch, sizeof(ch));
        }
    }
}

void Fano::compress(fstream &fin, fstream &fout, unsigned int n, map<char, long> *asciiTable) {
    encode(asciiTable);
    char ch;
    char byte = 0;

    int offset = 0;
    unsigned int size = probabilities.size();
    unsigned int freq;

    fout.write((char *) &n, sizeof(n));
    fout.write((char *) &size, sizeof(size));
    for (pair<char, long> ch : probabilities) {
        fout.write(&ch.first, sizeof(ch.first));
        freq = ch.second;
        fout.write((char *) &freq, sizeof(freq));
    }

    while (fin >> std::noskipws >> ch) {

        for (int bit : codes[ch]) {
            byte = byte | (bit << (7 - offset));
            offset++;
            if (offset == 8) {
                fout.write(&byte, sizeof(byte));
                offset = 0;
                byte = 0;
            }
        }
    }
    fout.write(&byte, sizeof(byte));
    clear();
}

void Fano::decode(fstream &fin) {
    clear();
    unsigned int size = 0;
    char ch = 0;
    unsigned int freq = 0;
    fin.read((char *) &size, sizeof(size));
    for (int i = 0; i < size; ++i) {
        fin.read((char *) &ch, sizeof(ch));
        fin.read((char *) &freq, sizeof(freq));
        probabilities.push_back(make_pair(ch, freq));
    }
    start();
    root = new Node();
    fano(0, size - 1, root);
}

unsigned int Fano::findMedian(unsigned int start, unsigned int end) {
    long toExtract = start > 0 ? sums[start - 1] : 0;
    long sum = sums[end] - toExtract;
    long curSum = probabilities[start].second;

    long mod = abs(curSum - (sum - curSum));
    long curMod;
    for (unsigned int i = start + 1; i < end + 1; ++i) {
        curSum += probabilities[i].second;
        curMod = abs(curSum - (sum - curSum));
        if (curMod >= mod)
            return i - 1;
        mod = curMod;
    }
    return end;
}

void Fano::fano(unsigned int start, unsigned int end) {
    if (start >= end)
        return;
    unsigned int median = findMedian(start, end);
    for (unsigned int i = median + 1; i < end + 1; ++i)
        codes[probabilities[i].first].push_back(1);

    for (unsigned int j = start; j <= median; ++j)
        codes[probabilities[j].first].push_back(0);

    fano(start, median);
    fano(median + 1, end);
}

void Fano::fano(unsigned int start, unsigned int end, Fano::Node *parent) {
    if (start >= end) {
        parent->value = probabilities[end].first;
        parent->empty = false;
        return;
    }
    unsigned int median = findMedian(start, end);

    if (median + 1 < end + 1)
        parent->right = new Node();
    if (start <= median)
        parent->left = new Node();

    fano(start, median, parent->left);
    fano(median + 1, end, parent->right);
}

void Fano::encode(map<char, long> *asciiTable) {
    clear();
    for (int j = 0; j < 256; ++j) {
        if (asciiTable->at(j) != 0) {
            probabilities.push_back(make_pair(j, asciiTable->at(j)));
        }
    }
    sort(probabilities.begin(), probabilities.end(), comparator);
    start();
    fano(0, probabilities.size() - 1);
}

void Fano::clear() {
    codes.clear();
    probabilities.clear();
    sums.clear();
}

void Fano::start() {
    long sum = 0;
    for (int i = 0; i < 256; ++i) {
        sums.push_back(probabilities[i].second + sum);
        sum = sums.back();
    }
}
