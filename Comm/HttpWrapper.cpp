/////////////////////////////////////////////////////////////////////
// HttpWrapper.cpp - Test HttpWrapper class                        //
// ver 1.0                                                         //
// Language:      Visual C++, 2011                                 //
// Platform:      Studio 1558, Windows 7 Pro SP1                   //
// Application:   CIS 687 / Project 3, Sp13                        //
// Author:        Kevin Wang, Syracuse University                  //
//                xwang166@syr.edu                                 //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_HTTPWRAPPER

#include "Message.h"
#include "HttpWrapper.h"

//----< test stub >--------------------------------------------
void main() {
	Message m;
	m.fromString("Test string");	// make a string message
	HttpWrapper wrapper;
	wrapper.wrap(m);
	std::string header = wrapper.writeHeader();
	std::cout<<"\n\n Header: \n"<< header ;
	header = "POST <string> HTTP/1.1 ; Content-Type: plain/text , Content-Length: 100 , Range: 0-99 , Connection: close ";
	// read header info
	if (wrapper.readHeader(header)) {
		std::cout<<"\n Wrapper info:"
			<<"\n File Name: "<< wrapper.fileName()
			<<"\n Content-Type: "<< wrapper.contentType()
			<<"\n Content-Length: "<< wrapper.contentLength()
			<<"\n Range: "<< wrapper.rangeStart()<<" - "<< wrapper.rangeEnd()
			<<"\n Connection: "<< (wrapper.keepAlive() ? "Keep-Alive" : "close")
			;
	}
	else {
		std::cout<<"\n Parse error.";
	}
	std::cout<<"\n\n";
}
#endif