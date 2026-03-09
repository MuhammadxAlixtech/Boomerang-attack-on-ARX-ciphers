#include <iostream>
#include <fstream>
#include "chacha.hpp"

// Write block words as separate CSV columns
void writeBlockWords(const block* x, std::ofstream& file, const std::string& prefix) {
    for (int i = 0; i < 16; i++) {
        file <<(int) x->w[i].bits;
        if (!(prefix == "CipherText" && i == 15)) {
            file << ",";
        }
    }
}
void generateWithID(block* plainText, uint32_t IDWordIndex, uint32_t IDBitIndex, std::ofstream& file) {
    block x0, x1;
    x0 = *plainText;
    x1 = *plainText;

    // flip one bit in x1
    x1.w[IDWordIndex].bits ^= (1 << IDBitIndex);

    // Apply 2 rounds (odd + even)
    for (int iter = 1; iter <= 2; iter++) {  
        if (iter % 2 == 0) {
            evenRounds(&x0);
            evenRounds(&x1);
        } else {
            oddRounds(&x0);
            oddRounds(&x1);
        }
    }

    // Row: IDWordIndex, IDBitIndex, PlainText_0..15, CipherText1_0..15, CipherText2_0..15
    file << IDWordIndex << "," << IDBitIndex << ",";
    xorBlock(&x0,&x1);
    writeBlockWords(plainText, file, "PlainText");
    writeBlockWords(&x0, file, "CipherText");
    file << "\n";
}

int main() {
    std::ofstream file("dataset.csv");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file" << std::endl;
        return 1;
    }

    // Write CSV header
    file << "IDWordIndex,IDBitIndex";
    for (int i = 0; i < 16; i++) file << ",PlainText_" << i;
    for (int i = 0; i < 16; i++) file << ",CipherTextDiff_" << i;
    file << "\n";

    // Generate dataset
    file <<std::hex;
    block plainTextArr[10000];

    for (int plainTextcnt = 0; plainTextcnt <10000; plainTextcnt++) {
        initialize(&plainTextArr[plainTextcnt]);
    }
    for(int plainTextcnt=0;plainTextcnt<10000;plainTextcnt++){
        for (int i = 12; i < 16; i++) {
            for (int j = 0; j < 8; j++) {
                generateWithID(&plainTextArr[plainTextcnt], i, j, file);
            }
        }
    }

    file.close();
    return 0;
}
