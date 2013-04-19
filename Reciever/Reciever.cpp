/////////////////////////////////////////////////////////////////////
// Receiver.h  -  Mainly demonstrate the receive function          //
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

//----< program entry >--------------------------------------------
void main() {
	try	{
		// this demo will start two channels, one for receive files from 
		// sender concurrently, one for receive instruction and return the 
		// answer as string

		ReceiverHelperThread* r[2];
		Channel* ch[2];
		Peer* p[2];

		p[0] = new Peer(8081, "127.0.0.1", 8080);
		p[1] = new Peer(8083, "127.0.0.1", 8082);

		// initialize all channels
		for (size_t i=0; i<2; i++) {
			std::ostringstream os;
			os << "CH" << i;
			ch[i] = new Channel(os.str(), *p[i]);
			r[i] = new ReceiverHelperThread(*ch[i]);
			r[i]->start();
		}

		// start two listen processes concurrently
		for (size_t i=0;i<2;i++) {
			r[i]->join();
		}
	}
	catch(...) {
		sout << "\n  something bad happened";
	}
	sout << "\n\n";
	system("pause");
}
