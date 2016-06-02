#include <iostream>
#include <fstream>

#include "DDA_Parser.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return -1;
	}

	char a;
	scanf_s("%c", &a);

	ofstream myDDAfile;
	myDDAfile.open(argv[1]);
	if (myDDAfile.is_open())
	{
		printf_s("DDA file %s exists!\n", argv[1]);
		CDDAParser* myParser = new CDDAParser();

		CDDA_FileFormat* myDDAFileFormat = new CDDA_FileFormat();
		if (myDDAFileFormat->InitDefinition()==1)
		{
			printf_s("DDA XML definition file loaded!\n");
			myDDAFileFormat->PrintDefinition();

			myParser->ParseFile(argv[1], myDDAFileFormat->GetDefinition());
			
			return 1;
		}

		myDDAfile.close();
	}

	return -1;
}