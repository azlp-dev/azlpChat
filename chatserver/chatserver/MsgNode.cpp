#include "MsgNode.h"

RecvNode::RecvNode(short msgid, short msglen)
	:MsgNode(msglen),_msgid(msgid)
{

}

SendNode::SendNode(const char* msg,short msgid, short msglen)
	:MsgNode(msglen+HEAD_TOTAL_LEN),_msgid(msgid)
{
	//函数的返回值为unsigned类型，这里如果id>=则可以正常使用
	auto id = boost::asio::detail::socket_ops::host_to_network_short(msgid);
	::memcpy(_data, &id, HEAD_ID_LEN);
	auto len = boost::asio::detail::socket_ops::host_to_network_short(msglen);
	::memcpy(_data + HEAD_ID_LEN, &len, HEAD_DATA_LEN);
	::memcpy(_data + HEAD_TOTAL_LEN, msg, msglen);
}
