/*
 * $Id: vec.cpp,v 1.3 2005/05/10 02:32:50 woods Exp $
 */

static char id[] = "$Id: vec.cpp,v 1.3 2005/05/10 02:32:50 woods Exp $";

#include "vec.h"

vec<char>::vec<char>(const char *str)
{
    m_size = 0;
    m_len = 0;
    m_elem = NULL;

    size_t len = strlen(str);
    wide(len + 1);
    for (iterator i = 0; i < len + 1; i++) {
        m_elem[i] = str[i];
    }
    m_len = len;
}

vec<char>& vec<char>::operator=(const char *str)
{
    size_t len = strlen(str);
    wide(len + 1);
    for (iterator i = 0; i < len + 1; i++) {
        m_elem[i] = str[i];
    }
    m_len = len;
    return *this;
}

vec<char>& vec<char>::operator+=(const char *str)
{
    size_t len = strlen(str);
    wide(m_len + len + 1);
    for (size_t i = 0; i < len + 1; i++) {
        m_elem[m_len + i] = str[i];
    }
    m_len += len;
    return *this;
}

char* vec<char>::c_str()
{
    wide(m_len + 1);
    m_elem[m_len] = 0;

    return m_elem;
}
