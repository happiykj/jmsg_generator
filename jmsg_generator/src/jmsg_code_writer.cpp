#include "jmsg_code_writer.h"
#include <stdarg.h>
#include <stdio.h>
#define TABSIZE 4

JMSGCodeWriter::JMSGCodeWriter()
    : m_file(nullptr)
    , m_indent(0)
{
}

JMSGCodeWriter::~JMSGCodeWriter()
{
    m_indent = 0;
    if (m_file) {
        fclose((FILE*)m_file);
        m_file = nullptr;
    }
}

bool JMSGCodeWriter::open(const char* filepath)
{
    m_file = fopen(filepath, "w");
    return m_file ? true : false;
}

void JMSGCodeWriter::addIndent()
{
    m_indent++;
}

void JMSGCodeWriter::removeIndent()
{
    if (m_indent > 0) {
        m_indent--;
    }
}

void JMSGCodeWriter::writeLine(const char* format,...)
{
    for (size_t i = 0; i < m_indent * TABSIZE; i++) {
        fprintf((FILE*)m_file, " ");
    }

    va_list ap;
    va_start(ap, format);
    vfprintf((FILE*)m_file, format, ap);
    va_end(ap);

    fprintf((FILE*)m_file, "\n");
    fflush((FILE*)m_file);
}

void JMSGCodeWriter::write(const char* format,...)
{
    for (size_t i = 0; i < m_indent * TABSIZE; i++) {
        fprintf((FILE*)m_file, " ");
    }

    va_list ap;
    va_start(ap, format);
    vfprintf((FILE*)m_file, format, ap);
    va_end(ap);

    fflush((FILE*)m_file);
}
