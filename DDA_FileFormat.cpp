#include "DDA_FileFormat.h"

void CDDA_FileFormat::ReadXMLDefinitionFile(unsigned int version)
{
	if (version == 0)
	{
		std::cout << "[ERROR] DDA version not found!" << std::endl;
		return;
	}
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("DDA_Definition.xml");
	std::cout << "DDA_Definition: " << result.description() << std::endl;

	pugi::xml_node defListNode = doc.child("DDA_Definitions");
	for (pugi::xml_node definition = defListNode.child("DDA_Definition"); definition; definition = definition.next_sibling("DDA_Definition"))
	{
		if (version != definition.attribute("version").as_uint())
		{
			continue;
		}
		std::cout << "DDA Version : " << definition.attribute("version").value() << std::endl;

		SDDADefinition def;
		def.DDAversion = definition.attribute("version").as_int();
		def.frequency = definition.attribute("frequency").as_int();
		def.clock = definition.attribute("clock").as_float();
		def.headerSize = definition.attribute("headersize").as_int();

		pugi::xml_node paramListNode = definition.child("EntryToRead");
		for (pugi::xml_node param = paramListNode.child("Param"); param; param = param.next_sibling("Param"))
		{
			SDDAParam pa;

			char* nameStr = (char *)malloc((strlen(param.attribute("name").value()) + 1)*sizeof(char));
			sprintf_s(nameStr, strlen(param.attribute("name").value()) + 1, "%s", param.attribute("name").value());

			pa.name = nameStr;
			pa.bitsize = param.attribute("bitsize").as_int();
			pa.interval = param.attribute("interval").as_float();

			if (!param.attribute("operator").empty())
			{				
				if (strcmp(param.attribute("operator").value(), "add") == 0)
				{
					pa.op = SDDAParam::add;
				}
				if (strcmp(param.attribute("operator").value(), "sub") == 0)
				{
					pa.op = SDDAParam::sub;
				}
				if (strcmp(param.attribute("operator").value(), "mul") == 0)
				{
					pa.op = SDDAParam::mul;
				}
				if (strcmp(param.attribute("operator").value(), "div") == 0)
				{
					pa.op = SDDAParam::div;
				}
				if (strcmp(param.attribute("operator").value(), "dif") == 0)
				{
					pa.op = SDDAParam::dif;
				}
				
				pa.val = param.attribute("value").as_int();
			}

			def.inputParameters.push_back(pa);
		}

		m_definition = def;
	}
}

int CDDA_FileFormat::InitDefinition(unsigned int version)
{
	pugi::xml_document doc;
	if (doc.load_file("DDA_Definition.xml"))
	{
		ReadXMLDefinitionFile(version);
		return 1;
	}
	else
	{
		std::cout << "[ERROR] while trying to open DDA_Definition.xml" << std::endl;
		return -1;
	}
}

void CDDA_FileFormat::PrintDefinition()
{
	std::cout << "DDA file base info: version=" << m_definition.DDAversion << " clock=" << m_definition.clock << " headerSize=" << m_definition.headerSize << std::endl;
	std::cout << "Parameter List:" << std::endl;

	std::vector<SDDAParam>::iterator it = m_definition.inputParameters.begin();
	while (it != m_definition.inputParameters.end())
	{
		std::cout << "Param: name=" << (it)->name << " bitsize=" << (it)->bitsize << " interval=" << (it)->interval << std::endl;
		it++;
	}
}