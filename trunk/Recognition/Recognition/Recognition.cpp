// Recognition.cpp : main project file.
// Recognition.cpp : Defines the entry point for the console application.
//

#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <System.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;


#include <iostream>
#include <stdio.h>
#include <conio.h>

#include "Recognizer.h"

int main(int argc, char* argv[])
{
	int a = 0;

	//create recognizer
	Recognizer* recog = new Recognizer(); 
	Eye* eye1 = new Eye(false, 150, 151, 30, 100, 101, 100, 99, 50, 10);
	Eye* eye2 = new Eye(false, 150, 151, 30, 100, 101, 100, 99, 50, 10);
	while(true)
	{
		recog->updateState(*eye1, *eye2);
		for(int i=0; i < recog->MAF_LENGTH;i++)
		{
			printf(" [(%d,%d),", recog->currentEyePositionLX[i], recog->currentEyePositionLY[i]);
			printf("%d,%d)]\n", recog->currentEyePositionRX[i], recog->currentEyePositionRY[i]);
		}
		printf("\n");
		std::cin >> a;
		
		delete eye1;
		delete eye2;
		switch(a)
		{
		case 1://left
			eye1 = new Eye(false, 110, 111, 30, 100, 101, 100, 99, 40, 10);
			eye2 = new Eye(false, 110, 111, 30, 100, 101, 100, 99, 40, 10);
			break;
		case 2://center
			eye1 = new Eye(false, 150, 151, 30, 100, 101, 100, 99, 50, 10);
			eye2 = new Eye(false, 150, 151, 30, 100, 101, 100, 99, 50, 10);
			break;
		case 3://right
			eye1 = new Eye(false, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			eye2 = new Eye(false, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			break;
		case 4://blink right
			eye1 = new Eye(false, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			eye2 = new Eye(true, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			break;
		case 5://blink left
			eye1 = new Eye(true, 190, 191, 30, 100, 101, 100, 99, 10, 10);
			eye2 = new Eye(false, 190, 191, 30, 100, 101, 100, 99, 10, 10);
			break;
		case 6://blink both
			eye1 = new Eye(true, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			eye2 = new Eye(true, 190, 191, 30, 100, 101, 100, 99, 40, 10);
			break;
		}
		if(a>9)
			break;

	}
	//example of how to send a key
    System::Windows::Forms::SendKeys::SendWait("m");

	delete recog;
	//fake some data
	//update recognizer with it
	//print outputs
	return 0;
}

