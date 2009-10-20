/*
 * $Id: rectstr.h,v 1.3 2005/05/10 01:18:18 woods Exp $
 */

#ifndef RECTCHAR_H
#define RECTCHAR_H

#include <windows.h>

#include "vec.h"

/// �\�������\����
class rectchar {
    /// �\���̈�
    RECT m_rect;
    /// �\������
    char m_str[3];
    /// �\��������
    unsigned char m_len;
    /// ���]�\���L��
    int m_reverse;
public:
    /// @brief �\���̈��ԋp����
    RECT& rect(void){return m_rect;};
    /// @brief �\��������ԋp����
    const char* str(void){return m_str;};
    const char* c_str(void){return m_str;};
    /// @brief �\����������ԋp����
    const int length(void){return m_len;};
    /// @brief ���]�\���L����ԋp����
    const int reverse(void){ return m_reverse;};

    void init(void) {
        memset(this, 0, sizeof(*this));
    };
    void free(void) {
        memset(this, 0, sizeof(*this));
    };
    rectchar(void){init();};
    ~rectchar(void){free();};

    /**
     * @brief ������ݒ肷��
     * @param str �ݒ肷�镶��
     * @param len �����̒���
     */
    void set(const char* str, int len) {
        memset(m_str, 0, sizeof(m_str));
        m_len = (((size_t)len > sizeof(m_str) - 1) ? (sizeof(m_str) - 1) : (size_t)len);
        strncpy(m_str, str, m_len);
        m_str[m_len] = 0;
    };

    /**     * @brief ���]�\���L����ݒ肷��
     * @param rev ���]�\���L��
     */
    int setreverse(const int rev) {
        return m_reverse = rev;
    };

    /**
     * @brief �\���̈悪�w�肳�ꂽ�̈�ɏd�Ȃ邩�`�F�b�N����
     * @param rect �`�F�b�N����̈�
     * @retval TRUE �d�Ȃ�
     * @retval FALSE �d�Ȃ�Ȃ�
     */
    BOOL in_rect(const RECT& rect) {
        if (((m_rect.left <= rect.right) && (m_rect.right >= rect.left)) &&
            ((m_rect.top <= rect.bottom) && (m_rect.bottom >= rect.top)))
        {
            return TRUE;
        }
        return FALSE;
    };
};

class rectstr : public vec<rectchar> {
    const static int TAB_SIZE = 4;
    RECT m_rect;
public:
    static inline int tabsize(void){return TAB_SIZE;};
    const RECT& rect(void) {return m_rect;};
    rectstr& operator=(const char* str);
    void setrect(HDC hdc, const POINT& pt);
    rectstr& operator+=(const char* str);
};

#endif /* RECTCHAR_H */
