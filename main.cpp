/// КДЗ по дисциплине Алгоритмы и структуры данных, 2019-2020 уч.год
/// Уварова Анастасия Игоревна, группа БПИ-181, дата (06.04.2020)
/// Среда разработки - Clion
/// Состав проекта: main.cpp, Fano.h, Fano.cpp, LZ.h, LZ.cpp
/// Что сделано: сжатие и распаковка методом Шеннона -Фано, сжатие и распаковка методом LZ77, проведен вычислительный эксперимент, построены таблицы и графики,
/// для измерения времени выполнения использовалась библиотека chrono, оформлен отчет
/// Не сделан алгоритм LZW

#include <iostream>
#include <map>
#include <bitset>
#include <chrono>

#include "Fano.h"
#include "LZ.h"

using namespace std;

static map<char, long> asciiTable;
static const int NUM_OF_CHARS = 256;
static string formats[10] = {"docx", "jpg", "bmp", "bmp", "pdf", "djvu", "jpg", "pptx", "dll", "txt"};
static const string PATH = "../DATA/";
static const int ITERATIONS = 10;

static ofstream FREQ_FILE;
static ofstream TIME_FILE;

/// Writing frequencies and entropy to csv
double countEntropy(int length, string num) {
    double entropy = 0;
    double w;

    FREQ_FILE << num;
    for (int i = 0; i < NUM_OF_CHARS; ++i) {
        w = (double) asciiTable[i] / length;
        FREQ_FILE << ", " + to_string(w);
        if (w != 0)
            entropy -= w * log2(w);
    }

    FREQ_FILE << ", " + to_string(entropy / 8) + '\n';

    return entropy;
}

/// Reading input file and counting symbol frequencies
string readFile(fstream &fin, string &num) {
    asciiTable.clear();
    for (int i = 0; i < NUM_OF_CHARS; ++i)
        asciiTable[i] = 0;

    char input;
    string res;
    unsigned int n = 0;
    while (fin >> std::noskipws >> input) {
        n++;
        asciiTable[input]++;
        res += input;
    }
    countEntropy(res.length(), num);
    fin.clear();
    fin.seekg(0);

    return res;
}

/// Fano compressing/decompressing, counting time
void compressWithFano(fstream &fin, string num, string res) {
    Fano *fano;
    fstream fout;
    fstream packed;

    cout << "Fano: Compressing file " + num << endl;
    std::chrono::duration<long, std::ratio<1, 1000> > sum = std::chrono::duration<long, std::ratio<1, 1000> >::zero();

    for (int i = 0; i < ITERATIONS; ++i) {
        fout.open(PATH + num + ".shan", ios::out | ios::binary);
        fano = new Fano();
        auto start = chrono::system_clock::now();
        fano->compress(fin, fout, res.length(), &asciiTable);

        auto diff = chrono::system_clock::now() - start;
        auto ms = chrono::duration_cast<chrono::milliseconds>(diff);
        cout << "Compressed in " + to_string(ms.count()) + " ms\n";

        sum += ms;

        fin.clear();
        fin.seekg(0);
        fout.close();
        delete fano;
    }
    sum /= ITERATIONS;
    cout << "Average compression time: " + to_string(sum.count()) + '\n';
    TIME_FILE << num + ", , , " + to_string(sum.count()) + ", ";
    cout << "Fano: Decompressing file " + num + '\n';

    sum = std::chrono::duration<long, std::ratio<1, 1000> >::zero();
    for (int i = 0; i < ITERATIONS; ++i) {
        packed.open(PATH + num + ".shan", ios::in | ios::binary);
        fout.open(PATH + num + ".unshan", ios::out | ios::binary);
        fano = new Fano();

        auto start = chrono::system_clock::now();
        fano->decompress(packed, fout);
        auto diff = chrono::system_clock::now() - start;
        auto ms = chrono::duration_cast<chrono::milliseconds>(diff);

        cout << "Decompressed in " + to_string(ms.count()) + +" ms\n";
        sum += ms;
        delete fano;

        packed.close();
        fout.close();
    }
    sum /= ITERATIONS;
    TIME_FILE << to_string(sum.count()) + ", ";
    cout << "Average decompression time: " + to_string(sum.count()) + '\n';
    cout << "===============================\n";
}

