#include "DDA_Parser.h"

using namespace std;

int main(int argc, char *argv[])
{
	int result = -1;
	if (argc < 2)
	{
		return result;
	}

	ifstream myDDAfile;
	myDDAfile.open(argv[1]);
	if (myDDAfile.is_open())
	{
		myDDAfile.close();
		std::cout << "DDA file " << argv[1] << " exists!" <<std::endl;
		CDDAParser* myParser = new CDDAParser();
                myParser->ParseFile(argv[1]);

		result = 1;
	}

	std::cout << std::endl << std::endl << "Press any key!" << std::endl;
	char a;
	// scanf_s("%c", &a);
	scanf("%c", &a);
	
	return result;
}
