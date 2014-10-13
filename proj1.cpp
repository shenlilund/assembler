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
					if (words[0] == ".INT")
					{
						string temp;
						temp = words[1];
						int temp1 = atoi(temp.c_str());
						*ptr = temp1;	
					}
					else if (words[0] == ".BYT")
					{
						string temp;
						temp = words[1];
						char temp1 = temp[0];
						mem[address] = temp1;
					}
					else
					{
						*ptr = j;
					}

					// label
					if (words[0] == "JMP")
					{
						*(ptr+1) = symbolTable[words[1]];
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
						*(ptr+2) = symbolTable[words[2]];
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
		}

		address+=INSTRSIZE; 
	}

	// cout << mem[36] << endl;
	// int* ptr1 = static_cast<int*>(static_cast<void*>(&mem[36]));
	// cout << *ptr1 << endl;
	// cout << *(ptr1+1) << endl;
	// cout << *(ptr1+2) << endl;

}