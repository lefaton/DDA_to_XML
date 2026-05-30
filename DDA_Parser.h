/*
DDA_Reader - Copyright (C) 2016 Matthieu Dirrenberger
This program idea is mainly based on the work done by Andrew Allan in 2009.
The white paper he wrote and the code he gave me was really helpful for speeding up
the reverse engineering part (http://www.ducati.ms/).
This software XML parser/writer is based on pugixml library (http://pugixml.org).
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine.
*/
#pragma once
#include "DDA_FileFormat.h"
constexpr bool         DEBUG_ON    = false;
constexpr unsigned int BYTE_SIZE   = 8;
constexpr unsigned int BUFFER_SIZE = 16;
class CDDAParser {
public:
    CDDAParser()  = default;
    ~CDDAParser() = default;
    CDDAParser(const CDDAParser&)            = delete;
    CDDAParser& operator=(const CDDAParser&) = delete;
    bool ParseFile(const char* fileName);
private:
    bool GetBytes(std::ifstream& file, char* buffer, unsigned int size, unsigned int offset);
    bool WriteDDAXMLFile(const char* fileName, std::ifstream& stream);
    SDDADefinition m_definition;
    unsigned int   m_fileSize = 0;
    unsigned int   m_seekPos  = 0;
};
