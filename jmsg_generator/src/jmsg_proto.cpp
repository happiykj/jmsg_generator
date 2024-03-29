#include "jmsg_proto.h"
#include "jmsg_writer.h"
#include "jmsg_reader.h"
#include "jmsg_type.h"
#include "jmsg_field.h"
#include "jmsg_idl_parse.h"
#include "jmsg_util.h"

JMsgProto::~JMsgProto()
{
    for (JMsgType* msgType : m_vecTypes) {
        delete msgType;
    }
    m_vecTypes.clear();
}

JMsgProto*  JMsgProto::createProto(const std::string& idlString, bool fixFieldLen/* = true*/)
{
    JMsgProto* proto = new JMsgProto;
    proto->m_fixFieldLen = fixFieldLen;
    if (!jMsgIDLParse(idlString, proto->m_vecTypes)) {
        delete proto;
        return nullptr;
    }

    for (size_t idx = 0; idx < proto->m_vecTypes.size(); idx++) {
        proto->m_mapNameToIndex[proto->m_vecTypes[idx]->m_typeName] = idx;
        proto->m_mapIdToIndex[proto->m_vecTypes[idx]->m_id] = idx;
    }
    return proto;
}

bool JMsgProto::encode(int typeId, JMsgWriter* writer, JMsgProtoEncodeCallback callback, void* args)
{
    JMsgType* msgType = getTypeById(typeId);
    if (!msgType) {
        return false;
    }

    writer->writeFieldHeader(msgType->m_id);
    for (JMsgField* field : msgType->m_vecFields) {
        if (getFixFieldLen()) {
            JMsgWriter tempWriter;
            callback(this, field, &tempWriter, args);
            writer->writeEncodedLength(tempWriter.getBufferLen());
            writer->appendBuffer(&tempWriter);
        }
        else {
            callback(this, field, writer, args);
        }
    }

    // fields end with 0
    writer->writeFieldHeader(0);
    return true;
}

bool JMsgProto::encode(const std::string& typeName, JMsgWriter* writer, JMsgProtoEncodeCallback callback, void* args)
{
    JMsgType* msgType = getTypeByName(typeName);
    if(!msgType) {
        return false;
    }

    writer->writeFieldHeader(msgType->m_id);
    for (JMsgField* field : msgType->m_vecFields) {
        if (getFixFieldLen()) {
            JMsgWriter tempWriter;
            callback(this, field, &tempWriter, args);
            writer->writeEncodedLength(tempWriter.getBufferLen());
            writer->appendBuffer(&tempWriter);
        }
        else {
            callback(this, field, writer, args);
        }
    }

    // fields end with 0
    writer->writeFieldHeader(0);
    return true;
}

int JMsgProto::decode(JMsgReader* reader, JMsgProtoDecodeCallback callback, void* args)
{
    bool isSuccess = false;
    int typeId = reader->readFieldId(isSuccess);
    if (!isSuccess) {
        return -1;
    }

    JMsgType* msgType = getTypeById(typeId);
    int fieldId = 0;
    if (!msgType) {
        return -1;
    }

    do {
        if (getFixFieldLen()) {
            int fieldLen = reader->readEncodedLen(isSuccess);
            if (fieldLen == 0) {
                break;
            }

            fieldId = reader->peekMessageTypeId(isSuccess);
            if (!isSuccess) {
                return -1;
            }

            JMsgField* field = msgType->getFieldById(fieldId);
            if (!field) {
                if(reader->isOffsetOutOfRange(fieldLen)) {
                    return -1;
                }
                reader->skipLen(fieldLen);
                continue;
            }

            reader->readFieldId(isSuccess);
            if (!callback(this, field, reader, args)) {
                return -1;
            }
        }
        else {
            fieldId = reader->readFieldId(isSuccess);
            if (fieldId == 0) {
                break;
            }

            JMsgField* field = msgType->getFieldById(fieldId);
            if (!field) {
                return -1;
            }

            if (!callback(this, field, reader, args)) {
                return -1;
            }
        }
    } while(true);
    return typeId;
}

JMsgType* JMsgProto::getTypeByName(const std::string& name)
{
    std::unordered_map<std::string, size_t>::iterator iter = m_mapNameToIndex.find(name);
    if(iter == m_mapNameToIndex.end()) {
        return nullptr;
    }
    return m_vecTypes[iter->second];
}

JMsgType* JMsgProto::getTypeById(int id)
{
    std::unordered_map<int, size_t>::iterator iter = m_mapIdToIndex.find(id);
    if(iter == m_mapIdToIndex.end()) {
        return nullptr;
    }

    return m_vecTypes[iter->second];
}

