/////////////////////////////////////////////////////////////////////
// Message.cpp -  Test Message class                               //
// ver 1.0                                                         //
// Language:      Visual C++, 2011                                 //
// Platform:      Studio 1558, Windows 7 Pro SP1                   //
// Application:   CIS 687 / Project 3, Sp13                        //
// Author:        Kevin Wang, Syracuse University                  //
//                xwang166@syr.edu                                 //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_MESSAGE

#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include "Message.h"

//----< test stub >--------------------------------------------
void main() {
	Message m;
	std::cout<<"\n Reading from run.bat..";
	// test binary message here
	m.fromFile("../run.bat");
	std::cout<<"\n Message length: "<< m.length()
		<<"\n Block size: "<< m.blockSize()
		<<"\n Block number: "<< m.size()
		<<"\n Message name: "<< m.fileName();
	std::cout<<"\n Writing message to disk..";
	::CreateDirectory(L"ReceivedFiles", NULL);	// save files to specific directory
	std::string path("ReceivedFiles/"+ m.fileName());
	std::ofstream f(path, std::ios::out | std::ios::binary);
	m.to(f);
	f.close();

	// test string message here
	Message m2;
	std::cout<<"\n Testing string message..";
	m2.fromString("Test string message");
	std::cout<<"\n Message length: "<< m.length()
		<<"\n Block size: "<< m.blockSize()
		<<"\n Block number: "<< m.size()
		<<"\n Message name: "<< m.fileName();
	std::ostringstream os;
	m2.to(os);
	std::cout<<"\n Message content: "<< os.str().c_str();
	std::cout<<"\n\n";
}
#endif