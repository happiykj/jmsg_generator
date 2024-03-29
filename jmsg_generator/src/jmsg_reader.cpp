#include "jmsg_reader.h"
#include "jmsg_util.h"

JMsgReader::JMsgReader(unsigned char* buf, int len)
{
    m_buf = buf;
    m_len = len;
    m_cur = m_buf;
    m_endPtr = m_buf + len;
}

int JMsgReader::peekMessageTypeId(bool& isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return -1;
    }
    isSuccess = true;
    return size;
}

int JMsgReader::readMessageTypeId(bool& isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return -1;
    }
    isSuccess = true;
    m_cur += sizeLen;
    return size;
}

int JMsgReader::readFieldId(bool& isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return -1;
    }
    isSuccess = true;
    m_cur += sizeLen;
    return size;
}

int JMsgReader::readArrayLength(bool& isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return -1;
    }
    isSuccess = true;
    m_cur += sizeLen;
    return size;
}

int JMsgReader::readInt(bool& isSuccess)
{
    if (m_endPtr - m_cur < 4) {
        isSuccess = false;
        return -1;
    }
    int val1 = m_cur[0] ? ((int)m_cur[0]) << 24 : 0;
    int val2 = m_cur[1] ? ((int) m_cur[1]) << 16 : 0;
    int val3 = m_cur[2] ? ((int)m_cur[2]) << 8 : 0;
    int val4 =  m_cur[3];

    int ret = val1 + val2 + val3+ val4;
    m_cur += 4;
    isSuccess = true;
    return ret;
}

int64_t JMsgReader::readInt64(bool& isSuccess)
{
    if (m_endPtr - m_cur < 8) {
        isSuccess = false;
        return -1;
    }
    int64_t val1 = m_cur[0] ? ((int64_t)m_cur[0]) << 56 : 0;
    int64_t val2 = m_cur[1] ? ((int64_t)m_cur[1]) << 48 : 0;
    int64_t val3 = m_cur[2] ? ((int64_t)m_cur[2]) << 40 : 0;
    int64_t val4 = m_cur[3] ? ((int64_t)m_cur[3]) << 32 : 0;
    int64_t val5 = m_cur[4] ? ((int64_t)m_cur[4]) << 24 : 0;
    int64_t val6 = m_cur[5] ? ((int64_t)m_cur[5]) << 16 : 0;
    int64_t val7 = m_cur[6] ? ((int64_t)m_cur[6]) << 8 : 0;
    int64_t val8 = m_cur[7];
    int64_t ret = val1 + val2 + val3 + val4 + val5 + val6 + val7 + val8;
    m_cur += 8;
    isSuccess = true;
    return ret;
}

bool JMsgReader::readBool(bool& isSuccess)
{
    if (m_endPtr - m_cur < 1) {
        isSuccess = false;
        return false;
    }
    isSuccess = true;
    bool ret = m_cur[0] > 0;
    m_cur += 1;
    return ret;
}

std::string JMsgReader::readString(bool& isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return "";
    }

    m_cur += sizeLen;

    if(m_endPtr - m_cur < size) {
        isSuccess = false;
        return "";
    }
    isSuccess = true;
    std::string ret((char*)m_cur, size);
    m_cur += size;
    return ret;
}

std::wstring JMsgReader::readWString(bool & isSuccess)
{
    int sizeLen = 0;
    int size = jMsgDecodeSize(m_cur, &sizeLen, m_endPtr);
    if (size < 0) {
        isSuccess = false;
        return L"";
    }

    m_cur += sizeLen;

    if (m_endPtr - m_cur < size) {
        isSuccess = false;
        return L"";
    }
    isSuccess = true;
    std::wstring ret((wchar_t*)m_cur, size / 2);
    m_cur += size;
    return ret;
}

double JMsgReader::readDouble(bool& isSuccess)
{
    isSuccess = true;
    double ret = *(double*)m_cur;
    m_cur += sizeof(double);
    return ret;
}

int JMsgReader::readEncodedLen(bool& isSuccess)
{
    return readFieldId(isSuccess);
}

void JMsgReader::skipLen(int len)
{
    m_cur += len;
}

bool JMsgReader::isOffsetOutOfRange(int offset)
{
    return m_cur + offset >= m_endPtr;
}
