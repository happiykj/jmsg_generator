#ifndef _JMSG_PROTO_H
#define _JMSG_PROTO_H
#include <string>
#include <vector>
#include <unordered_map>
#include "json/value.h"

class JMsgType;
class JMsgWriter;
class JMsgReader;
class JMsgField;
class JMsgProto;
typedef bool (*JMsgProtoDecodeCallback)(JMsgProto* proto, JMsgField* type, JMsgReader* reader, void* args);
typedef bool (*JMsgProtoEncodeCallback)(JMsgProto* proto, JMsgField* type, JMsgWriter* writer, void* args);
typedef bool (*JMsgProtoDecodeJsonCallback)(JMsgProto* proto, JMsgField* type, Json::Value& reader, void* args);
typedef bool (*JMsgProtoEncodeJsonCallback)(JMsgProto* proto, JMsgField* type, Json::Value& writer, void* args);
class JMsgProto
{
public:
    JMsgProto() : m_fixFieldLen(true) {}
    ~JMsgProto();
    static JMsgProto* createProto(const std::string& idlString, bool fixFieldLen = true);
    bool encode(int typeId, JMsgWriter* writer, JMsgProtoEncodeCallback callback, void* args);
    bool encode(const std::string& typeName, JMsgWriter* writer, JMsgProtoEncodeCallback callback, void* args);
    int decode(JMsgReader* reader, JMsgProtoDecodeCallback callback, void* args);
    void toJson(JMsgReader* reader, int len, std::string& result);
    std::vector<JMsgType*>& getAllTypes() { return m_vecTypes; }
    JMsgType* getTypeByName(const std::string& name);
    JMsgType* getTypeById(int id);
    void setFixFieldLen(bool value) { m_fixFieldLen = value; }
    bool getFixFieldLen() { return m_fixFieldLen; }

    bool encodeJson(int typeId,  Json::Value& obj, JMsgProtoEncodeJsonCallback callback, void* args);
    bool decodeJson(const std::string& typeName, Json::Value& obj, JMsgProtoDecodeJsonCallback, void* args);
    bool decodeJson(int typeId, Json::Value& obj, JMsgProtoDecodeJsonCallback, void* args);
private:
    std::vector<JMsgType*> m_vecTypes;
    std::unordered_map<std::string, size_t> m_mapNameToIndex;
    std::unordered_map<int, size_t> m_mapIdToIndex;
    bool m_fixFieldLen;
};

#endif // !_JMSG_PROTO_H
