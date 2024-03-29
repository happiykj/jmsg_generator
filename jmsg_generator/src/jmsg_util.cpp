#include <stdarg.h>
#include "jmsg_util.h"

bool jMsgIsDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool jMsgIsAlpha(char c)
{
    return c >= 'A' && c <= 'z';
}

bool jMsgIsUnderLine(char c)
{
    return c == '_';
}

bool jMsgIsEmptyChar(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

int jMsgEcodeSize(int sizeInput, unsigned char* buf)
{
    if (sizeInput >= 0 && sizeInput <= 127) {
        *buf = (char)sizeInput;
        return 1;
    }

    if (sizeInput > 127 && sizeInput <= 0x7fffffff) {
        buf[0] = sizeInput >> 24 | 0x80;
        buf[1] = sizeInput >> 16 & 0xff;
        buf[2] = sizeInput >> 8 & 0xff;
        buf[3] = sizeInput & 0xff;
        return 4;
    }

    return -1;
}

int jMsgDecodeSize(unsigned char* buf, int* sizeLen, unsigned char* endPtr)
{
    if (*buf & 0x80) {
        if (endPtr - buf < 4) {
            return -1;
        }
        unsigned char firstChar =  buf[0] ^ 0x80;
        *sizeLen = 4;
        int val1 = firstChar ? ((int) firstChar)  << 24 : 0;
        int val2 = buf[1] ? ((int) buf[1])  << 16 : 0;
        int val3 = buf[2] ? ((int) buf[2])  << 8 : 0;
        return val1 + val2 + val3 +  buf[3];
    }

    if (endPtr - buf < 1) {
        return -1;
    }

    *sizeLen = 1;
    return buf[0];
}

std::string jMsgGetFormatString(const char * format, ...)
{
    std::string res;
    char buf[4096] = {};
    char* backup_buf = nullptr;
    int cur_buf_len = sizeof(buf);
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(buf, cur_buf_len, format, ap);
    va_end(ap);
    while (len >= cur_buf_len) {
        cur_buf_len *= 2;
        if(backup_buf) {
            free(backup_buf);
        }

        backup_buf = (char*)calloc(1, cur_buf_len);
        va_start(ap, format);
        len = vsnprintf(backup_buf, cur_buf_len, format, ap);
        va_end(ap);
    }

    if (backup_buf) {
        res =backup_buf;
        free(backup_buf);
    }
    else {
        res = buf;
    }
    return res;
}

void jMsgAppendFormatString(std::string& data, const char* format, ...)
{
    char buf[4096] = {};
    char* backup_buf = nullptr;
    int cur_buf_len = sizeof(buf);
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(buf, cur_buf_len, format, ap);
    va_end(ap);
    while (len >= cur_buf_len) {
        cur_buf_len *= 2;
        if (backup_buf) {
            free(backup_buf);
        }

        backup_buf = (char*)calloc(1, cur_buf_len);
        va_start(ap, format);
        len = vsnprintf(backup_buf, cur_buf_len, format, ap);
        va_end(ap);
    }

    if (backup_buf) {
        data.append(backup_buf);
        free(backup_buf);
    }
    else {
        data.append(buf);
    }
}

bool jMsgIsChangeLine(char c)
{
    return c == '\n' || c == '\r';
}

std::string JMsgGetFileNameWithoutExt(const std::string& fullfileName)
{
    std::string fileName = JmsgGetFileName(fullfileName);
    size_t pos = fileName.rfind('.');
    if (pos > 0) {
        return fileName.substr(0, pos);
    }
    return fileName;
}

std::string JMsgGetFileExt(const std::string& fullfileName)
{
    std::string fileName = JmsgGetFileName(fullfileName);
    size_t pos = fileName.rfind('.');
    if (pos > 0) {
        return fileName.substr(pos);
    }
    return "";
}

std::string JmsgGetFilePath(const std::string& fileName)
{
    size_t pos1 = fileName.rfind('/');
    size_t pos2 = fileName.rfind('\\');
    size_t pos = pos1 > pos2 ? pos1 : pos2;
    if (pos > 0) {
        return fileName.substr(0, pos + 1);
    }
    return "";
}

std::string JmsgGetFileName(const std::string& fileName)
{
    size_t pos1 = fileName.rfind('/');
    size_t pos2 = fileName.rfind('\\');
    size_t pos = pos1 > pos2 ? pos1 : pos2;
    if (pos > 0) {
        return fileName.substr(pos + 1);
    }
    return fileName;
}

const std::string& JMsgBase64Encde(const char* pData,int nDataLen, std::string& strValue)
{
    static const char cEncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char ch1,ch2;
    char szEncode[4];
    strValue.clear();
    for (int idx = 0; idx < nDataLen / 3; ++idx) {
        ch1 = *pData++;
        ch2 = *pData++;
        szEncode[0] = cEncodeTable[ch1>>2];
        szEncode[1] = cEncodeTable[((ch1<<4)|(ch2>>4))&0x3F];

        ch1= *pData++;
        szEncode[2] = cEncodeTable[((ch2<<2)|(ch1>>6))&0x3f];
        szEncode[3] = cEncodeTable[ch1&0x3f];
        strValue.append(szEncode,sizeof(szEncode));
    }

    int nTail = nDataLen%3;
    if (nTail == 1) {
        ch1=*pData++;
        szEncode[0] = cEncodeTable[(ch1>>2)&0x3f];
        szEncode[1] = cEncodeTable[(ch1<<4)&0x30];
        szEncode[2] = '=';
        szEncode[3] = '=';
        strValue.append(szEncode,sizeof(szEncode));
    } else
    if (nTail == 2) {
        ch1 = *pData++;
        ch2 = *pData++;
        szEncode[0] = cEncodeTable[(ch1>>2)&0x3f];
        szEncode[1] = cEncodeTable[((ch1<<4)&0x30)|((ch2>>4)&0xf)];
        szEncode[2] = cEncodeTable[(ch2<<2)&0x3c];
        szEncode[3] = '=';
        strValue.append(szEncode,sizeof(szEncode));
    }
    return strValue;
}

typedef unsigned char BYTE;
int JMsgBase64Decode(const char* pData,size_t nDataLen,void* pDecodeData)
{
    const unsigned char cDecodeTable[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
        52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 2, 3, 4, 5, 6,7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
        0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,
    };

    if (pData == nullptr || nDataLen % 4) {
        return -1;
    }

    BYTE* pRecv = reinterpret_cast<BYTE*>(pDecodeData);
    BYTE* pStart = pRecv;
    BYTE cTemp[4];

    for (size_t idx = 0; idx < nDataLen; idx += 4) {
        cTemp[0] = cDecodeTable[*(pData + idx)];
        cTemp[1] = cDecodeTable[*(pData + idx + 1)];

        if(*(pData + idx + 2)!='=')
            cTemp[2]= cDecodeTable[*(pData + idx + 2)];
        else
            cTemp[2] = 0;

        if(*(pData + idx + 3)!='=')
            cTemp[3]= cDecodeTable[*(pData + idx + 3)];
        else
            cTemp[3] = 0;

        pRecv[0] = static_cast<char>((cTemp[0]<<2) + (cTemp[1]>>4));
        pRecv[1] = static_cast<char>(((cTemp[1]&0xf)<<4) + (cTemp[2]>>2));
        pRecv[2] = static_cast<char>(cTemp[3] + (cTemp[2]<<6));
        pRecv+=3;
    }

    return (int)(pRecv - pStart);
}

std::string JMsgGetFirstCharBiggerCase(const std::string& data)
{
    if (data.empty()) {
        return data;
    }

    std::string ret = data;
    ret[0] = (char)toupper(ret[0]);
    return ret;
}