/// Compressing/decompressing with LZ77 using specified window and history sizes
void compressWithLZ(fstream &fin, int window, int history, string res, string num) {
    fstream fout;
    fstream packed;
    LZ *lz;

    cout << "LZ77" + to_string(window) + ": Compressing file " + num << endl;
    std::chrono::duration<long, std::ratio<1, 1000> > sum = std::chrono::duration<long, std::ratio<1, 1000> >::zero();

    for (int i = 0; i < ITERATIONS; ++i) {
        lz = new LZ(window, history);
        fout.open(PATH + num + ".lz77" + to_string(window), ios::out | ios::binary);

        auto start = chrono::system_clock::now();
        lz->compress(fout, res.length(), res);
        auto diff = chrono::system_clock::now() - start;
        auto ms = chrono::duration_cast<chrono::milliseconds>(diff);
        cout << "Compressed in " + to_string(ms.count()) + " ms\n";

        sum += ms;
        fin.clear();
        fin.seekg(0);
        fout.close();
        delete lz;
    }

    sum /= ITERATIONS;
    cout << "Average compression time: " + to_string(sum.count()) + '\n';
    TIME_FILE << to_string(sum.count()) + ", ";

    sum = std::chrono::duration<long, std::ratio<1, 1000> >::zero();
    for (int i = 0; i < ITERATIONS; ++i) {
        lz = new LZ(window, history);

        packed.open(PATH + num + ".lz77" + to_string(window), ios::in | ios::binary);
        fout.open(PATH + num + ".unlz77" + to_string(window), ios::out | ios::binary);

        cout << "LZ77" + to_string(window) + ": Decompressing file " + num << endl;
        auto start = chrono::system_clock::now();
        lz->decompress(packed, fout);
        auto diff = chrono::system_clock::now() - start;
        auto ms = chrono::duration_cast<chrono::milliseconds>(diff);
        cout << "Decompressed in " + to_string(ms.count()) + " ms\n";

        sum += ms;
        packed.close();
        fout.close();

        delete lz;
    }

    sum /= ITERATIONS;
    TIME_FILE << to_string(sum.count()) + ", ";
    cout << "Average decompression time: " + to_string(sum.count()) + '\n';
    cout << "===============================\n";
}

///Writing column names to csv files
void fillCsv() {
    FREQ_FILE.open(PATH + "freq.csv");
    TIME_FILE.open(PATH + "time.csv");

    for (int i = 0; i < NUM_OF_CHARS; ++i) {
        FREQ_FILE << ", " + to_string(i);
    }
    FREQ_FILE << ", H \n";

    TIME_FILE << ", S1, H, Fano (tp), Fano (tu), LZ775 (tp), LZ775 (tu), LZ7710 (tp), LZ7710 (tu),"
                 "LZ7720 (tp), LZ7720 (tu)\n";
}

/// Experiment starting point
int main() {
    fillCsv();

    fstream fin;
    string file;

    for (int i = 0; i < 10; ++i) {
        file = to_string(i + 1);
        fin.open(PATH + file + "." + formats[i], ios::in | ios::binary);
        string res = readFile(fin, file);

        compressWithFano(fin, to_string(i + 1), res);
        compressWithLZ(fin, 5, 4, res, to_string(i + 1));
        compressWithLZ(fin, 10, 8, res, to_string(i + 1));
        compressWithLZ(fin, 20, 16, res, to_string(i + 1));

        cout << endl;
        fin.close();
        TIME_FILE << '\n';
    }
    FREQ_FILE.close();
    TIME_FILE.close();

    return 0;
}
