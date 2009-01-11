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

	//example of how to send a key
    System::Windows::Forms::SendKeys::SendWait("e");
	std::cin >> a;

	//fake some data
	//update recognizer with it
	//print outputs
	return 0;
}

