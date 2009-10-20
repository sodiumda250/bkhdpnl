/*
 * $Id: rectstr.h,v 1.3 2005/05/10 01:18:18 woods Exp $
 */

#ifndef RECTCHAR_H
#define RECTCHAR_H

#include <windows.h>

#include "vec.h"

/// 表示文字構造体
class rectchar {
    /// 表示領域
    RECT m_rect;
    /// 表示文字
    char m_str[3];
    /// 表示文字長
    unsigned char m_len;
    /// 反転表示有無
    int m_reverse;
public:
    /// @brief 表示領域を返却する
    RECT& rect(void){return m_rect;};
    /// @brief 表示文字を返却する
    const char* str(void){return m_str;};
    const char* c_str(void){return m_str;};
    /// @brief 表示文字長を返却する
    const int length(void){return m_len;};
    /// @brief 反転表示有無を返却する
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
     * @brief 文字を設定する
     * @param str 設定する文字
     * @param len 文字の長さ
     */
    void set(const char* str, int len) {
        memset(m_str, 0, sizeof(m_str));
        m_len = (((size_t)len > sizeof(m_str) - 1) ? (sizeof(m_str) - 1) : (size_t)len);
        strncpy(m_str, str, m_len);
        m_str[m_len] = 0;
    };

    /**     * @brief 反転表示有無を設定する
     * @param rev 反転表示有無
     */
    int setreverse(const int rev) {
        return m_reverse = rev;
    };

    /**
     * @brief 表示領域が指定された領域に重なるかチェックする
     * @param rect チェックする領域
     * @retval TRUE 重なる
     * @retval FALSE 重ならない
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
