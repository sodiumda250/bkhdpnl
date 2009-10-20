/*
 * $Id: vec.h,v 1.3 2005/05/10 03:53:14 woods Exp $
 */

#ifndef VEC_H
#define VEC_H

#include <cstddef>
#include <string.h>

template <class T> class vec {
protected:
const static size_t MIN_SIZE = 32;
    size_t m_size;
    size_t m_len;
    T* m_elem;
public:
    typedef size_t iterator;
protected:
    size_t wide(size_t s) {
        if (m_size >= s) {
            return m_size;
        }
        size_t tsize = ((m_size < MIN_SIZE) ? MIN_SIZE : m_size);
        while (tsize < s) {
            tsize *= 2;
        }
        T* tmp = new T[tsize];
        m_size = tsize;
        if (m_elem != NULL) {
            for (iterator i = 0; i < m_len; i++) {
                tmp[i] = m_elem[i];
            }
            delete[] m_elem;
        }
        m_elem = tmp;

        return m_size;
    };
public:
    vec<T>(size_t s = MIN_SIZE) {
        m_size = 0;
        m_len = 0;
        m_elem = NULL;
        wide(s);
    };
    vec<T>(vec<T>& dst) {
        m_size = 0;
        m_len = 0;
        m_elem = NULL;
        wide(dst.m_size);
        for (iterator i = 0; i < dst.m_len; i++) {
            m_elem[i] = dst.m_elem[i];
        }
        m_len = dst.m_len;
    };

    ~vec<T>() {
        if (m_elem != NULL) {
            delete[] m_elem;
        }
        m_size = 0;
        m_len = 0;
    };

    void init(size_t s = MIN_SIZE) {
        if (m_len > 0) {
            if (m_elem != NULL) {
                delete[] m_elem;
                m_elem = NULL;
            }
            m_size = 0;
            m_len = 0;
        }
        wide(s);
    };

    vec<T>& operator=(vec<T>& dst) {
        if (m_size < dst.m_size) {
            wide(dst.m_size);
        }
        for (iterator i = 0; i < dst.m_len; i++) {
            m_elem[i] = dst.m_elem[i];
        }
        m_len = dst.m_len;
        return *this;
    };

    inline iterator begin() {return 0;};
    inline iterator end() {return m_len;};

    inline size_t length() {return m_len;};

    inline size_t setsize(size_t s) {wide(s); m_len = s; return s;};

    inline T& operator[](iterator index) {return m_elem[index];};

    inline size_t add(T& dst) {
        wide(m_len + 1);
        m_elem[m_len++] = dst;
        return m_len;
    };

    bool del(T& dst, iterator first = 0, bool deleted = false) {
        bool isdel = false;

        for (iterator i = first; i < end(); i++) {
            if (isdel) {
                m_elem[i - 1] = m_elem[i];
            } else if (dst == m_elem[i]) {
                isdel = true;
                first = i;
            }
        }

        if (isdel) {
            m_len--;
            return del(dst, first, isdel);
        } else {
            return deleted;
        }
    };

    vec<T>(const char *str);
    vec<T>& operator=(const char *);
    vec<T>& operator+=(const char *);
    char* c_str();
};

vec<char>::vec<char>(const char *str);
vec<char>& vec<char>::operator=(const char *str);
vec<char>& vec<char>::operator+=(const char *str);
char* vec<char>::c_str();

#endif /* VEC_H */
