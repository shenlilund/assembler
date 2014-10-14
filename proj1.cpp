#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdlib.h>

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

	int reg[8];
	char mem[3000];

	string line;
	string word;

	//assembler pass 1 ---------------------------------------------------------------------------------
	int address = 0;
	int dataAddress = 0;
	int beginData = 0;
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
				if (words[1] == ".BYT" || words[1] == ".INT")
				{
					symbolTable[words[0]] = dataAddress;
				}
				else
				{
					symbolTable[words[0]] = address;
				}
			}
			// for (int i = 0; i < words.size(); i++)
			// {
			// 	cout << words[i] << endl;
			// }
			if (words[0] == ".BYT" || words[0] == ".INT" || words[1] == ".BYT" || words[1] == ".INT")
			{
				dataAddress++;
			}
			else
			{
				address+=INSTRSIZE;
				beginData = address;
			}
		}

		
	}
	// cout << beginData << endl;
	// cout << symbolTable["FIVE"] << endl;

	infile.close();
	infile.open(argv[1]);

	address = 0;
	dataAddress = 0;
	//assembler pass 2 -----------------------------------------------------------------------------------
	while (getline(infile, line))
	{
		vector<string> words;
		istringstream iss(line);
		bool first = true;
		while (iss >> word)
		{
			//for space characters
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
				if (word[1] == '\\' && word[2] == 'n')
				{
					string temp1;
					temp1 = word.erase(0,1);
					temp1 = word.erase(2,1);
					words.push_back(temp1);
				}
				else
				{
					string temp;
					temp = word[1];
					words.push_back(temp);
				}
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
		// for (int i = 0; i < words.size(); i++)
		// {
		// 	cout << words[i] << endl;
		// }	
		if (!words.empty())
		{
			for (int j = 0; j < instructions.size(); j++)
			{
				if (instructions[j] ==  words[0])
				{
					int* ptr = static_cast<int*>(static_cast<void*>(&mem[address]));
					int* ptr1 = static_cast<int*>(static_cast<void*>(&mem[beginData+dataAddress]));
					if (words[0] == ".INT")
					{
						string temp;
						temp = words[1];
						int temp1 = atoi(temp.c_str());
						*ptr1 = temp1;	
					}
					else if (words[0] == ".BYT")
					{
						if (words[0] == "\n")
						{
							string temp;
							temp = words[1];
							char temp1 = temp[0];
							mem[beginData+dataAddress] = temp1;
							temp1 = temp[1];
							mem[beginData+dataAddress+1] = temp1;
						}
						else
						{	
							string temp;
							temp = words[1];
							char temp1 = temp[0];
							mem[beginData+dataAddress] = temp1;
						}
					}
					else
					{
						*ptr = j;
					}

					// label
					if (words[0] == "JMP")
					{
						if (!(symbolTable.find(words[1]) == symbolTable.end()))
						{
							*(ptr+1) = symbolTable[words[1]];
						}
						else
						{
							cout << "Label "+ words[1] +" not found" << endl;
							return -1;
						}

					}
					// register
					else if (words[0] == "JMR")
					{
						int temp;
						temp = words[1][1] - '0';
						*(ptr+1) = temp;
					}
					// register, label
					else if (words[0] == "BNZ" || words[0] == "BGT" || words[0] == "BLT" || words[0] == "BRZ" || words[0] == "LDA" || words[0] == "STR" || words[0] == "LDR" || words[0] == "STB" || words[0] == "LDB")
					{
						int temp;
						temp = words[1][1] - '0';
						*(ptr+1) = temp;

						if (!(symbolTable.find(words[2]) == symbolTable.end()))
						{
							*(ptr+2) = symbolTable[words[2]];
						}	
						else
						{
							cout << "Label "+ words[2] +" not found" << endl;
							return -1;
						}
					}
					// register, register
					else if (words[0] == "MOV" || words[0] == "ADD" || words[0] == "SUB" || words[0] == "MUL" || words[0] == "DIV" || words[0] == "AND" || words[0] == "OR" || words[0] == "CMP")
					{
						int temp;
						temp = words[1][1] - '0';
						int temp1;
						temp1 = words[2][1] - '0';
						*(ptr+1) = temp;
						*(ptr+2) = temp1;
					}
					// register, immediate
					else if (words[0] == "ADI")
					{
						int temp;
						temp = words[1][1] - '0';
						string temp1;
						temp1 = words[2].erase(0,1);
						int temp2 = atoi(temp1.c_str());
						*(ptr+1) = temp;
						*(ptr+2) = temp2;
					}
					else if (words[0] == "TRP")
					{
						string temp;
						temp = words[1];
						int temp1 = atoi(temp.c_str());
						*(ptr+1) = temp1;
					}
				}
			}
			if (words[0] == ".BYT" || words[0] == ".INT")
			{
				dataAddress+=INTSIZE;
			}
			else
			{
				address+=INSTRSIZE;
			}
		}

	
	}

	// cout << mem[36] << endl;
	// int* ptr2 = static_cast<int*>(static_cast<void*>(&mem[60]));
	// cout << *ptr2 << endl;
	// cout << *(ptr2+1) << endl;
	// cout << *(ptr2+2) << endl;
	// cout << *(ptr2+4) << endl;


	//virtual machine ----------------------------------------------------------------------------------
	bool running = true;
	int PC = 0;
	while (running)
	{
		int* ptrPC = static_cast<int*>(static_cast<void*>(&mem[PC]));
		int* ptrData = static_cast<int*>(static_cast<void*>(&mem[beginData]));	
		switch(*ptrPC)
		{
			case 0:
				if (*(ptrPC+1) == 0)
				{
					running = false;
				}
				else if (*(ptrPC+1) == 1)
				{
					cout << reg[7];
					PC+=INSTRSIZE;
				}
				else if (*(ptrPC+1) == 3)
				{
					char* charPtr = static_cast<char*>(static_cast<void*>(&reg[7]));
					cout << *charPtr;
					PC+=INSTRSIZE;
				}
				break;
			case 7:
				reg[*(ptrPC+1)] = reg[*(ptrPC+2)];
				PC+=INSTRSIZE;
				break;
			case 10:
				reg[*(ptrPC+1)] = *(ptrData+(*(ptrPC+2)));
				PC+=INSTRSIZE;
				break;
			case 13:
				reg[*(ptrPC+1)] += reg[*(ptrPC+2)];
				PC+=INSTRSIZE;
				break;
			case 15:
				reg[*(ptrPC+1)] -= reg[*(ptrPC+2)];
				PC+=INSTRSIZE;
				break;
			case 16:
				reg[*(ptrPC+1)] *= reg[*(ptrPC+2)];
				PC+=INSTRSIZE;
				break;
			case 17:
				reg[*(ptrPC+1)] /= reg[*(ptrPC+2)];
				PC+=INSTRSIZE;
				break;
		}
	}


	// while(Running) 
	// {
	// 	switch(*PC) 
	// 	{
	// 		case ADD: 
	// 			reg[instr[PC].opd1] =
	// 				reg[instr[PC].opd1] + reg[instr[PC].opd2];
	// 			PC++;
	// 			break;

	// 		case MOV:
	// 			reg[instr[PC].opd1] = reg[instr[PC].opd2];
	// 			PC++;
	// 			break;

	// 		case LDR:
	// 			reg[instr[PC].opd1] = mem[instr[PC].opd2];
	// 			PC++;
	// 			break;
	// 	}
	// 	PC+=INSTRSIZE;
	// }

}