#ifndef _JMSG_UTIL_H
#define _JMSG_UTIL_H
#include <string>

bool jMsgIsDigit(char c);
bool jMsgIsAlpha(char c);
bool jMsgIsUnderLine(char c);
bool jMsgIsEmptyChar(char c);
bool jMsgIsChangeLine(char c);
int jMsgEcodeSize(int sizeInput, unsigned char* buf);
int jMsgDecodeSize(unsigned char* buf, int* sizeLen, unsigned char* endPtr);
std::string JMsgGetFileNameWithoutExt(const std::string& fileName);
std::string JmsgGetFilePath(const std::string& fileName);
std::string JmsgGetFileName(const std::string& fileName);
std::string JMsgGetFileExt(const std::string& fileName);
const std::string& JMsgBase64Encde( const char* pData,int nDataLen, std::string& strValue);
int JMsgBase64Decode(const char* pData,size_t nDataLen,void* pDecodeData);
std::string JMsgGetFirstCharBiggerCase(const std::string& data);
std::string jMsgGetFormatString(const char* format, ...);
void jMsgAppendFormatString(std::string&, const char* format, ...);

#endif // !_JMSG_UTIL_H
