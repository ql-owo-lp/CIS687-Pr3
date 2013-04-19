#ifndef MESSAGE_H
#define MESSAGE_H
/////////////////////////////////////////////////////////////////////
// Message.h   -  define message unit. Support both string and     //
//                binary file.                                     //
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
This module defines the structure of data blocks, and basic message unit,
which can be used to exchange during communication
Three structures are provided:

DataBlock:
-------------
A baisc unit of chunked message.

Message:
-------
A message which will be used to sent during one communication, it contains
an array of data blocks.  It can be either a file or a string.

Public Interface:
=================
DataBlock data;
char * dat = data.data();
size_t size = data.size();
std::string header = data.header();

Message m;
m.fromString(str);	// load message from string
m.fromFile(const std::string& path);	// load message from file
m.from(_istream);	// initialize message from istream
m.to(_ostream);	// output current blocks to the stream
Message::iterator it = m.begin();	// first iterator
Message::iterator it = m.end();	// last iterator
size_t size = m.size();	// how many blocks we have now
m.push(dat);	// push data block into vector
size_t len = m.length();	// return the total content length
std::string name = m.fileName();	// return current file name
bool isACK = m.isACK();	// return ACK status
bool isBin = m.isBinary();	// is current mssage a binary message?

Build Process:
==============
Required Files:
- none

Maintenance History:
====================
- Apr 16, 2013 : initial version

*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

// define the size of each block
#define BLOCK_SIZE 1024

/////////////////////////////////////////////////////////////////////
// A raw data block
// it is supposed to be a struct, while there is a char * here
class DataBlock {
	// data size
	size_t _size;
	// data content
	char * _data;
	// block header
	std::string _header;
public:
	///////////////////////////////////////////////////
	// constructors
	DataBlock() : _data(0), _size(0) {}
	///////////////////////////////////////////////////
	// constructing from raw data
	DataBlock(const char * _dat, size_t _s) : _size(_s) {
		if (_size<1) return;
		_data = new char[_size];
		// do a string copy here
		for (size_t i=0; i<_size; i++)
			_data[i] = _dat[i];
	}
	///////////////////////////////////////////////////
	// constructing from string
	DataBlock(const std::string _dat) : _size(_dat.length()) {
		if (_size<1) return;
		_data = new char[_size];
		for (size_t i=0; i<_size; i++)
			_data[i] = _dat[i];
	}
	///////////////////////////////////////////////////
	// copy constructor
	DataBlock(const DataBlock& _dat) : _size(_dat._size), _header(_dat._header), _data(_dat._data) {
		if (_size<1) return;
		char* tmp = _data;
		char* _data = new char[_size];
		for (size_t i=0; i<_size; i++)
			_data[i] = tmp[i];
	}

	///////////////////////////////////////////////////
	// return data
	char * data() {
		return _data;
	}

	///////////////////////////////////////////////////
	// return data size
	size_t size() {
		return _size;
	}

	///////////////////////////////////////////////////
	// reture header
	std::string& header() {
		return _header;
	}
};

/////////////////////////////////////////////////////////////////////
// A Message
class Message {
	// data block list
	std::vector<DataBlock> data;
	// data content length in total
	size_t _contentLength;
	// how many bytes in one block by default
	size_t _blockSize;
	// transmitted file name
	std::string _fileName;
	// is this message an acknowledge message?
	bool _isACK;
public:
	// iterator for blocks
	typedef std::vector<DataBlock>::iterator iterator;
	// string message type
	static const std::string TYPE_STRING;

	///////////////////////////////////////////////////
	// constructor
	Message() :
		_contentLength(0), _blockSize(BLOCK_SIZE), _fileName(TYPE_STRING), _isACK(false) {}
	Message(const std::string& f) :
		_contentLength(0), _blockSize(BLOCK_SIZE), _fileName(f), _isACK(false) {}

	///////////////////////////////////////////////////
	// load message from string
	inline void fromString(const std::string& str) {
		_fileName = TYPE_STRING;
		if (str.empty()) return;
		std::stringstream ss;
		ss << str;
		from(ss);
	}

	///////////////////////////////////////////////////
	// load message from file
	void fromFile(const std::string& path) {
		// get file name from path
		size_t pos = path.find_last_of('/');
		if (pos == std::string::npos)
			_fileName = path;
		else
			_fileName = path.substr(pos+1);
		std::ifstream fs(path, std::ios::in | std::ios::binary);
		if (fs.good())
			from(fs);
		fs.close();
	}

	///////////////////////////////////////////////////
	// initialize message from istream
	// as it is stream, it can either be a istringstream or a ifstream
	void from(std::istream& s) {
		_contentLength = 0;
		s.seekg(0, s.beg);
		s.clear(s.goodbit);
		while (s.good()) {
			char * buff = new char[_blockSize];	// read buffer
			size_t bytesRead = 0;
			while (bytesRead<_blockSize) {
				s.get(buff[bytesRead]);
				if (s.good())	// if it is still good
					bytesRead++;
				else
					break;
			}
			// push block
			if (bytesRead>0) {
				data.push_back(DataBlock(buff, bytesRead));
				_contentLength += bytesRead;
			}
			delete buff;
		}
	}

	///////////////////////////////////////////////////
	// output current blocks to the stream
	inline void to(std::ostream& s) {
		for (iterator it=begin(); it!=end(); it++) {
			s.write(it->data(), it->size());
		}
	}

	///////////////////////////////////////////////////
	// first iterator
	inline iterator begin() {
		return data.begin();
	}

	///////////////////////////////////////////////////
	// last iterator
	inline iterator end() {
		return data.end();
	}

	///////////////////////////////////////////////////
	// how many blocks we have now
	inline size_t size() {
		return data.size();
	}

	///////////////////////////////////////////////////
	// return current block size
	inline size_t& blockSize() {
		return _blockSize;
	}

	///////////////////////////////////////////////////
	// push data block into vector
	inline void push(DataBlock dat) {
		data.push_back(dat);
		_contentLength += dat.size();
	}

	///////////////////////////////////////////////////
	// return the total content length
	inline size_t length() {
		return _contentLength;
	}

	///////////////////////////////////////////////////
	// return current file name
	inline std::string& fileName() {
		return _fileName;
	}

	///////////////////////////////////////////////////
	// return ACK status
	inline bool& isACK() {
		return _isACK;
	}

	///////////////////////////////////////////////////
	// is current mssage a binary message?
	inline bool isBinary() {
		return _fileName!=TYPE_STRING;
	}
};

// static const, string message name
const std::string Message::TYPE_STRING = "<string>";

#endif
