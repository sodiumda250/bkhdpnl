/*
 * $Id: dialog.cpp,v 1.1 2005/05/10 02:32:50 woods Exp $
 * dialog.cpp : �_�C�A���O�p�̃v���V�[�W����`
 */

static char id[] = "$Id: dialog.cpp,v 1.1 2005/05/10 02:32:50 woods Exp $";

#include <stdio.h>
#include "resource.h"
#include "dialog.h"
#include "bkhdpnl.h"
#include "panelwin.h"

// �O���[�o���ϐ�:

/// ���݂̃C���X�^���X
static HINSTANCE hInstance;

/// �ݒ�_�C�A���O�̕\���w�b�_�ݒ蕶����o�b�t�@
static char *setstr;
/// �ݒ�_�C�A���O�̕\���w�b�_�ݒ蕶����o�b�t�@�̑傫��
static int setsize;
/// �E�B���h�E���������L��
static int adjustwindow;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̑O�錾:
extern "C" {
static LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK Setting(HWND, UINT, WPARAM, LPARAM);
}

static BOOL SelectFont(void);

/**
 * @brief �ݒ�_�C�A���O�\���֐�
 * @param hInst �_�C�A���O���܂܂��C���X�^���X�̃n���h��
 * @param hWnd �e�E�B���h�E�̃E�B���h�E�n���h��
 * @param str �\������w�b�_������̏����l(�J���}��؂�)
 * @param size str�o�b�t�@�̑傫��
 * @param adjust �E�B���h�E�T�C�Y�������߃t���O
 *
 * �ݒ�_�C�A���O��\������B
 */
int DoSetting(HINSTANCE hInst, HWND hWnd, char *str, int size, UINT *adjust)
{
    setstr = str;
    setsize = size;
    adjustwindow = *adjust;
    hInstance = hInst;
    ::DialogBox(hInst, (LPCTSTR) IDD_SETTING, hWnd, (DLGPROC) Setting);

    *adjust = (adjustwindow != BST_UNCHECKED) ? 1 : 0;

    SaveWindowPos(szIni);
    return 0;
}

/**
 * @brief About�_�C�A���O�\���֐�
 * @param hInst �_�C�A���O���܂܂��C���X�^���X�̃n���h��
 * @param hWnd �e�E�B���h�E�̃E�B���h�E�n���h��
 *
 * About�_�C�A���O��\������B
 */
int DoAbout(HINSTANCE hInst, HWND hWnd)
{
    ::DialogBox(hInst, (LPCTSTR) IDD_ABOUTBOX, hWnd, (DLGPROC) About);
    return 0;
}

/**
 * @brief �ݒ�_�C�A���O�p���b�Z�[�W �n���h��
 * @param hDlg : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �ݒ�_�C�A���O�̃E�B���h�E���b�Z�[�W����������B
 */
static LRESULT CALLBACK
Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT length;
    UINT chk;

    switch (message) {
    case WM_INITDIALOG:
        chk = (adjustwindow != 0) ? BST_CHECKED : BST_UNCHECKED;
        ::CheckDlgButton(hDlg, IDC_ADJUSTWINDOW, chk);
        ::SetDlgItemText(hDlg, IDC_HEADERS, setstr);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            length =
                ::GetDlgItemText(hDlg, IDC_HEADERS, setstr, setsize);
            adjustwindow = ::IsDlgButtonChecked(hDlg, IDC_ADJUSTWINDOW);
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            break;
        case IDCANCEL:
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            break;
        case IDC_ABOUT:
            DoAbout(hInstance, hDlg);
            break;
        case IDC_FONT:
            SelectFont();
            break;
        }
        break;
    default:
        break;
    }
    return FALSE;
}


/**
 * @brief About�_�C�A���O���b�Z�[�W �n���h��
 * @param hDlg : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * About�_�C�A���O�̃E�B���h�E���b�Z�[�W����������B
 */
static LRESULT CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/**
 * @brief �t�H���g�I���_�C�A���O�\��
 *
 * ini�t�@�C������t�H���g�����擾���āA
 * ����������l�Ƃ��ăt�H���g�I���R�����_�C�A���O��\�����A
 * �R�����_�C�A���O�Őݒ肳�ꂽ����ini�t�@�C���ɐݒ肷��B
 */
static BOOL SelectFont(void)
{
    BOOL ret;
    CHOOSEFONT font;
    LOGFONT logfont;

    memset(&font, 0, sizeof(font));

    LoadFont(&logfont, szIni);

    font.lStructSize = sizeof(font);
    font.lpLogFont = &logfont;
    font.hInstance = hInstance;
    font.Flags = CF_SCREENFONTS | CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT ;
    ret = ::ChooseFont(&font);
    if (ret) {
        SaveFont(&logfont, szIni);
    }
    return ret;
}
