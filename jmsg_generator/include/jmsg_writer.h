#ifndef _JMSG_WRITER_H
#define _JMSG_WRITER_H
#include <string>

class JMsgField;
class JMsgWriter
{
public:
    void writeFieldHeader(JMsgField* field);
    void writeFieldHeader(int fieldId);
    void writeArrayHeader(JMsgField* field, int length);
    void writeStringField(JMsgField* field, const std::string& str);
    void writeWStringField(JMsgField* field, const std::wstring& str);
    void writeIntField(JMsgField* field, const int value);
    void writeInt64Field(JMsgField* field, const int64_t value);
    void writeDoubleField(JMsgField* field, const double value);
    void writeBoolField(JMsgField* field, const bool value);

    void writeString(const std::string& str);
    void writeWString(const std::wstring& str);
    void writeInt(const int value);
    void writeInt64(const int64_t value);
    void writeDouble(const double value);
    void writeBool(const bool value);
    char* getBuffer();
    std::string& getBufferStr();
    size_t getBufferLen();
    void appendBuffer(JMsgWriter* temp);
    void writeEncodedLength(size_t length);
private:
    std::string m_buffer;
};

#endif // !_JMSG_WRITER_H
