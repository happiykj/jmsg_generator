#ifndef _JMSG_CODE_WRITER
#define _JMSG_CODE_WRITER

class JMSGCodeWriter
{
public:
    JMSGCodeWriter();
    ~JMSGCodeWriter();
    void addIndent();
    void removeIndent();
    bool open(const char* filepath);
    void writeLine(const char* format...);
    void write(const char* format,...);
private:
    void* m_file;
    size_t m_indent;
};

#endif  // !_JMSG_CODE_WRITER
