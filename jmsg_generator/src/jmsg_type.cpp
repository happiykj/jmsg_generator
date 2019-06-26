#include "jmsg_type.h"

JMsgType::JMsgType()
    : m_id(0)
{
}

JMsgType::~JMsgType()
{
    for (JMsgField* field : m_vecFields) {
        delete field;
    }
    m_vecFields.clear();
}

JMsgField* JMsgType::getFieldById(int fieldId)
{
    for (JMsgField* field : m_vecFields) {
        if (field->m_id == fieldId) {
            return field;
        }
    }
    return nullptr;
}

JMsgField* JMsgType::getFieldByName(const std::string& fieldName)
{
    for (JMsgField* field : m_vecFields) {
        if(field->m_name == fieldName) {
            return field;
        }
    }
    return nullptr;
}
