#pragma once
#include "const.h"

class LogicSystem;

class MsgNode
{
public:
	MsgNode(short maxlen)
		: _cur(0),_total(maxlen)
	{
		_data = new char[_total + 1];
		_data[_total] = '\0';
	}
	~MsgNode() {
		delete _data;
	}
	void clear() {
		::memset(_data, 0, _total);
		_cur = 0;
	}

public:
	short _cur;
	short _total;
	char* _data;
};

class RecvNode :public MsgNode {
	friend class LogicSystem;
public:
	RecvNode(short msgid, short msglen);

private:
	short _msgid;
};

class SendNode :public MsgNode {
	friend class LogicSystem;
public:
	SendNode(const char* msg,short msgid, short msglen);

private:
	short _msgid;
};
