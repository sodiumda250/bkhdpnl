/*
 * $Id: vec.cpp,v 1.4 2005/05/11 02:57:09 woods Exp $
 */

static char id[] = "$Id: vec.cpp,v 1.4 2005/05/11 02:57:09 woods Exp $";

#include "vec.h"

/**
 * @brief �����񂩂�̃R�s�[�R���X�g���N�^
 * @param str �R�s�[��������
 * 
 * ������̒����{�P�̗̈���m�ۂ��A��������i�[����
 * �Ō�̗v�f�̂P��ɂ�'\0'��ݒ肷��B
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
 * @brief �����������Z�q
 * @param str ������镶����
 *
 * �p�����[�^�Ŏw�肳�ꂽ�R�s�[��������̒����{�P�̗̈���m�ۂ��A�l���R�s�[����B
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
 * @brief ������ǉ�������Z�q
 * @param str �ǉ����镶����
 *
 * �p�����[�^�Ŏw�肳�ꂽ�R�s�[����������A���v�f�̌��ɒǉ�����
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
 * @brief C�X�^�C���̕������ԋp����
 *
 * �Ō��0���Ȃ���Βǉ�����C�̕�������쐬���ĕԋp����
 */
char* vec<char>::c_str()
{
    wide(m_len + 1);
    m_elem[m_len] = 0;

    return m_elem;
}
