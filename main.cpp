
/*
 * File:   main.cpp
 * Author: trenton
 *
 * Created on July 26, 2017, 4:17 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;

/*
 *
 */
 //int luhnChecker(string);
 //bool uniqueChecker(int, int[], int**);
void progressUpdater(int);

int currentPart = -1;

int main(int argc, char** argv) {


	clock_t start;

	start = clock();

	time_t t;

	int sum = 0;
	int finalNum = 0;
	int cardNum[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int cardNum2[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int prefixes[9][4] = { {4,0,0,0},{6,0,1,1},{5,1,0,0},{5,2,0,0}, {5,3,0,0}, {5,4,0,0}, {5,5,0,0},{3,4,0,0},{3,7,0,0} };
	const int VA = 0;   //4xxx 
	const int DY = 1;   //6011 
	const int MC1 = 2;  //51xx 
	const int MC2 = 3;  //52xx 
	const int MC3 = 4;  //53xx 
	const int MC4 = 5;  //54xx
	const int MC5 = 6;  //55xx
	const int AX1 = 7;  //34xx
	const int AX2 = 8;  //37xx
	const int PH = 9;
	ofstream output;
	int input = 0;
	int** cards;
	int code = VA;
	int places = 0;
	int temp = 0;
	string cardType = "";
	string filename = "";
	string option = "";
	string arguments = "";
	string args[3];
	double duration;
	double dtemp;
	srand((unsigned)time(&t));

	//output the initial progress bar
	cout << "|" << right << "Generating Credit Card Numbers\r";

	//used for debugging
	/*for(int i = 0; i < argc; i++)
	{
		cout <<  "Argument " << i << ":" << argv[i] << endl;
	}*/

	//let them know if the tool is being used incorrectly
	if (argc != 3)
	{
		cout << "Error" << endl;
		cout << "Usage: <number to generate> <type of card>" << endl;
		cout << "\t -v Visa\n\t -m Mastercard \n\t -a American Express \n\t -d Discover" << endl;
		return 0;
	}

	//cout << "Input <number to generate> <type of card>" << endl;
	//check which kind of cards they are trying to generate and set the code variable accordingly
	cardType = argv[2];
	if (cardType == "-v")
	{
		code = VA;
	}
	else if (cardType == "-m")
	{
		code = MC1;
	}
	else if (cardType == "-a")
	{
		code = AX1;
	}
	else if (cardType == "-d")
	{
		code = DY;
	}

	//put the input number into a numerical form
	input = atoi(argv[1]);

	//this part is to make sure that we leave enough space for generation
	//if somebody wants to generate 10k we don't want important numbers ticking
	//over making the card invalid
	//ex if we are supposed to generate 100 000 000 000 discover cards, we don't
	//want our seed to be 6011 9xxx xxxx xxxx because then eventually the 6011 will
	//increment and become 6012, making it an invalid discover card
	temp = input;
	for (places = 0; places < 16; places++)
	{
		temp = temp / 10;
		if (temp < 10)
		{
			break;
		}
	}


	//using the code set earlier, get the correct prefix on the number
	//and generate the seed number
	switch (code)
	{
	case VA:
		cardNum[0] = 4;
		for (int i = 1; i < 15; i++)
		{
			cardNum[i] = rand() % 9;
		}
		cardNum[15 - (places - 1)] = rand() % (10 - temp);
		break;

	case MC1:
		cardNum[0] = 5;
		cardNum[1] = rand() % 5 + 1;
		for (int i = 2; i < 15; i++)
		{
			cardNum[i] = rand() % 9;
		}
		cardNum[15 - places - 1] = rand() % (10 - temp);
		break;
	case DY:
		cardNum[0] = 6;
		cardNum[1] = 0;
		cardNum[2] = 1;
		cardNum[3] = 1;
		for (int i = 4; i < 15; i++)
		{
			cardNum[i] = rand() % 9;
		}
		cardNum[15 - places - 1] = rand() % (10 - temp);
		break;
	case AX1:
		cardNum[0] = 3;
		(rand() % 1 == 0 ? cardNum[1] = 5 : cardNum[1] = 7);
		for (int i = 2; i < 15; i++)
		{
			cardNum[i] = rand() % 9;
		}
		cardNum[15 - places - 1] = rand() % (10 - temp);
		break;
	}


	//get the output filename ready
	filename = std::to_string(input);
	//append the correct code onto is
	switch (code)
	{
	case VA:
		filename.append("_visa.txt");
		break;
	case DY:
		filename.append("_discover.txt");
		break;
	case AX1:
		filename.append("_amex.txt");
		break;
	case MC1:
		filename.append("_mastercard.txt");
		break;
	}
	//open the file for i/o
	output.open(filename);

	//used for keeping a master list of generated numbers
	//deprecated
	/*cards = (int**) calloc(input*16,sizeof(int));
	for(int i = 0; i < input; i++)
	{
		cards[i] = (int*) calloc(16,sizeof(int));
	}*/

	//cout << "Code = " << code << endl;
	//for every number that we want to generate
	for (int genAmnt = 0; genAmnt < input; genAmnt++)
	{
		progressUpdater((int)floor(((double)genAmnt / (double)input) * 10));
		//set our card sum and our final number to zero for initialization
		sum = 0;
		finalNum = 0;

		//deep copy the card number so we can do math with it
		for (int i = 0; i < 15; i++)
		{
			cardNum2[i] = cardNum[i];
		}

		//do the aforementioned math with the deep copy
		for (int i = 0; i < 15; i = i + 2)
		{
			(cardNum2[i] * 2 >= 10 ? cardNum2[i] = (cardNum2[i] * 2) - 9 : cardNum2[i] = cardNum2[i] * 2);
		}

		//find the sum of the first 15 numbers of the newly calculated card number
		for (int i = 0; i < 15; i++)
		{
			sum = sum + cardNum2[i];
		}

		//the final number is whatever will make the sum%10 = 0
		//so we divide the sum by 10 to get a decimal between 0 and 14.4
		//ceiling the decimal number to find what the sum needs to be
		//multiply by ten to get the actual number
		//and subtract the original sum from this new number to get
		//what our final digit must be
		//ex sum is 57
		//          57/10 = 5.7 
		//          ceil(5.7) = 60
		//          60 - 57 = 3
		//          3 is our final digit
		finalNum = ((int)((ceil(((double)sum) / 10)) * 10)) - sum;
		cardNum[15] = finalNum;


		//write the number to the output
		for (int i = 0; i < 16; i++)
		{
			output << cardNum[i];
			//cout << cardNum[i];
		}

		//end the current line we are on
		output << endl;
		//cout << endl;

		//used for keeping the master list of card numbers
		//deprecated
		/*
		for(int i = 0; i < 15;i++)
		{
			cards[genAmnt][i] = cardNum[i];
		}
		*/

		//increment the second to last digit
		cardNum[14]++;

		//make sure that we don't have any 10s in the card number
		//if we do update the next position over accordingly
		for (int i = 15; i >= 0; i--)
		{
			if (cardNum[i] >= 10)
			{
				cardNum[i - 1]++;
				cardNum[i] = 0;
			}
		}
	}

	progressUpdater(10);
	//check to make sure that the numbers are all valid
	//luhnChecker(filename);

	cout << endl;
	cout << "DONE" << endl;
	cout << "Generated " << input << " numbers." << endl;
	cout << "All of the numbers were placed in " << filename << endl;
	duration = (clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "Elapsed time: " << setprecision(2) << duration << "s" << endl;
	return 0;
}

//function to display progress bar and information
void progressUpdater(int part)
{
	if (part > currentPart)
	{
		currentPart = part;
		string blip = "o";
		string bar = "";
		int progress = part * 2;
		int i;
		for (i = 0; i < progress; i++)
		{
			bar = bar.append(blip);
		}
		for (; i < 20; i++)
		{
			bar = bar.append(" ");
		}

		cout << "|" << right << "Generating Credit Card Numbers" << left << "[" << left << bar << left << "]|\r";
		fflush(stdout);
		return;
	}
}

//performs the validation check on all of the credit card numbers
//deprecated
//not necessary now as they are generated in such a way that they
//will always be valid
/*
int luhnChecker(string filename) {

	ifstream input;
	input.open(filename.c_str());
	string line = "TEST";
	char c;
	int cardNum[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int invalidcounter = 0;
	int sum= 0;
	int counter = 0;
	int validcounter = 0;
	char* cline;

	while(line != "")
	{
		sum=0;
	getline(input,line);
	cline = (char*) line.c_str();

	for(int i  = 0; i < 16; i++)
	{
		c = cline[i];
		cardNum[i] = c-'0';
	}

	for(int i = 0; i < 15; i=i+2)
	{
		(cardNum[i]*2 >= 10 ? cardNum[i] = cardNum[i]*2-9 : cardNum[i] = cardNum[i]*2);

	}
	sum = 0;
	for(int i = 0; i < 16; i++)
	{
		sum = sum+cardNum[i];
	}
		if(sum%10 == 0)
		{
			validcounter++;
		}
		else
		{
			invalidcounter++;
		}
		counter++;
	}

	//cout <<"Of " << counter-1 << " numbers generated " << validcounter << " were valid" << endl;

	return 0;
}

//used to check the uniqueness of the numbers generated
//deprecated
/*
bool uniqueChecker(int size, int cardNum[], int** cards)
{
	unsigned long ph = 0;
	unsigned long ph2 = 0;
	unsigned long a;
	unsigned long b;
	unsigned long c;

	if(size == 0)
	{
		return true;
	}
	for(int i = 0; i < 16; i++)
	{
		a = cardNum[i];
		b = pow(10, 15-i);
		c = a*b;
		ph = ph +c;
	}



	//cout << "going to compare " << size << " numbers" << endl;
	for(int i = 0; i < size; i++)
	{
		ph2 = 0;
		for(int j = 0; j < 16; j++)
		{
			a = cards[i][j];
			b = pow(10, 15-j);
			c = a*b;
			ph2 = ph2 +c;
		}

		//cout << ph << " == " << ph2 << endl;

		if(ph == ph2)
		{
			return false;
		}
	}
	return true;
}
*/