void JMsgProto::toJson(JMsgReader* reader, int len, std::string& result)
{
    bool isSuccess = false;
    int typeId = reader->readFieldId(isSuccess);
    if (!isSuccess) {
        return;
    }

    JMsgType* msgType = getTypeById(typeId);
    int fieldId = 0;
    if (!msgType) {
        return;
    }
    result.append("{");
    int currentFieldCount = 0;

    do {
        int fieldLen = 0;
        JMsgField* field = nullptr;
        if (getFixFieldLen()) {
            fieldLen = reader->readEncodedLen(isSuccess);
            if (!fieldLen) {
                break;
            }
        }

        if (!isSuccess) {
            return;
        }

        if (getFixFieldLen()) {
            fieldId = reader->peekMessageTypeId(isSuccess);
        }
        else {
            fieldId = reader->readFieldId(isSuccess);
        }

        if (!isSuccess) {
            return;
        }

        field = msgType->getFieldById(fieldId);
        if (getFixFieldLen()) {
            if (!field) {
                reader->skipLen(fieldLen);
                continue;
            }
            reader->readMessageTypeId(isSuccess);
        }
        else {
            if (!field) {
                break;
            }
        }

        if (!isSuccess) {
            return;
        }

        if (currentFieldCount > 0) {
            result.append(",");
        }

        if (!field->m_isArray) {
            if (field->m_type == "int") {
                jMsgAppendFormatString(result, "\"%s\": %d", field->m_name.c_str(), reader->readInt(isSuccess));
            } else
            if (field->m_type == "int64") {
                jMsgAppendFormatString(result, "\"%s\": %lld", field->m_name.c_str(), reader->readInt64(isSuccess));
            } else
            if (field->m_type == "double") {
                jMsgAppendFormatString(result, "\"%s\": %f", field->m_name.c_str(), reader->readDouble(isSuccess));
            } else
            if (field->m_type == "string") {
                jMsgAppendFormatString(result, "\"%s\": \"%s\"", field->m_name.c_str(), reader->readString(isSuccess).c_str());
            } else
            if (field->m_type == "bool") {
                jMsgAppendFormatString(result, "\"%s\": %s", field->m_name.c_str(), reader->readBool(isSuccess) ? "true" : "false");
            }
            else {
                jMsgAppendFormatString(result, "\"%s\": ", field->m_name.c_str());
                toJson(reader, len, result);
            }

            if (!isSuccess) {
                break;
            }
        }
        else {
            int arrayCount = reader->readArrayLength(isSuccess);
            if (!isSuccess) {
                break;
            }

            jMsgAppendFormatString(result, "\"%s\": [", field->m_name.c_str());
            bool arrayItemWritten = false;
            for (int i = 0; i < arrayCount; i++) {
                if (arrayItemWritten) {
                    jMsgAppendFormatString(result, ",");
                }

                if (field->m_type == "int") {
                    jMsgAppendFormatString(result, "%d", reader->readInt(isSuccess));
                } else
                if (field->m_type == "int64") {
                    jMsgAppendFormatString(result, "%lld", reader->readInt64(isSuccess));
                } else
                if (field->m_type == "double") {
                    jMsgAppendFormatString(result, "%f", reader->readDouble(isSuccess));
                } else
                if (field->m_type == "string") {
                    jMsgAppendFormatString(result, "\"%s\"", reader->readString(isSuccess).c_str());
                } else
                if (field->m_type == "bool") {
                    jMsgAppendFormatString(result, " %s", reader->readBool(isSuccess) ? "true" : "false");
                }
                else {
                    toJson(reader, len, result);
                }

                if (!isSuccess) {
                    return;
                }

                arrayItemWritten = true;
            }
            if (!isSuccess) {
                return;
            }

            jMsgAppendFormatString(result, "]");
        }

        currentFieldCount++;
    } while(true);
    result.append("}");
}

bool JMsgProto::encodeJson(int typeId, Json::Value& obj, JMsgProtoEncodeJsonCallback callback, void* args)
{
    JMsgType* msgType = this->getTypeById(typeId);
    if (!msgType) {
        return false;
    }

    for (JMsgField* field : msgType->m_vecFields) {
        if (field) {
            callback(this, field, obj, args);
        }
    }
    return true;
}

bool JMsgProto::decodeJson(const std::string& typeName, Json::Value& obj, JMsgProtoDecodeJsonCallback callback, void* args)
{
    JMsgType* msgType = this->getTypeByName(typeName);
    if (!msgType) {
        return false;
    }

    if (!obj.isObject()) {
        return true;
    }

    for (std::string& key : obj.getMemberNames()) {
        JMsgField* field = msgType->getFieldByName(key);
        if (field) {
            callback(this, field, obj, args);
        }
    }
    return true;
}

bool JMsgProto::decodeJson(int typeId, Json::Value& obj, JMsgProtoDecodeJsonCallback callback, void* args)
{
    JMsgType* msgType = this->getTypeById(typeId);
    if (!msgType) {
        return false;
    }

    if (!obj.isObject()) {
        return true;
    }

    for (std::string& key : obj.getMemberNames()) {
        JMsgField* field = msgType->getFieldByName(key);
        if (field) {
            callback(this, field, obj, args);
        }
    }
    return true;
}
