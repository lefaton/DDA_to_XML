/*
DDA_Reader - Copyright (C) 2016 Matthieu Dirrenberger
This program idea is mainly based on the work done by Andrew Allan in 2009.
The white paper he wrote and the code he gave me was really helpful for speeding up
the reverse engineering part (http://www.ducati.ms/).
This software XML parser/writer is based on pugixml library (http://pugixml.org).
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine.
*/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "libs/pugixml/src/pugixml.hpp"
struct SDDAParam {
    enum class EOperator { add, sub, mul, div, nil, dif };
    SDDAParam() : name(""), bitsize(8), interval(1.0f), op(EOperator::nil), val(0) {}
    std::string  name;
    unsigned int bitsize;
    float        interval;
    EOperator    op;
    unsigned int val;
};
struct SDDADefinition {
    unsigned int            DDAversion  = 0;
    unsigned int            frequency   = 0;
    float                   clock       = 0.0f;
    unsigned int            headerSize  = 0;
    std::vector<SDDAParam>  inputParameters;
};
class CDDA_FileFormat {
public:
    CDDA_FileFormat()  = default;
    ~CDDA_FileFormat() = default;
    CDDA_FileFormat(const CDDA_FileFormat&)            = delete;
    CDDA_FileFormat& operator=(const CDDA_FileFormat&) = delete;
    SDDADefinition*       GetDefinition()      { return &m_definition; }
    const SDDADefinition* GetDefinition() const { return &m_definition; }
    bool InitDefinition(unsigned int version);
    void PrintDefinition() const;
private:
    bool ReadXMLDefinitionFile(unsigned int version);
    SDDADefinition m_definition;
};
