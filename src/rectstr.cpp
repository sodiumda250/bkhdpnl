/*
 * $Id: rectstr.cpp,v 1.7 2005/05/26 04:59:37 woods Exp $
 */

static char id[] = "$Id: rectstr.cpp,v 1.7 2005/05/26 04:59:37 woods Exp $";

#include <windows.h>
#include "vec.h"
#include "rectstr.h"

COLORREF rectchar::c_color = 0x00000000;

/**
 * @brief �_�u���o�C�g�������܂ޕ�����̕�������ԋp����B
 * @param str ���������v�Z���镶����
 * @return ������
 *
 * �_�u���o�C�g�������܂ޕ�����̕�������ԋp����B
 * �A���A"\r\n"��1�����Ƃ��Đ�����B
 */
static int strlenDBCS(const char *str)
{
    int len = 0;

    while (*str != 0) {
        if (::IsDBCSLeadByte(*str)) {
            str++;
            if (*str == 0) {
                break;
            }
        } else if (*str == '\r') {
            str++;
            if (*str == 0) {
                break;
            } else if (*str != '\n') {
                str--;
            }
        }
        len++;
        str++;
    }
    return len;
}

/**
 * @brief �\�������z��\���̂ɕ������ǉ�����
 * @param str �ǉ����镶����
 *
 * �\�������z��\���̂ɕ������ݒ肷��B
 */
rectstr& rectstr::operator+=(const char* str)
{
    size_t i, j;
    size_t slen;
    size_t len = strlenDBCS(str);
    size_t new_len = m_len + len;

    memset(&m_rect, 0, sizeof(m_rect));

    wide(new_len);

    i = 0;
    j = m_len;
    slen = strlen(str);
    while ((i < slen) && (j < new_len)) {
        m_elem[j].init();
        if (::IsDBCSLeadByte(str[i])) {
            m_elem[j++].set(str + i, 2);
            i += 2;
        } else if ((str[i] == '\r') && (str[i + 1] == '\n')) {
            m_elem[j++].set("\n", 1);
            i += 2;
        } else {
            m_elem[j++].set(str + i, 1);
            i += 1;
        }
    }
    m_len = new_len;
    return *this;
}

/**
 * @brief �\�������z��\���̂ɕ������ݒ肷��
 * @param str �ݒ肷�镶����
 *
 * �\�������z��\���̂ɕ������ݒ肷��B
 */
rectstr& rectstr::operator=(const char* str)
{
    int i, j;
    int slen;
    int len = strlenDBCS(str);

    memset(&m_rect, 0, sizeof(m_rect));

    wide(len);

    i = j = 0;
    slen = strlen(str);
    while ((i < slen) && (j < len)) {
        m_elem[j].init();
        if (::IsDBCSLeadByte(str[i])) {
            m_elem[j++].set(str + i, 2);
            i += 2;
        } else if ((str[i] == '\r') && (str[i + 1] == '\n')) {
            m_elem[j++].set("\n", 1);
            i += 2;
        } else {
            m_elem[j++].set(str + i, 1);
            i += 1;
        }
    }
    m_len = len;
    return *this;
}
/**
 * @brief �\�������z��\���̂̕\���̈���v�Z���Đݒ肷��
 * @param hdc �\���p�f�o�C�X�R���e�L�X�g
 * @param pt �\���ʒu
 *
 * �\�������z��\���̂̕\���̈���v�Z���Đݒ肷��B
 */
void rectstr::setrect(HDC hdc, const POINT& pt)
{
    size_t i;
    SIZE size;
    POINT next;

    next = pt;
    m_rect.top = pt.y;
    m_rect.bottom = pt.y;
    m_rect.left = pt.x;
    m_rect.right = pt.x;

    for (i = 0; i < m_len; i++) {
        if ((m_elem[i].str()[0] == '\n') || (m_elem[i].str()[0] == '\r')){
            m_elem[i].rect().top = next.y;
            m_elem[i].rect().left = next.x;
            m_elem[i].rect().bottom = next.y;
            m_elem[i].rect().right = next.x;
            next.y += size.cy;
            next.x = pt.x;
        } else if (m_elem[i].str()[0] == '\t') {
            ::GetTextExtentPoint32(hdc, " ", 1, &size);
            long tsize = size.cx * tabsize();
            long tabs = tsize;
            while (tabs < next.x) {
                tabs += tsize;
            }
            m_elem[i].rect().top = next.y;
            m_elem[i].rect().left = next.x;
            m_elem[i].rect().bottom = next.y + size.cy;
            m_elem[i].rect().right = tabs;
            next.x = tabs;
        } else {
            ::GetTextExtentPoint32(hdc, m_elem[i].str(), m_elem[i].length(), &size);
            m_elem[i].rect().top = next.y;
            m_elem[i].rect().left = next.x;
            m_elem[i].rect().bottom = next.y + size.cy;
            m_elem[i].rect().right = next.x + size.cx;
            next.x += size.cx;
        }
        if (m_rect.bottom < m_elem[i].rect().bottom) {
            m_rect.bottom = m_elem[i].rect().bottom;
        }
        if (m_rect.right < m_elem[i].rect().right) {
            m_rect.right = m_elem[i].rect().right;
        }
    }
}
