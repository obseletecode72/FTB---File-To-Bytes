#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "File argument required." << std::endl;
        return 1;
    }
    std::string input = argv[1];
    std::string outputFile = input + "_output.txt";

    std::ifstream inFile(input, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file: " << input << std::endl;
        return 1;
    }
    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inFile)),
        std::istreambuf_iterator<char>());
    inFile.close();

    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error creating file: " << outputFile << std::endl;
        return 1;
    }
    outFile << "{";
    int count = 0;
    for (size_t i = 0; i < fileData.size(); ++i) {
        ++count;
        outFile << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)fileData[i] << std::dec << ",";
        if (count % 16 == 0)
            outFile << "\n";
        else
            outFile << " ";
    }
    outFile << "};";
    outFile.close();

    std::ifstream inFileOut(outputFile);
    std::stringstream buffer;
    buffer << inFileOut.rdbuf();
    std::string content = buffer.str();
    inFileOut.close();

    std::cout << "Content to be copied: " << content.substr(0, 100) << "..." << std::endl;

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, content.size() + 1);
        if (hGlob) {
            char* pGlob = static_cast<char*>(GlobalLock(hGlob));
            memcpy(pGlob, content.c_str(), content.size() + 1);
            GlobalUnlock(hGlob);
            SetClipboardData(CF_TEXT, hGlob);
        }
        CloseClipboard();
    }
    else {
        std::cerr << "Error opening clipboard." << std::endl;
    }

    std::cout << "Content copied to clipboard (Ctrl + C)" << std::endl;
    std::filesystem::remove(outputFile);
    return 0;
}
