#ifndef _JMSG_TYPE_H
#define _JMSG_TYPE_H
#include <string>
#include <vector>
#include "jmsg_field.h"

class JMsgType
{
public:
    JMsgType();
    ~JMsgType();

    JMsgField* getFieldById(int fieldId);
    JMsgField* getFieldByName(const std::string& fieldName);

public:
    int m_id;
    std::string m_typeName;
    std::vector<JMsgField*> m_vecFields;
};

#endif // !_JMSG_TYPE_H
