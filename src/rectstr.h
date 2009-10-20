/*
 * $Id: rectstr.h,v 1.9 2005/05/26 04:09:08 woods Exp $
 */

#ifndef RECTSTR_H
#define RECTSTR_H

#include <ctype.h>
#include <windows.h>

#include "vec.h"

/// 表示文字構造体
class rectchar {
    /// @brief デフォルト表示色
    static COLORREF c_color;
private:
    /// @brief 表示領域
    RECT m_rect;
    /// @brief 表示文字
    char m_str[3];
    /// @brief 表示文字長
    unsigned char m_len;
    /// @brief 反転表示有無
    int m_reverse;
    /// @brief 表示色
    COLORREF m_color;
public:
    /// @brief デフォルト表示色を返却する
    static COLORREF& defautColor(void){return c_color;};
    /// @brief デフォルト表示色を設定する
    static void setDefaultColor(COLORREF& col){c_color = col;};
public:
    /// @brief 表示領域を返却する
    RECT& rect(void){return m_rect;};
    /// @brief 表示文字を返却する
    const char* str(void){return m_str;};
    /// @brief C文字列を返却する
    const char* c_str(void){return m_str;};
    /// @brief 表示文字長を返却する
    const int length(void){return m_len;};
    /// @brief 反転表示有無を返却する
    const int reverse(void){ return m_reverse;};
    /// @brief 表示色を返却する
    const COLORREF& color(void) {return m_color;};

    /// @brief インスタンスを初期化する
    void init(void) {
        memset(this, 0, sizeof(*this));
        m_color = c_color;
    };
    /// @brief インスタンスを初期化する
    void free(void) {
        memset(this, 0, sizeof(*this));
    };
    /// @brief コンストラクタ
    rectchar(void){init();};
    /// @brief デストラクタ
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

    /**
     * @brief 反転表示有無を設定する
     * @param rev 反転表示有無
     */
    int setreverse(const int rev) {
        return m_reverse = rev;
    };

    /**
     * @brief 表示色を設定する
     * @param col 設定する表示色
     */
     void setcolor(COLORREF& col) {
         m_color = col;
     }

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

    /**
     * @brief 行末文字かどうかチェックする
     * @retval true 行末文字
     * @retval false 行末文字でない
     */
    BOOL eol(void) {
        if ((m_str[0] == '\r') || (m_str[0] == '\n')) {
            return true;
        } else {
            return false;
        }
    };

    /**
     * @brief 空白文字かどうかチェックする
     * @retval true 空白文字
     * @retval false 非空白文字
     */
    BOOL isspace(void) {
        for (size_t i = 0; i < m_len; i++) {
            if (!::isspace(m_str[i])) {
                return false;
            }
        }
        return true;
    }
};

class rectstr : public vec<rectchar> {
    /// @brief タブサイズ
    const static int TAB_SIZE = 4;
    /// @brief 文字列表示領域
    RECT m_rect;
public:
    /// @brief タブサイズを返却する
    static inline int tabsize(void){return TAB_SIZE;};
    /// @brief 文字列表示領域を返却する
    inline const RECT& rect(void) {return m_rect;};
    rectstr& operator=(const char* str);
    void setrect(HDC hdc, const POINT& pt);
    rectstr& operator+=(const char* str);
};

#endif /* RECTSTR_H */
