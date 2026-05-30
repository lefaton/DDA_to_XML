#include "DDA_FileFormat.h"
bool CDDA_FileFormat::ReadXMLDefinitionFile(unsigned int version) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("DDA_Definition.xml");
    std::cout << "DDA_Definition: " << result.description() << std::endl;
    if (!result) return false;
    pugi::xml_node defListNode = doc.child("DDA_Definitions");
    for (pugi::xml_node def = defListNode.child("DDA_Definition"); def; def = def.next_sibling("DDA_Definition")) {
        if (version != def.attribute("version").as_uint()) continue;
        std::cout << "DDA Version : " << def.attribute("version").value() << std::endl;
        SDDADefinition d;
        d.DDAversion = def.attribute("version").as_uint();
        d.frequency  = def.attribute("frequency").as_uint();
        d.clock      = def.attribute("clock").as_float();
        d.headerSize = def.attribute("headersize").as_uint();
        for (pugi::xml_node param = def.child("EntryToRead").child("Param"); param; param = param.next_sibling("Param")) {
            SDDAParam pa;
            pa.name     = param.attribute("name").value();
            pa.bitsize  = param.attribute("bitsize").as_uint();
            pa.interval = param.attribute("interval").as_float();
            if (!param.attribute("operator").empty()) {
                std::string o = param.attribute("operator").value();
                if (o=="add") pa.op=SDDAParam::EOperator::add;
                else if(o=="sub") pa.op=SDDAParam::EOperator::sub;
                else if(o=="mul") pa.op=SDDAParam::EOperator::mul;
                else if(o=="div") pa.op=SDDAParam::EOperator::div;
                else if(o=="dif") pa.op=SDDAParam::EOperator::dif;
                else std::cout<<"[WARNING] Unknown operator '"<<o<<"' for param '"<<pa.name<<"' - treated as nil."<<std::endl;
                pa.val = param.attribute("value").as_uint();
            }
            d.inputParameters.push_back(pa);
        }
        m_definition = std::move(d);
        return true;
    }
    std::cout<<"[ERROR] DDA version "<<version<<" not found!"<<std::endl;
    return false;
}

bool CDDA_FileFormat::InitDefinition(unsigned int version) {
    if (version==0) { std::cout<<"[ERROR] DDA version 0 is invalid."<<std::endl; return false; }
    return ReadXMLDefinitionFile(version);
}

void CDDA_FileFormat::PrintDefinition() const {
    std::cout<<"DDA base info: version="<<m_definition.DDAversion<<" clock="<<m_definition.clock<<" headerSize="<<m_definition.headerSize<<std::endl;
    for (const auto& p : m_definition.inputParameters)
        std::cout<<"  Param: name="<<p.name<<" bitsize="<<p.bitsize<<" interval="<<p.interval<<std::endl;
}
