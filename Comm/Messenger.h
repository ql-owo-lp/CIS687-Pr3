#ifndef MESSENGER_H
#define MESSENGER_H

/////////////////////////////////////////////////////////////////////
// Messenger.h -  Process received message                         //
// ver 1.0                                                         //
// Language:      Visual C++, 2011                                 //
// Platform:      Studio 1558, Windows 7 Pro SP1                   //
// Application:   CIS 687 / Project 3, Sp13                        //
// Author:        Kevin Wang, Syracuse University                  //
//                xwang166@syr.edu                                 //
/////////////////////////////////////////////////////////////////////
/*

Module Operations:
==================
Messenger is responsible for operating received Message.  When a string
message is received, it will simply print it on the screen.  When a binary
message is received, it will save it on the disk.  It will also conduct
tasks based on specific string instructions.

Public Interface:
=================
Messenger m(channel);	// declare a messenger instance
m(message);	// process received message

Build Process:
==============
Required Files:
- Message.h

Maintenance History:
====================
- Apr 16, 2013 : initial version

*/

#include <string>
#include <fstream>
#include <sstream>
#include "Message.h"

/////////////////////////////////////////////////////////////////////
// Messenger class, used to processing message
class Messenger {
	Channel& ch;	// bind current channel

	///////////////////////////////////////////////////
	// save message content to binary file
	std::string saveBinary(Message& m) {
		::CreateDirectory(L"ReceivedFiles", NULL);	// save files to specific directory
		std::string path("ReceivedFiles/"+ m.fileName());
		std::ofstream f(path, std::ios::out | std::ios::binary);
		m.to(f);
		f.close();
		return path;
	}

	///////////////////////////////////////////////////
	// print string on the screen
	std::string saveString(Message& m) {
		std::ostringstream s;
		m.to(s);
		return s.str();
	}

	///////////////////////////////////////////////////
	// conduct specific task based on message content
	void conductTask(const std::string& instr) {
		// we have two types of instructions here
		if (instr=="Task#1") {
			ch.log("Instruction received! Calculation the ultimate answer to the universe..");
			::Sleep(3000);	// just sleep for 3 sec
			ch.log("Calculation finished!");
			// send a message back
			Message m;
			m.fromString("Task#1 Answer: The ultimate answer to the universe is 42");	// return the ultimate answer to the universe
			ch.send(m);
		}
		else if (instr=="Task#2") {
			ch.log("Instruction received! Calculation the ultimate question to the universe..");
			::Sleep(3000);
			ch.log("Calculation finished!");
			Message m;
			m.fromString("Task#2 Answer: The ultimate question to the universe is 'How many roads must a man walk down?'");	// return the final question to the universe
			ch.send(m);
		}
	}

public:
	///////////////////////////////////////////////////
	// constructor
	Messenger(Channel& _ch) : ch(_ch) {}

	///////////////////////////////////////////////////
	// operate the message
	void operator() (Message& m) {
		if (m.isACK()) {
			if (m.isBinary())
				ch.log("Binary message ["+ m.fileName() +"] is acknowledged by receiver!");
			else
				ch.log("String message is acknowledged by receiver!");
		}
		else if (m.isBinary()) {
			std::string path = saveBinary(m);
			ch.log("File ["+ m.fileName() +"] is received and saved to ["+ path +"]!");
		}
		else {
			std::string str = saveString(m);
			ch.log("String ["+ str +"] is received!");
			conductTask(str);	// see if there is any work to do
		}
	}
};

#endif