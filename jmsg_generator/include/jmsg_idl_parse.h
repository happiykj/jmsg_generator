#ifndef _JMSG_PARSE_H
#define _JMSG_PARSE_H
#include <vector>
#include <string>

class JMsgType;
bool jMsgIDLParse(const std::string& data, std::vector<JMsgType*>& vecMessages);

#endif // !_JMSG_PARSE_H
