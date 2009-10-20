/*
 * $Id: vec.cpp,v 1.4 2005/05/11 02:57:09 woods Exp $
 */

static char id[] = "$Id: vec.cpp,v 1.4 2005/05/11 02:57:09 woods Exp $";

#include "vec.h"

/**
 * @brief 文字列からのコピーコンストラクタ
 * @param str コピー元文字列
 * 
 * 文字列の長さ＋１の領域を確保し、文字列を格納する
 * 最後の要素の１つ後にを'\0'を設定する。
 */
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

/**
 * @brief 文字列代入演算子
 * @param str 代入する文字列
 *
 * パラメータで指定されたコピー元文字列の長さ＋１の領域を確保し、値をコピーする。
 */
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

/**
 * @brief 文字列追加代入演算子
 * @param str 追加する文字列
 *
 * パラメータで指定されたコピー元文字列を、現要素の後ろに追加する
 */
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

/**
 * @brief Cスタイルの文字列を返却する
 *
 * 最後に0がなければ追加してCの文字列を作成して返却する
 */
char* vec<char>::c_str()
{
    wide(m_len + 1);
    m_elem[m_len] = 0;

    return m_elem;
}
