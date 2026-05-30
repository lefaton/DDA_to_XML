#include "DDA_Parser.h"
#include "DDA_Operators.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
bool CDDAParser::GetBytes(std::ifstream& file, char* buffer, unsigned int size, unsigned int offset) {
    file.seekg(offset, std::ios::beg);
    if (file.fail()) { std::cout<<"[ERROR] seekg failed at offset "<<offset<<std::endl; return false; }
    file.read(buffer, size);
    if (file.fail() && !file.eof()) { std::cout<<"[ERROR] read failed at offset "<<offset<<std::endl; return false; }
    return true;
}
bool CDDAParser::WriteDDAXMLFile(const char* fileName, std::ifstream& stream) {
    pugi::xml_document doc;
    pugi::xml_node rootNode = doc.append_child("DDA_file");
    rootNode.append_attribute("version").set_value(m_definition.DDAversion);
    rootNode.append_attribute("frequency").set_value(m_definition.frequency);
    unsigned int counter = 0, freq = m_definition.frequency;
    char buffer[BUFFER_SIZE];
    std::vector<unsigned int> lastValue(m_definition.inputParameters.size(), 0);
    unsigned int bytesPerCycle = 0;
    for (const auto& p : m_definition.inputParameters) {
        unsigned int e = static_cast<unsigned int>(freq * p.interval); if (e==0) e=1;
        bytesPerCycle += (freq/e) * (p.bitsize/BYTE_SIZE);
    }
    if (bytesPerCycle==0) { std::cout<<"[ERROR] bytesPerCycle=0"\n'; return false; }
    unsigned int totalFrames = ((m_fileSize-m_definition.headerSize)/bytesPerCycle)*freq;
    std::cout<<"Decoding DDA file ("<<totalFrames<<" frames):"<<std::endl;
    for (unsigned int f = 0; f < totalFrames; ++f) {
        pugi::xml_node entry = rootNode.append_child("entry");
        std::ostringstream ts; ts<<std::fixed<<std::setprecision(2)<<(double)f/freq;
        entry.append_attribute("timing").set_value(ts.str().c_str());
        std::cout<<"\r"<<(f*100/totalFrames)<<"%";
        std::size_t i=0;
        for (const auto& param : m_definition.inputParameters) {
            unsigned int ev = static_cast<unsigned int>(freq*param.interval); if(ev==0) ev=1;
            if (counter==0 || counter%ev==0) {
                unsigned int nb = param.bitsize/BYTE_SIZE;
                std::memset(buffer,0,NUFFER_SIZE);
                if (!GetBytes(stream,buffer,nb,m_seekPos)) return false;
                unsigned int v=0; std::memcpy(&v,buffer,nb);
                lastValue[i]=v; m_seekPos+=nb;
            }
            pugi::xml_node n = entry.append_child(param.name.c_str());
            n.append_child(pugi::node_pcdata).set_value(std::to_string(ApplyOperator(lastValue[i],param.op,param.val)).c_str());
            ++i;
        }
        counter=(counter+1)%freq;
    }
    std::string out(fileName);
    if (out.size()>=4 && out.substr(out.size()-4)==".dda") out=out.substr(0,out.size()-4)+".xml"; else out+=".xml";
    std::cout<<std::endl<<"Writing XML: ";
    if (doc.save_file(out.c_str())) { std::cout<<"OK -> "<<out<<std::endl; return true; }
    std::cout<<"[ERROR] Could not write "<<out<<std::endl; return false;
}
bool CDDAParser::ParseFile(const char* fileName) {
    m_seekPos = 0;
    std::ifstream f(fileName, std::ifstream::binary|std::ios::in);
    if (!f.is_open()||!f.good()) { std::cout<<"[ERROR] Cannot open "<<fileName<<std::endl; return false; }
    unsigned char vbyte=0; f.read(reinterpret_cast<char*>(&vbyte),1);
    if (f.fail()) { std::cout<<"[ERROR] Cannot read version byte."<<std::endl; return false; }
    CDDA_FileFormat ff;
    if (!ff.InitDefinition(vbyte)) { std::cout<<"[ERROR] XML definition file error."<<std::endl; return false; }
    m_definition = *ff.GetDefinition();
    f.seekg(0,std::ios::end); m_fileSize=(unsigned int)f.tellg(); f.seekg(0,std::ios::beg);
    if (m_definition.headerSize<5) { std::cout<<"[ERROR] Header too small."<<std::endl; return false; }
    std::vector<char> hh(m_definition.headerSize,0);
    if (!GetBytes(f,hh.data(),m_definition.headerSize,0)) { std::cout<<"[ERROR] Cannot read header."<<std::endl; return false; }
    if (!(hh[2]=='D'&&hh[3]=='D'&&hh[4]=='A')) { std::cout<<"[ERROR] Not a DDA file: "<<fileName<<std::endl; return false; }
    m_seekPos = m_definition.headerSize;
    return WriteDDAXMLFile(fileName,f);
}
