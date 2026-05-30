#include "DDA_Parser.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: DDA_Reader <dda_file>" << std::endl;
        return -1;
    }
    CDDAParser parser;
    return parser.ParseFile(argv[1]) ? 0 : -1;
}
