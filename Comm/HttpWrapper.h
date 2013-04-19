#ifndef HTTPWRAPPER_H
#define HTTPWRAPPER_H

/////////////////////////////////////////////////////////////////////
// HttpWrapper.h -  Wrapping messages with HTTP protocol 1.1       //
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
HTTP header class is a wrapper that used to generate HTTP protocol
header based on raw message.  It does nothing more but setting data
length, computing how many data to download in this particular request.
It should support HTTP range fetch and keep-alive feature.

Public Interface:
=================
HttpWrapper w;
bool ka = w.keepAlive();	// keep alive
std::string type = w.contentType();	// return current type
bool isBin = w.isContentBinary();	// return current mimeType
int len = w.contentLength();	// message content length
std::string fname = w.fileName();	// set file name
int rs = w.rangeStart();	// return range start value
int re = w.rangeEnd();	// return range end value
std::string header = w.writeHeader();	// set the header info
bool suc = w.readHeader(header);	// read information from HTTP header
w.unwrap(msg);	// unwrap message, read message info to HTTP header
w.wrap(msg);	// wrap message, fill HTTP header into message info
bool isMsgArrived = w.isAllMsgArrived();	// are all data blocks in a message series arrived
bool isACK = w.isACK();	// is current message a ACK message?
bool newMsg = w.isNewMsg();	// is this a new message?

Build Process:
==============
Required Files:
Message.h

Maintenance History:
====================
- Apr 13, 2013 : initial version

*/

#include <string>
#include "Message.h"

/////////////////////////////////////////////////////////////////////
// HTTP wrapper class
class HttpWrapper {
	// HTTP request and response format here
	static const std::string HEADER_POST;
	// content-type here
	static const std::string TYPE_BIN ;
	static const std::string TYPE_TEXT;
	static const std::string TYPE_ACK;
	// connection type here
	static const std::string CONN_KEEP_ALIVE;
	static const std::string CONN_CLOSE;

	// keep-alive flag
	bool _keepAlive;
	// the MIME type
	std::string _mimeType;
	// current range start number
	int _rangeStart;
	// current range end number
	int _rangeEnd;
	// total data length
	int _contentLength;
	// received / sent block name
	std::string _fileName;
	// read keep-alive status from string
	void keepAlive(const std::string& str) {
		_keepAlive = (str==CONN_KEEP_ALIVE);
	}
public:
	///////////////////////////////////////////////////
	// constructor, by default, it will be close-connection and binary content-type
	HttpWrapper() : 
		_keepAlive(false), 
		_mimeType(TYPE_BIN),
		_rangeStart(0),
		_rangeEnd(0),
		_contentLength(0) {}

	///////////////////////////////////////////////////
	// return current connection status
	bool& keepAlive() {
		return _keepAlive;
	}

	///////////////////////////////////////////////////
	// return current type
	std::string& contentType() {
		return _mimeType;
	}

	///////////////////////////////////////////////////
	// return current mimeType
	bool isContentBinary() {
		return _mimeType==TYPE_BIN;
	}

	///////////////////////////////////////////////////
	// message content length
	int& contentLength() {
		return _contentLength;
	}

	///////////////////////////////////////////////////
	// set file name
	std::string& fileName() {
		return _fileName;
	}

	///////////////////////////////////////////////////
	// return range start value
	int& rangeStart() {
		return _rangeStart;
	}

	///////////////////////////////////////////////////
	// return range end value
	int& rangeEnd() {
		return _rangeEnd;
	}

	///////////////////////////////////////////////////
	// set the header info
	std::string writeHeader() {
		char buff[1024];
		sprintf_s(buff, 1024, HEADER_POST.c_str(), _fileName.c_str(), _mimeType.c_str(), _contentLength, _rangeStart, _rangeEnd, _keepAlive ? CONN_KEEP_ALIVE.c_str() : CONN_CLOSE.c_str());
		std::string header(buff);
		return header;
	}

	///////////////////////////////////////////////////
	// read information from HTTP header
	// return false when data is corrupted
	bool readHeader(const std::string& header) {
		// do the scan
		char _keep_alive[16], _file_name[256], _mime_type[64];
		if (sscanf_s(header.c_str(), HEADER_POST.c_str(), _file_name, 256, _mime_type, 64, &_contentLength, &_rangeStart, &_rangeEnd, _keep_alive, 16) < 6) {
			// something wrong is just happening
			return false;
		}
		_fileName = _file_name;
		_mimeType = _mime_type;
		keepAlive(std::string(_keep_alive));	// change the value of keep-alive
		return true;
	}

	///////////////////////////////////////////////////
	// unwrap message, read message info to HTTP header
	void unwrap(Message& msg) {
		msg.fileName() = isContentBinary() ? _fileName : Message::TYPE_STRING;
		msg.isACK() = isACK();
	}

	///////////////////////////////////////////////////
	// wrap message, fill HTTP header into message info
	void wrap(Message& msg) {
		_fileName = msg.fileName();
		if (_fileName == Message::TYPE_STRING)
			_mimeType = TYPE_TEXT;
		else
			_mimeType = TYPE_BIN;
		if (msg.isACK())
			_mimeType = TYPE_ACK;
		_contentLength = msg.length();
	}

	///////////////////////////////////////////////////
	// are all data blocks in a message series arrived
	bool isAllMsgArrived() {
		return _rangeEnd==_contentLength-1;
	}

	///////////////////////////////////////////////////
	// is current message a ACK message?
	bool isACK() {
		return _mimeType==TYPE_ACK;
	}

	///////////////////////////////////////////////////
	// is this a new message?
	bool isNewMsg() {
		return _rangeStart==0;
	}
};

// now init the static constants
const std::string HttpWrapper::CONN_KEEP_ALIVE = "Keep-Alive";
const std::string HttpWrapper::CONN_CLOSE = "close";

// this is a custom-defined HTTP 1.1 header
const std::string HttpWrapper::HEADER_POST =
	"POST %s HTTP/1.1 ; Content-Type: %s , Content-Length: %d , Range: %d-%d , Connection: %s \r\n";	// filename, content-type, content-length, file-range

const std::string HttpWrapper::TYPE_BIN = "application/octet-stream";	// binary content-type
const std::string HttpWrapper::TYPE_TEXT = "plain/text";	// text content-type
const std::string HttpWrapper::TYPE_ACK = "etc/ack";	// custom defined type : ACK

#endif