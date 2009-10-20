/*
 * $Id: vec.h,v 1.7 2005/05/12 06:16:20 woods Exp $
 */

#ifndef VEC_H
#define VEC_H

#include <cstddef>
#include <string.h>

template <class T> class vec {
protected:
/// @brief �̈�m�ێ��̍ŏ��T�C�Y
const static size_t MIN_SIZE = 32;
    /// @brief ���݊m�ۂ��Ă���̈�̑傫��
    size_t m_size;
    /// @brief ���ݕێ����Ă���̈�̑傫��
    size_t m_len;
    /// @brief �i�[���Ă��郁�����̃|�C���^
    T* m_elem;
public:
    /// @brief �v�f�ɃA�N�Z�X���邽�߂̃C�e���[�^
    typedef size_t iterator;
protected:
    /**
     * @brief �̈���g������
     * @param s �g������傫��
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
     * @brief �ċA�I�Ɍ������Ďw�肳�ꂽ�v�f���폜����
     * @param dst �폜����v�f
     * @param first �폜�v�f�̌����J�n�ʒu
     * @param deleted ���ɍ폜�����v�f�����݂��邩�ǂ���
     * @retval true �폜���ꂽ�v�f�����݂���
     * @retval false �폜���ꂽ�v�f�����݂��Ȃ�
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
     * @brief �f�t�H���g�R���X�g���N�^
     * @param s �m�ۂ���̈�̑傫��(�f�t�H���g�l�FMIN_SIZE�j
     *
     * �p�����[�^�Ŏw�肳�ꂽ(�܂��͊m�ۃ������̍ŏ��l��)�̈���m�ۂ���
     */
    vec<T>(size_t s = MIN_SIZE) {
        m_size = 0;
        m_len = 0;
        m_elem = NULL;
        wide(s);
    };

    /**
     * @brief �R�s�[�R���X�g���N�^
     * @param dst �R�s�[��
     *
     * �p�����[�^�Ŏw�肳�ꂽ�R�s�[���Ɠ����傫���̗̈���m�ۂ��A
     * �l���R�s�[����B
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
     * @brief �f�X�g���N�^
     *
     * �m�ۂ����̈��j������B
     */
    ~vec<T>() {
        if (m_elem != NULL) {
            delete[] m_elem;
        }
        m_size = 0;
        m_len = 0;
    };

    /**
     * @brief �ێ����Ă������e������������
     * @param s �m�ۂ���̈�̑傫��(�f�t�H���g�l�FMIN_SIZE�j
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
     * @brief ������Z�q
     * @param dst �R�s�[��
     *
     * �p�����[�^�Ŏw�肳�ꂽ�R�s�[���Ɠ����傫���̗̈���m�ۂ��A
     * �l���R�s�[����B
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

    /// @brief ���e�̐擪�������C�e���[�^��ԋp����
    inline iterator begin() {return 0;};
    /// @brief ���e�̖���(�̈���)�������C�e���[�^��ԋp����
    inline iterator end() {return m_len;};

    /// @brief �ێ����Ă�����e�̌���ԋp����
    inline size_t length() {return m_len;};

    /**
     * @brief �ێ���������p�����[�^�Ŏw�肵�����Ɋg�傷��
     * @param s �g�傷��̈�̑傫��
     */
    inline size_t setsize(size_t s) {wide(s); m_len = s; return s;};

    /**
     * @brief �A�N�Z�X���Z�q
     * @param index �C�e���[�^
     *
     * �p�����[�^�Ŏw�肳�ꂽ���e��ԋp����
     */
    inline T& operator[](iterator index) {return m_elem[index];};

    /**
     * @brief �v�f��ǉ�����
     * @param dst �ǉ�����v�f
     */
    inline size_t add(T& dst) {
        wide(m_len + 1);
        m_elem[m_len++] = dst;
        return m_len;
    };

    /**
     * @brief �v�f���폜����
     * @param dst �ǉ�����v�f
     * @retval true �폜���ꂽ
     * @retval false �폜����Ă��Ȃ�
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
