/*
 * $Id: hookproc.cpp,v 1.2 2005/05/12 02:51:27 woods Exp $
 */

static char id[] = "$Id: hookproc.cpp,v 1.2 2005/05/12 02:51:27 woods Exp $";

#include <stdio.h>
#include <windows.h>
#include "hookproc.h"
#include "panel.h"

extern "C" {
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

/// ���̃E�B���h�E�v���V�[�W��
static LONG prevWndProc = 0;
/// �T�u�N���X�������E�B���h�E�̃E�B���h�E�n���h��
static HWND hookhwnd = 0;

/**
 * @brief �E�B���h�E�̃T�u�N���X��
 * @param hMainWnd �T�u�N���X������E�B���h�E�̃E�B���h�E�n���h��
 *
 * �w�肳�ꂽ�E�B���h�E���T�u�N���X������B
 */
void hookMailWindow(HWND hMainWnd)
{
    prevWndProc = ::SetWindowLong(hMainWnd, GWL_WNDPROC, (LONG)WindowProc);
    if (prevWndProc != 0) {
        hookhwnd = hMainWnd;
    } else {
        hookhwnd = 0;
    }
}

/**
 * @brief �T�u�N���X������
 *
 * hookMailWindow()�̃T�u�N���X������������B
 */
void unhookMailWindow(void)
{
    if (prevWndProc != 0) {
        ::SetWindowLong(hookhwnd, GWL_WNDPROC, (LONG)prevWndProc);
        prevWndProc = 0;
        hookhwnd = 0;
    }
}

/**
 * @brief �T�u�N���X�E�B���h�E���b�Z�[�W�n���h��
 * @param hwnd �E�B���h�E�̃n���h��
 * @param uMsg ���b�Z�[�W
 * @param wParam ���b�Z�[�W�̒ǉ����1
 * @param lParam ���b�Z�[�W�̒ǉ����2
 */
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    ::GetWindowThreadProcessId((HWND)(lParam), &pid_before);
    ::GetWindowThreadProcessId(hookhwnd, &pid_after);

    switch (uMsg) {
    case WM_ACTIVATE:
        if (pid_before != pid_after && (BOOL)HIWORD(wParam) == false) {
            switch (LOWORD(wParam)) {
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                ShowWindow();
                break;
            case WA_INACTIVE:
                HideWindow();
                break;
            default:
               break;
            }
        }
        break;
    default:
        break;
    }
    return ::CallWindowProc((WNDPROC)prevWndProc, hwnd, uMsg, wParam, lParam);
}

