/*
 * $Id: vec.h,v 1.7 2005/05/12 06:16:20 woods Exp $
 */

#ifndef VEC_H
#define VEC_H

#include <cstddef>
#include <string.h>

template <class T> class vec {
protected:
/// @brief 領域確保時の最小サイズ
const static size_t MIN_SIZE = 32;
    /// @brief 現在確保している領域の大きさ
    size_t m_size;
    /// @brief 現在保持している領域の大きさ
    size_t m_len;
    /// @brief 格納しているメモリのポインタ
    T* m_elem;
public:
    /// @brief 要素にアクセスするためのイテレータ
    typedef size_t iterator;
protected:
    /**
     * @brief 領域を拡張する
     * @param s 拡張する大きさ
     */
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

    /**
     * @brief 再帰的に検索して指定された要素を削除する
     * @param dst 削除する要素
     * @param first 削除要素の検索開始位置
     * @param deleted 既に削除した要素が存在するかどうか
     * @retval true 削除された要素が存在する
     * @retval false 削除された要素が存在しない
     */
    bool inner_del(T& dst, iterator first, bool deleted) {
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
            return inner_del(dst, first, isdel);
        } else {
            return deleted;
        }
    };

public:
    /**
     * @brief デフォルトコンストラクタ
     * @param s 確保する領域の大きさ(デフォルト値：MIN_SIZE）
     *
     * パラメータで指定された(または確保メモリの最小値の)領域を確保する
     */
    vec<T>(size_t s = MIN_SIZE) {
        m_size = 0;
        m_len = 0;
        m_elem = NULL;
        wide(s);
    };

    /**
     * @brief コピーコンストラクタ
     * @param dst コピー元
     *
     * パラメータで指定されたコピー元と同じ大きさの領域を確保し、
     * 値をコピーする。
     */
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

    /**
     * @brief デストラクタ
     *
     * 確保した領域を破棄する。
     */
    ~vec<T>() {
        if (m_elem != NULL) {
            delete[] m_elem;
        }
        m_size = 0;
        m_len = 0;
    };

    /**
     * @brief 保持していた内容を初期化する
     * @param s 確保する領域の大きさ(デフォルト値：MIN_SIZE）
     */
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

    /**
     * @brief 代入演算子
     * @param dst コピー元
     *
     * パラメータで指定されたコピー元と同じ大きさの領域を確保し、
     * 値をコピーする。
     */
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

    /// @brief 内容の先頭を示すイテレータを返却する
    inline iterator begin() {return 0;};
    /// @brief 内容の末尾(の一つ後ろ)を示すイテレータを返却する
    inline iterator end() {return m_len;};

    /// @brief 保持している内容の個数を返却する
    inline size_t length() {return m_len;};

    /**
     * @brief 保持する個数をパラメータで指定した分に拡大する
     * @param s 拡大する領域の大きさ
     */
    inline size_t setsize(size_t s) {wide(s); m_len = s; return s;};

    /**
     * @brief アクセス演算子
     * @param index イテレータ
     *
     * パラメータで指定された内容を返却する
     */
    inline T& operator[](iterator index) {return m_elem[index];};

    /**
     * @brief 要素を追加する
     * @param dst 追加する要素
     */
    inline size_t add(T& dst) {
        wide(m_len + 1);
        m_elem[m_len++] = dst;
        return m_len;
    };

    /**
     * @brief 要素を削除する
     * @param dst 追加する要素
     * @retval true 削除された
     * @retval false 削除されていない
     */
    inline bool del(T& dst) {
        return inner_del(dst, 0, false);
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
