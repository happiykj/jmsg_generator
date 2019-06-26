#include <unordered_map>
#include <unordered_set>
#include "jmsg_idl_parse.h"
#include "jmsg_type.h"
#include "jmsg_field.h"
#include "jmsg_util.h"
#include <stdio.h>

static void jMsgOrderTypes(std::unordered_map<std::string, JMsgType*>& mapMessages, std::vector<JMsgType*>& vecMessages);

static char* skipEmptyChars(char* data)
{
    while (jMsgIsEmptyChar(*data)) {
        data ++;
    }
    return data;
}

static char* skipComment(char* data)
{
    while (data && *data == ';') {
        data ++;
        while (!jMsgIsChangeLine(*data)) {
            data ++;
        }

        if (data) {
            data = skipEmptyChars(data);
        }
    }
    return data;
}

static char* getLeftBrace(char* data)
{
    if (*data == '{') {
        data++;
        return data;
    }
    return nullptr;
}

static char* getColons(char* data)
{
    if (*data == ':') {
        data++;
        return data;
    }
    return nullptr;
}

static char* getCommonWord(char* data, std::string& word)
{
    if (!jMsgIsAlpha(*data) && !jMsgIsUnderLine(*data)) {
        return nullptr;
    }

    word.append(data, 1);
    data++;
    while (jMsgIsAlpha(*data) || jMsgIsDigit(*data) || jMsgIsUnderLine(*data)) {
        word.append(data, 1);
        data++;
    }
    return data;
}

static char* getEqual(char* data)
{
    if (*data == '=') {
        data++;
        return data;
    }
    return nullptr;
}

static char* getSquareBrackets(char* data)
{
    if (data[0] == '[' && data[1] == ']') {
        data += 2;
        return data;
    }
    return nullptr;
}

static char* getNumber(char* data, int* number)
{
    int ret = 0;
    bool found = false;
    while (jMsgIsDigit(*data)) {
        ret = ret * 10 + (*data) - '0';
        found = true;
        data++;
    }

    if (found) {
        *number = ret;
        return data;
    }
    return nullptr;
}

static char* getField(char* data, JMsgField** pField)
{
    std::string fieldName;
    std::string fieldType;
    int fieldId = 0;
    JMsgField* field = nullptr;
    bool isArray = false;

    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    data = getCommonWord(data, fieldName);
    if(!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = getColons(data);
    if(!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    char* squireBranceStart = getSquareBrackets(data);
    if (squireBranceStart) {
        isArray = true;
        data = squireBranceStart;
    }

    data = getCommonWord(data, fieldType);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = getEqual(data);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = getNumber(data, &fieldId);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    field = new JMsgField;
    field->m_name = fieldName;
    field->m_type = fieldType;
    field->m_isArray = isArray;
    field->m_id = fieldId;
    *pField = field;

    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    return data;
}

static char* getType(char* data, JMsgType** ppMsgType)
{
    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    std::string typeName;
    data = getCommonWord(data, typeName);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    data = getEqual(data);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    int id;
    data = getNumber(data, &id);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = getLeftBrace(data);
    if (!data) {
        return nullptr;
    }

    data = skipEmptyChars(data);
    if (!data) {
        return nullptr;
    }

    data = skipComment(data);
    if (!data) {
        return nullptr;
    }

    JMsgType* msgType = new JMsgType;
    msgType->m_typeName = typeName;
    msgType->m_id = id;
    while(*data != '}') {
        JMsgField* field = nullptr;
        data = getField(data, &field);
        if (!data) {
            break;
        }
        msgType->m_vecFields.emplace_back(field);
    }

    if (!data) {
        delete msgType;
        return nullptr;
    }

    data++;
    *ppMsgType = msgType;
    return data;
}

static bool isBasicType(const std::string& fieldName)
{
    if (fieldName == "string" ||
        fieldName == "wstring" ||
        fieldName == "bool" ||
        fieldName == "int" ||
        fieldName == "int64" ||
        fieldName == "double") {
        return true;
    }
    return false;
}

static bool checkMessages(std::vector<JMsgType*>& vecMessages)
{
    std::unordered_set<int> setTypeIds;
    std::unordered_map<std::string, JMsgType*> mapTypeNames;
    for (JMsgType* msgType : vecMessages) {
        if (setTypeIds.find(msgType->m_id) != setTypeIds.end()) {
            printf("%d is dup!\n", msgType->m_id);
            return false;
        }

        if (mapTypeNames.find(msgType->m_typeName) != mapTypeNames.end()) {
            printf("%s is dup!\n", msgType->m_typeName.c_str());
            return false;
        }

        mapTypeNames[msgType->m_typeName] = msgType;
        setTypeIds.insert(msgType->m_id);
    }

    for (JMsgType* msgType : vecMessages) {
        std::unordered_set<int> setFieldIds;
        for (JMsgField* field : msgType->m_vecFields) {
            if (setFieldIds.find(field->m_id) != setFieldIds.end()) {
                printf("%d is field dup!\n", field->m_id);
                return false;
            }

            if (!isBasicType(field->m_type)) {
                std::unordered_map<std::string, JMsgType*>::iterator iter = mapTypeNames.find(field->m_type);
                if (iter == mapTypeNames.end()) {
                    printf("%s is field dup!\n", field->m_type.c_str());
                    return false;
                }

                field->m_typeId = iter->second->m_id;
            }

            setFieldIds.insert(field->m_id);
        }
    }

    jMsgOrderTypes(mapTypeNames, vecMessages);
    return true;
}

static bool jMsgTypeNameInVector(std::string& typeName, std::vector<JMsgType*>& vecMessages)
{
    for (JMsgType* msgType : vecMessages) {
        if (msgType->m_typeName == typeName) {
            return true;
        }
    }
    return false;
}

static void jMsgAddTypeToVector(JMsgType* msgType, std::unordered_map<std::string, JMsgType*>& mapMessages, std::vector<JMsgType*>& vecMessages)
{
    if (jMsgTypeNameInVector(msgType->m_typeName, vecMessages)) {
        return;
    }

    for (JMsgField* field : msgType->m_vecFields) {
        if (!isBasicType(field->m_type)) {
            jMsgAddTypeToVector(mapMessages[field->m_type], mapMessages, vecMessages);
        }
    }

    vecMessages.emplace_back(msgType);
}

static void jMsgOrderTypes(std::unordered_map<std::string, JMsgType*>& mapMessages, std::vector<JMsgType*>& vecMessages)
{
    vecMessages.clear();
    for (std::unordered_map<std::string, JMsgType*>::iterator iter = mapMessages.begin(); iter != mapMessages.end(); iter++) {
        jMsgAddTypeToVector(iter->second, mapMessages, vecMessages);
    }
}

bool jMsgIDLParse(const std::string& strData, std::vector<JMsgType*>& vecMessages)
{
    char* data = (char*)strData.c_str();

    do {
        data = skipEmptyChars(data);
        if (!data) {
            break;
        }

        data = skipComment(data);
        if (!data) {
            break;
        }

        JMsgType* msgType = nullptr;
        data = getType(data, &msgType);
        if (!data) {
            break;
        }

        vecMessages.emplace_back(msgType);

        data = skipEmptyChars(data);
        if (!data) {
            break;
        }
    } while (*data != '\0');

    if (!data) {
        return false;
    }

    return checkMessages(vecMessages);
}
