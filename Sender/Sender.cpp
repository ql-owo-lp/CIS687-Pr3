
/////////////////////////////////////////////////////////////////////
// Sender.cpp  -  Mainly demonstrate the send function             //
// ver 1.0                                                         //
// Language:      Visual C++, 2011                                 //
// Platform:      Studio 1558, Windows 7 Pro SP1                   //
// Application:   CIS 687 / Project 3, Sp13                        //
// Author:        Kevin Wang, Syracuse University                  //
//                xwang166@syr.edu                                 //
/////////////////////////////////////////////////////////////////////

#include "../Comm/Channel.h"
#include "../Comm/Messenger.h"
#include <string>
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////
// a receiver helper class
class ReceiverHelperThread : public threadBase {
	Channel& ch;
	void run() {
		Messenger mess(ch);	// use default messenger here
		ch.listen<Messenger>(mess);
	}
public:
	///////////////////////////////////////////////////
	// constructor
	ReceiverHelperThread(Channel& _ch) : ch(_ch) {}
};

///////////////////////////////////////////////////
// a sender helper class
class SenderHelperThread : public threadBase {
	Channel& ch;
	Message& msg;
	///////////////////////////////////////////////////
	// main part
	void run() {
		ch.send(msg);
	}
public:
	///////////////////////////////////////////////////
	// constructor
	SenderHelperThread(Channel& _ch, Message& _msg) : ch(_ch), msg(_msg) {}
};

//----< program entry >--------------------------------------------
void main() {
	try
	{
		// this demo will run three senders concurrently, two send file to one same port
		// another sender sends a string to a different port
		::Sleep(1000);	// sleep 1 sec to wait receiver prepared
		sout<< "Send progress will begin in 1 second..";
		ReceiverHelperThread* r[4];
		SenderHelperThread* s[4];
		Channel* ch[4];
		Message m[4];
		Peer* p[4];

		p[0] = p[1] = new Peer(8080, "127.0.0.1", 8081);
		p[2] = p[3] = new Peer(8082, "127.0.0.1", 8083);

		m[0].fromFile("../run.bat");
		m[1].fromFile("../SocketComm.v11.suo");
		m[2].fromString("Task#1");	// send a string, which is a task instruction as well
		m[3].fromString("Task#2");

		// initialize all channels
		for (size_t i=0; i<4; i++) {
			std::ostringstream os;
			os << "CH" << i;
			ch[i] = new Channel(os.str(), *p[i]);
			r[i] = new ReceiverHelperThread(*ch[i]);
			s[i] = new SenderHelperThread(*ch[i], m[i]);
			s[i]->start();
			r[i]->start();
		}

		// now start all task concurrently
		for (size_t i=0;i<4;i++) {
			s[i]->join();
			r[i]->join();
		}
	}
	catch(...) {
		sout << "\n\n  Something bad happened to a Channel";
	}
	sout << "\n\n";
	system("pause");
}
