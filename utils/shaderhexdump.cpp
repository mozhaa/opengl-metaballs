#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "glob.hpp"

char buffer[2391];
const int maxBytesInLine = 120 / 6;

void append_byte(std::ofstream& fout, unsigned int value) {
    static int bytesInLine = 0;
    if (value > 0xff) {
        value -= 0xffffff00;
    }
    if (value >= 128) {
        fout << "-";
        value = 256 - value;
    }
    fout << "0x" << std::setw(2) << std::setfill('0') << std::hex << value << ", ";
    ++bytesInLine;
    if (bytesInLine == maxBytesInLine) {
        fout << std::endl;
        bytesInLine = 0;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <sourceDir> <headerFile>" << std::endl;
        return 1;
    }

    std::string sourceDirectory(argv[1]);
    std::string headerFilename(argv[2]);
    std::ofstream fout(headerFilename);

    // #include <cstddef>
    //
    // static const char <arrayName>[] = {
    //  /*... hexadecimal data from source file ...*/
    // };
    //
    // size_t <arrayName>_length = sizeof(<arrayName>) / sizeof(char);

    fout << "#ifndef SHADERS_H_INCLUDED_" << std::endl;
    fout << "#define SHADERS_H_INCLUDED_" << std::endl;
    fout << std::endl;
    fout << "#include <cstddef>" << std::endl;
    
    for (auto& sourcePath : glob::glob(sourceDirectory + "/*.*")) {
        std::ifstream fin(sourcePath, std::ios::binary);
        if (!fin) {
            std::cerr << "Can't open file " << sourcePath << "!" << std::endl;
            return 1;
        }
        
        std::string arrayName = sourcePath.filename();
        std::replace(arrayName.begin(), arrayName.end(), '.', '_');
        fout << std::endl;
        fout << "static const char " << arrayName << "[] = {" << std::endl;

        int bytesInLine = 0;

        std::streamsize n;
        do {
            fin.read(buffer, sizeof(buffer) / sizeof(char));
            n = fin.gcount();
            for (std::streamsize i = 0; i < n; ++i) {
                unsigned int value = (unsigned int) buffer[i];
                append_byte(fout, value);
            }
        } while (n > 0);
        append_byte(fout, 0);

        if (bytesInLine > 0)
            fout << std::endl;

        fout << "};" << std::endl;
        fout << std::endl;
        fout << "size_t " << arrayName << "_length = sizeof(" << arrayName << ") / sizeof(char);" << std::endl;

        fin.close();
    }

    fout << std::endl;
    fout << "#endif" << std::endl;
    fout.close();

    return 0;
}
