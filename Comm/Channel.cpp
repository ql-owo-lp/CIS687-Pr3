/////////////////////////////////////////////////////////////////////
// Channel.cpp -  Test Channel class                               //
// ver 1.0                                                         //
// Language:      Visual C++, 2011                                 //
// Platform:      Studio 1558, Windows 7 Pro SP1                   //
// Application:   CIS 687 / Project 3, Sp13                        //
// Author:        Kevin Wang, Syracuse University                  //
//                xwang166@syr.edu                                 //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_CHANNEL

#include "Channel.h"
#include "Messenger.h"
#include <string>
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////
// a receiver helper
class ReceiverHelperThread : public threadBase {
	Channel& ch;
	void run() {
		Messenger mess(ch);	// use default messenger here
		ch.listen<Messenger>(mess);
	}
public:
	ReceiverHelperThread(Channel& _ch) : ch(_ch) {}
};

///////////////////////////////////////////////////
// a sender helper
class SenderHelperThread : public threadBase {
	Channel& ch;
	Message& msg;
	void run() {
		ch.send(msg);
	}
public:
	SenderHelperThread(Channel& _ch, Message& _msg) : ch(_ch), msg(_msg) {}
};

//----< test stub >--------------------------------------------
void main() {
	try
	{
		// this demo will run three senders concurrently, two send file to one same port
		// another sender sends a string to a different port

		ReceiverHelperThread* r[2];
		SenderHelperThread* s[2];
		Channel* ch[2];
		Message m[2];
		Peer* p[2];

		p[0] = new Peer(8080, "127.0.0.1", 8081);
		p[1] = new Peer(8081, "127.0.0.1", 8080);

		m[0].fromFile("../run.bat");
		m[1].fromString("Task#1");

		// initialize all channels
		for (size_t i=0; i<2; i++) {
			std::ostringstream os;
			os << "CH" << i;
			ch[i] = new Channel(os.str(), *p[i]);
			r[i] = new ReceiverHelperThread(*ch[i]);
			s[i] = new SenderHelperThread(*ch[i], m[i]);
			s[i]->start();
			r[i]->start();
		}

		// now start all task concurrently
		for (size_t i=0;i<2;i++) {
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

#endif