/*
 * $Id: panel.cpp,v 1.1 2005/05/12 06:19:55 woods Exp $
 */

static char id[] = "$Id: panel.cpp,v 1.1 2005/05/12 06:19:55 woods Exp $";

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <ctype.h>
#include "resource.h"
#include "bkhdpnl.h"
#include "panelwin.h"
#include "panel.h"

// �\�[�X���O���[�o��

PanelWindow *Panel = NULL;

static void Redraw(void);

/**
 * @brief �w�b�_�\���E�B���h�E���̏�����
 * @param szIniFile ini�t�@�C���̃p�X��
 * @retval TRUE ����������
 * @retval FALSE ���������s
 *
 * �w�b�_�\���E�B���h�E��������������B
 */
BOOL InitHeader(HINSTANCE hInstance, const char *szIniFile)
{
    char buf[1024];
    int len;
    int i, j;

    PanelWindow::initClass(hInstance);
    Panel = new PanelWindow();

    LoadAdjust(szIniFile);

    Panel->rect().top =
             GetPrivateProfileInt("BkHdPnl", "Top", 0, szIni);
    Panel->rect().left =
             GetPrivateProfileInt("BkHdPnl", "Left", 0, szIni);
    int cy = GetPrivateProfileInt("BkHdPnl", "Height", 45, szIni);
    int cx = GetPrivateProfileInt("BkHdPnl", "Width", 200, szIni);
    Panel->rect().bottom = Panel->rect().top + cy;
    Panel->rect().right = Panel->rect().left + cx;

    len = ::GetPrivateProfileString("BkHdPnl", "HeaderList", "X-Mailer",
                          buf, sizeof(buf) - 1, szIni);

    Panel->header().init();
    for (i = 0; i < len; i++) {
        for (j = i;
             (j < len) && (buf[j] != ',') && (buf[j] != '\r') && (buf[j] != '\n');
             j++) { /* do nothing */ }
        buf[j] = 0;
        {
            vec<char> a(buf + i);
            Panel->header().add(a);
        }
        i = j;
    }

    if (Panel != NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief �w�b�_�\���E�B���h�E���̃f�X�g���N�^
 * @retval TRUE �j�󐬌�
 * @retval FALSE �j�󎸔s
 *
 * �w�b�_�\���E�B���h�E����j������B
 */
BOOL DestroyHeader(void)
{
    SaveWindowPos(szIni);

    delete Panel;
    Panel = NULL;

    PanelWindow::deleteFont();

    return TRUE;
}

/**
 * @brief �w�b�_���̕\��
 * @param lpMailID : ���[��ID
 * @return �\���E�B���h�E�̌�
 *
 * �w�肳�ꂽ���[���̃w�b�_��񂩂�w�b�_�\���E�B���h�E��\������B
 */
int ShowHeader(LPCTSTR lpMailID)
{
    if (Panel != NULL) {
        Panel->ShowHeader(lpMailID);
        Panel->ShowWindow();
    }
    return 0;
}

/**
 * @brief �w�b�_�\���E�B���h�E�̕\��
 *
 * ��A�N�e�B�u���Ŕ�\���ɂȂ��Ă���w�b�_�\���E�B���h�E��\������B
 */
void ShowWindow(void)
{
    if (Panel != NULL) {
        Panel->ShowWindow();
    }
}

/**
 * @brief �w�b�_�\���E�B���h�E�̔�\����
 *
 * ���C���E�B���h�E�̔�A�N�e�B�u���ɔ����ăw�b�_�\���E�B���h�E���\���ɂ���B
 */
void HideWindow(void)
{
    if (Panel != NULL) {
        Panel->HideWindow();
    }
}

/**
 * @brief �w�b�_�\���p�E�B���h�E�̍쐬
 * @param hInstance : Window Procedure�̂��郂�W���[���̃C���X�^���X
 * @param hParentWnd : �e�E�B���h�E
 * @retval TRUE ����
 * @retval FALSE ���s
 *
 * �w�b�_�\���E�B���h�E���쐬����B
 */
BOOL
InitHeaederInstance(HINSTANCE hInstance, HWND hParentWnd)
{
    HWND hWnd;
    LOGFONT logfont;
    HDC hDC;

    LoadFont(&logfont, szIni);
    PanelWindow::setFont(logfont);

    hWnd = Panel->createWindow(hParentWnd);
    if (!hWnd) {
        LPVOID lpMsgBuf;

        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
        );
        MessageBox( NULL, (char *)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
        LocalFree( lpMsgBuf );

        return FALSE;
    }

    hDC = ::GetDC(hWnd);
    ::SelectObject(hDC, PanelWindow::font());

    ::ShowWindow(hWnd, SW_HIDE);
    ::UpdateWindow(hWnd);
    ::ReleaseDC(hWnd, hDC);

    ::GetWindowRect(hWnd, &(Panel->rect()));

    return TRUE;
}

/**
 * @brief �t�H���g��񃍁[�h
 * @param lf �t�H���g�������[�h���邽�߂�LOGFONT�\���̂������|�C���^
 * @param szIni ini�t�@�C���̃p�X��
 * @retval TRUE �t�H���g��񃍁[�h����
 * @retval FALSE �t�H���g��񃍁[�h���s
 *
 * �t�H���g����ini�t�@�C�����烍�[�h����B
 */
BOOL LoadFont(LOGFONT *lf, const char *szIni)
{
    char buffer[128];

    memset(lf, 0, sizeof(LOGFONT));

    lf->lfHeight =
        ::GetPrivateProfileInt("BkHdPnl", "FontSize", -12, szIni);
    lf->lfCharSet = DEFAULT_CHARSET;
    lf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf->lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf->lfPitchAndFamily = DEFAULT_PITCH;
    ::GetPrivateProfileString("BkHdPnl", "FontName", "System",
                              buffer, sizeof(buffer), szIni);
    strncpy(lf->lfFaceName, buffer, sizeof(lf->lfFaceName));
    lf->lfWeight = ::GetPrivateProfileInt("BkHdPnl", "FontWeight", 400, szIni);
    lf->lfItalic = ::GetPrivateProfileInt("BkHdPnl", "FontItalic", 0, szIni);

    return TRUE;
}

/**
 * @brief �t�H���g���Z�[�u
 * @param lf �t�H���g���ɃZ�[�u����LOGFONT�\���̂������|�C���^
 * @param szIni ini�t�@�C���̃p�X��
 * @retval TRUE �t�H���g���Z�[�u����
 * @retval FALSE �t�H���g���Z�[�u���s
 *
 * �t�H���g����ini�t�@�C���ɃZ�[�u����B
 */
BOOL SaveFont(LOGFONT *lf, const char *szIni)
{
    char buffer[128];

    ::WritePrivateProfileString("BkHdPnl", "FontName",
                                lf->lfFaceName, szIni);
    sprintf(buffer, "%ld", lf->lfHeight);
    ::WritePrivateProfileString("BkHdPnl", "FontSize",
                                buffer, szIni);
    sprintf(buffer, "%ld", lf->lfWeight);
    ::WritePrivateProfileString("BkHdPnl", "FontWeight", buffer, szIni);

    sprintf(buffer, "%d", lf->lfItalic);
    ::WritePrivateProfileString("BkHdPnl", "FontItalic", buffer, szIni);

    PanelWindow::setFont(lf);
    Redraw();
    return TRUE;
}

/**
 * @brief �w�b�_�\���E�B���h�E�̍ĕ`��
 *
 * �w�b�_�\���E�B���h�E��S�čĕ`�悷��B
 */
static void Redraw(void)
{
    Panel->redraw();
}

/**
 * @brief �w�b�_�\���E�B���h�E�ʒu�̃Z�[�u
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �w�b�_�\���E�B���h�E�̈ʒu��ini�t�@�C���ɃZ�[�u����B
 */
void SaveWindowPos(const char *szIni)
{
    char buf[128];

    sprintf(buf, "%ld", Panel->rect().top);
    ::WritePrivateProfileString("BkHdPnl", "Top",
                                buf, szIni);
    sprintf(buf, "%ld", Panel->rect().left);
    ::WritePrivateProfileString("BkHdPnl", "Left",
                                buf, szIni);
    sprintf(buf, "%ld", Panel->rect().bottom - Panel->rect().top);
    ::WritePrivateProfileString("BkHdPnl", "Height",
                                buf, szIni);
    sprintf(buf, "%ld", Panel->rect().right - Panel->rect().left);
    ::WritePrivateProfileString("BkHdPnl", "Width",
                                buf, szIni);
}

/**
 * @brief �E�B���h�E����������񃍁[�h
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �E�B���h�E�̑傫���������������邩�ǂ����̏������[�h����B
 */ 
UINT LoadAdjust(const char *szIni)
{
    UINT ret = ::GetPrivateProfileInt("BkHdPnl", "AdjustWindow", 0, szIni);

    PanelWindow::setadjust(ret);
    return ret;
}

/**
 * @brief �E�B���h�E�����������Z�[�u
 * @param pAdjust ���������t���O(0:�����������Ȃ��A����ȊO:������������)
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �E�B���h�E�̑傫���������������邩�ǂ����̏���ini�t�@�C���ɃZ�[�u����B
 */ 
void SaveAdjust(UINT pAdjust, const char *szIni)
{
    char buf[32];

    PanelWindow::setadjust(pAdjust);
    sprintf(buf, "%d", pAdjust);
    ::WritePrivateProfileString("BkHdPnl", "AdjustWindow", buf, szIni);
}

/**
 * @brief RECT�͈̔͂̍���
 * @param dst �����Ώۂ�RECT�\���̃|�C���^
 * @param src ��������RECT�\���̃|�C���^
 * @return ��������RECT(dst�Ɠ�������)
 *
 * RECT�͈̔͂��������A�Q��RECT���Ƃ��Ɋ܂܂��RECT���쐬��dst�ɐݒ肷��B
 */
RECT& addrect(RECT& dst, const RECT& src)
{
    dst.top = (dst.top > src.top) ? src.top : dst.top;
    dst.bottom = (dst.bottom < src.bottom) ? src.bottom : dst.bottom;
    dst.left = (dst.left > src.left) ? src.left : dst.left;
    dst.right = (dst.right < src.right) ? src.right : dst.right;

    return dst;
}

/**
 * @brief �����񂩂�1�s�������؂�o���B
 * @param buf �؂�o����1�s���̕�����
 * @param pt �؂�o���Ώۂ̕�����|�C���^�ւ̃|�C���^
 * @return �؂�o����1�s���̕�����
 */
vec<char>& getLine(vec<char>& buf, char **pt)
{
    char *p = *pt;

    buf.init();
    if (p == NULL) {
        return buf;
    }

    while ((*p != '\r') && (*p != '\n') && (*p != 0)) {
        buf.add(*(p++));
    }
    if ((*p == '\r') || (*p == '\n')) {
        buf.add(*(p++));
    }
    if ((*(p - 1) == '\r') && (*p == '\n')) {
        buf.add(*(p++));
    }

    *pt = p;

    return buf;
}
