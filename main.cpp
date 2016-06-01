#include <iostream>

#include "DDA_FileFormat.h"

using namespace std;


int main(int argc, char *argv[])
{
	printf("test");

	CDDA_FileFormat* myDDAFileFormat = new CDDA_FileFormat();
	myDDAFileFormat->OpenXMLDefinitionFile();
}