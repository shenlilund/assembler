#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

int INSTRSIZE = 12;
int INTSIZE = 4;

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        cout << "Filename required\n";
        return -1;
    }

    ifstream infile;	

	try
	{
		infile.open(argv[1]);
	}
	catch (int e)
	{
		cout << "Could not open file.\n"; 
		return -1;
	}


	vector<string> instructions;
	instructions.push_back("TRP");
	instructions.push_back("JMP");
	instructions.push_back("JMR");
	instructions.push_back("BNZ");
	instructions.push_back("BGT");
	instructions.push_back("BLT");
	instructions.push_back("BRZ");
	instructions.push_back("MOV");
	instructions.push_back("LDA");
	instructions.push_back("STR");
	instructions.push_back("LDR");
	instructions.push_back("STB");
	instructions.push_back("LDB");
	instructions.push_back("ADD");
	instructions.push_back("ADI");
	instructions.push_back("SUB");
	instructions.push_back("MUL");
	instructions.push_back("DIV");
	instructions.push_back("AND");
	instructions.push_back("CMP");
	instructions.push_back(".INT");
	instructions.push_back(".BYT");

	int registers[7];
	char mem[3000];

	string line;
	string word;

	//assembler pass 1
	int address = 0;
	map<string,int> symbolTable;
	while (getline(infile, line))
	{
		vector<string> words;
		istringstream iss(line);
		while (iss >> word)
		{
			if (word[0] == ';')
				break;
			words.push_back(word);
		}

		if (!words.empty())
		{			
			bool label = true;
			for (int j = 0; j < instructions.size(); j++)
			{
				if (instructions[j] ==  words[0])
				{
					label = false;
				}
			}
			if (label == true && symbolTable.find(words[0]) == symbolTable.end())
			{
				symbolTable[words[0]] = address;
			}

			// for (int i = 0; i < words.size(); i++)
			// {
			// 	cout << words[i] << endl;
			// }
		}
		address+=INSTRSIZE;
	}

	infile.close();
	infile.open(argv[1]);

	// cout << "HI" << endl;
	// cout << symbolTable["SECOND"] << endl;

	 address = 0;
	//second pass
	while (getline(infile, line))
	{
		vector<string> words;
		istringstream iss(line);
		bool first = true;
		while (iss >> word)
		{
			if (word == "'")
			{
				char temp = iss.get();
				string tempStr;
				tempStr = temp;
				words.push_back(tempStr);
				break;
			}
			else if (word[0] == '\'')
			{
				string temp;
				temp = word[1];
				words.push_back(temp);
			}
			else
			{
				if (word[0] == ';')
					break;
				bool label = true;
				for (int i = 0; i < instructions.size(); i++)
				{
					if (instructions[i] == word)
					{
						label = false;
					}
				}
				if (label == false || first == false)
				{
					words.push_back(word);
					first = false;
				}
			}
			
		}
		for (int i = 0; i < words.size(); i++)
		{
			cout << words[i] << endl;
		}	
		if (!words.empty())
		{
			for (int j = 0; j < instructions.size(); j++)
			{
				if (instructions[j] ==  words[0])
				{
					int* ptr = static_cast<int*>(static_cast<void*>(&mem[address]));
					*ptr = j;
					if (words[0] == "JMP")
					{
						*(ptr+1) = symbolTable[words[0]];
					}
					else if (words[0] == "JMR")
					{
						
					}
				}
			}
		}

		address+=INSTRSIZE; 
	}

}