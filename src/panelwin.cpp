/*
 * $Id: panelwin.cpp,v 1.9 2005/05/10 04:04:16 woods Exp $
 */

static char id[] = "$Id: panelwin.cpp,v 1.9 2005/05/10 04:04:16 woods Exp $";

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <ctype.h>
#include "resource.h"
#include "bkhdpnl.h"
#include "panelwin.h"

// �\�[�X���O���[�o��

PanelWindow *Panel = NULL;

// PanelWindow�N���X��static�����o������
HINSTANCE PanelWindow::c_instance = 0;
int PanelWindow::c_addx = 0;
int PanelWindow::c_addy = 0;
HFONT PanelWindow::c_hFont = 0;
UINT PanelWindow::c_adjust = 0;
TCHAR PanelWindow::szWindowClass[100] = "";
vec<PanelWindow*> PanelWindow::window;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̑O�錾:
extern "C" {
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

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
 * @brief �w�b�_���̕\��
 * @param lpMailID : ���[��ID
 *
 * �w�肳�ꂽ���[���̃w�b�_��񂩂�w�b�_�\���E�B���h�E��\������B
 */
void PanelWindow::ShowHeader(LPCTSTR lpMailID)
{
    bool OutHeader = false;
    vec<char> buf;

    body() = "";
    LPSTR hdr = bka.GetHeader(lpMailID);
    char *pt = hdr;
    while (getLine(buf, &pt).length() != 0) {
        if (isspace(buf[0])) {
            if (OutHeader) {
                body() += buf.c_str();
            }
        } else {
            OutHeader = false;
            for (vec< vec<char> >::iterator i = header().begin();
                 i < header().end();
                 i++)
            {
                if (strnicmp(buf.c_str(),
                             header()[i].c_str(),
                             header()[i].length()) == 0)
                {
                    body() += buf.c_str();
                    OutHeader = true;
                    break;
                }
            }
        }
    }

    // ������̕��ƍ������v�Z���܂��B
    HDC hdc = GetDC(hwnd());
    ::SelectObject(hdc, font());
    POINT ptr;

    ptr.x = 1;
    ptr.y = 1;
    body().setrect(hdc, ptr);
    ReleaseDC(hwnd(), hdc);
    ShowWindow();
    if (adjust() && (body().length() > 0)) {
        AdjustWindow();
    }
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
 * @brief �w�b�_�\���E�B���h�E�N���X�̓o�^
 * @param hInstance : Window Procedure�̂��郂�W���[���̃C���X�^���X
 * @return �E�B���h�E�N���X�����ʂ���A�g��
 *
 * �w�b�_�\���E�B���h�E�̃E�B���h�E�N���X��o�^����B
 */
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
    ATOM ret;
    WNDCLASSEX wcex;


    if (strcmp(PanelWindow::WindowClass(), "BKHDPNL") == 0) {
        return 1;
    }
    PanelWindow::setWindowClass("BKHDPNL");

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wcex.lpfnWndProc = (WNDPROC) WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = PanelWindow::WindowClass();
    wcex.hIcon = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
    wcex.hIconSm = NULL;//LoadIcon(wcex.hInstance, (LPCTSTR) IDI_SMALL);

    ret = RegisterClassEx(&wcex);
    if (ret == 0) {
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
    }
    return ret;
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

    MyRegisterClass(hInstance);

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
 * @brief �w�b�_�\���E�B���h�E���b�Z�[�W�n���h��
 * @param hWnd : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E�̃E�B���h�E���b�Z�[�W����������B
 */
static LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    LRESULT ret = 0;

    PanelWindow& hw = PanelWindow::getWindow(hWnd);

    switch (message) {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // ���j���[�I���̉��:
        switch (wmId) {
        case IDM_COPY:
        case IDM_CUT:
            hw.SetClipBoardText();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CHAR:
        ret = hw.OnChar(wParam, lParam);
        break;
    case WM_PAINT:
        ret = hw.OnPaint(wParam, lParam);
        break;
    case WM_SIZE:
    case WM_MOVE:
        // �E�B���h�E�̑傫���A�ʒu���Z�[�u����B
        ret = hw.OnMove(wParam, lParam);
        break;
    case WM_ACTIVATE:
        ret = hw.OnActivate(wParam, lParam);
        break;
    case WM_DESTROY:
        //PostQuitMessage( 0 );
        ret = hw.OnDestroy(wParam, lParam);
        return ::DefWindowProc(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        ret = hw.OnLButtonDown(wParam, lParam);
        break;
    case WM_LBUTTONUP:
        ret = hw.OnLButtonUp(wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        ret = hw.OnMouseMove(wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
        ret = hw.OnLButtonDblClk(wParam, lParam);
        break;
    case WM_NCLBUTTONDBLCLK:
        ret = hw.OnNCLButtonDblClk(wParam, lParam);
        break;
    case WM_KILLFOCUS:
        ret = hw.OnKillFocus(wParam, lParam);
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return ret;
}

/**
 * @brief WM_CHAR�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^(character code)
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_CHAR���b�Z�[�W����������B
 * Ctrl-C�����͂��ꂽ��w�肳�ꂽ��������N���b�v�{�[�h�ɃR�s�[����B
 */
LRESULT PanelWindow::OnChar(WPARAM wParam, LPARAM lParam)
{
    if ((((TCHAR) wParam) == 'c') || (((TCHAR) wParam) == 'C')) {
        LRESULT ret = SendMessage(hwnd(), WM_GETHOTKEY, (WPARAM)0, (LPARAM)0); 
        if (ret & HOTKEYF_CONTROL) {
            this->SetClipBoardText();
        }
    } else if (((TCHAR) wParam) == 3/* ctrl-C */) {
        this->SetClipBoardText();
    }
    return 0;
}

/**
 * @brief WM_MOVE/WM_SIZE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_MOVE/WM_SIZE���b�Z�[�W����������B
 * �ړ��^�T�C�Y�ύX��̃E�B���h�E�ʒu�͈͂��L�^����B
 */
LRESULT PanelWindow::OnMove(WPARAM wParam, LPARAM lParam)
{
    ::GetWindowRect(hwnd(), &(rect()));

    return 0;
}

/**
 * @brief WM_ACTIVATE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_ACTIVATE���b�Z�[�W����������B
 */
LRESULT PanelWindow::OnActivate(WPARAM wParam, LPARAM lParam)
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    ::GetWindowThreadProcessId((HWND)(lParam), &pid_before);
    ::GetWindowThreadProcessId(hwnd(), &pid_after);

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

    return 0;
}

/**
 * @brief WM_PAINT�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^(the device context to draw in)
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_PAINT���b�Z�[�W���������A
 * �w�b�_�\���E�B���h�E��`�悷��B
 */
LRESULT PanelWindow::OnPaint(WPARAM wParam, LPARAM lParam)
{
    size_t j;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT urect;

    COLORREF ctextn, cbackn;

    hdc = ::BeginPaint(hwnd(), &ps);
    ctextn = ::GetTextColor(hdc);
    cbackn = ::GetBkColor(hdc);
    urect = ps.rcPaint;
    ::SelectObject(hdc, PanelWindow::font());
    for (j = 0; j < body().length(); j++) {
        if (body(j).in_rect(urect)) {
            if (body(j).reverse() != 0) {
                ::SetTextColor(hdc, cbackn);
                ::SetBkColor(hdc, ctextn);
            } else {
                ::SetTextColor(hdc, ctextn);
                ::SetBkColor(hdc, cbackn);
            }
            if (body(j).str()[0] == '\t') {
                ::DrawText(hdc, " ",
                         1,
                         &(body(j).rect()), DT_LEFT);
            } else {
                ::DrawText(hdc, body(j).str(),
                         body(j).length(),
                         &(body(j).rect()), DT_LEFT);
            }
        }
    }
    ::EndPaint(hwnd(), &ps);

    return 0;
}

/**
 * @brief WM_LBUTTONDOWN�C�x���g����
 * @param hWnd : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W(WM_LBUTTONDOWN)
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(mouse clicked point)
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONDOWN�C�x���g����������B
 * �}�E�X�N���b�N�t���O���Z�b�g���A�N���b�N�ʒu���L�^���A�I�𕶎�����N���A����B
 */
LRESULT PanelWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    size_t j;
    RECT rect;

    ::SetActiveWindow(hwnd());
    setclicked(1);
    setclickpoint(MAKEPOINTS(lParam));

    rect.top = rect.bottom = MAKEPOINTS(lParam).y;
    rect.left = rect.right = MAKEPOINTS(lParam).x;
    for (j = 0; j < body().length(); j++) {
        if (body(j).reverse() != 0) {
            body(j).setreverse(0);
            addrect(rect, body(j).rect());
        }
    }
    redraw(rect);
    ::SetCapture(hwnd());

    return 0;
}

/**
 * @brief WM_LBUTTONUP�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(mouse clicked point)
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONUP�C�x���g����������B
 * �N���b�N�t���O�����Z�b�g����B
 */
LRESULT PanelWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    ::ReleaseCapture();
    setclicked(0);

    return 0;
}

/**
 * @brief WM_MOUSEMOVE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(point of mouse cursor)
 *
 * �w�b�_�\���E�B���h�E��WM_MOUSEMOVE�C�x���g���������A
 * �}�E�X���N���b�N����Ă���Ƃ��͕�����I���������s���B
 */
LRESULT PanelWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    size_t j;
    RECT mrect;
    RECT crect;

    if (clicked() && DragDetect(hwnd(), clickpoint())) {
        // �h���b�O������
        mrect.left = (MAKEPOINTS(lParam).x < clickpoint().x) ?
                         MAKEPOINTS(lParam).x : clickpoint().x;
        mrect.right = (MAKEPOINTS(lParam).x > clickpoint().x) ?
                         MAKEPOINTS(lParam).x : clickpoint().x;
        mrect.top = (MAKEPOINTS(lParam).y < clickpoint().y) ?
                         MAKEPOINTS(lParam).y : clickpoint().y;
        mrect.bottom = (MAKEPOINTS(lParam).y > clickpoint().y) ?
                         MAKEPOINTS(lParam).y : clickpoint().y;
        crect = mrect;

        for (j = 0; j < body().length(); j++) {
            if (body(j).in_rect(mrect)) {
                if (body(j).reverse() == 0) {
                    body(j).setreverse(1);
                    addrect(crect, body(j).rect());
                }
            } else {
                if (body(j).reverse() != 0) {
                    body(j).setreverse(0);
                    addrect(crect, body(j).rect());
                }
            }
        }
        redraw(crect);
    }
    return 0;
}

/**
 * @brief WM_LBUTTONDBLCLK�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONDBLCLK�C�x���g���������A
 * �S������I����Ԃɂ���B
 */
LRESULT PanelWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    size_t j;

    for (j = 0; j < body().length(); j++) {
        body(j).setreverse(1);
    }
    redraw();

    return 0;
}

/**
 * @brief WM_NCLBUTTONDBLCLK�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_NCLBUTTONDBLCLK�C�x���g���������A
 * �E�B���h�E�̑傫���𒲐�����B
 */
LRESULT PanelWindow::OnNCLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    AdjustWindow();

    return 0;
}

/**
 * @brief WM_KILLFOCUS�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_KILLFOCUS�C�x���g���������A
 * �����̑I����Ԃ���������B
 */
LRESULT PanelWindow::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
    size_t j;

    ::ReleaseCapture();
    for (j = 0; j < body().length(); j++) {
        body(j).setreverse(0);
    }
    redraw();

    return 0;
}

/**
 * @brief WM_DESTROY�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_DESTROY�C�x���g���������A
 * �E�B���h�E�j�󎞂̏��������s����B
 */
LRESULT PanelWindow::OnDestroy(WPARAM wParam, LPARAM lParam)
{
    m_hwnd = NULL;
    return 0;
}

/**
 * @brief �N���b�v�{�[�h�փR�s�[
 * @param hwnd �R�s�[�Ώۂ̃E�B���h�E�̃n���h��
 *
 * �w�b�_�\���E�B���h�E�̃e�L�X�g���N���b�v�{�[�h�ɃR�s�[����B
 */
void PanelWindow::SetClipBoardText()
{
    size_t j, pt;
    HANDLE hText;
    size_t len;
    size_t blen;
    char *pText;
    char buf[128];

    len = body().length();
    blen = 0;
    for (j = 0; j < len; j++) {
        if (body(j).reverse() != 0) {
            blen += body(j).length();
        }
    }

    if (blen == 0) {
        return;
    }

    hText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, blen + 1);
    if (hText == NULL) {
        return;
    }

    pText = (char *) ::GlobalLock(hText);
    pt = 0;
    for (j = 0; j < len; j++) {
        if (body(j).reverse() != 0) {
            memcpy(pText + pt, body(j).str(),
                   body(j).length());
            pt += body(j).length();
        }
    }
    pText[pt] = 0;
    ::GlobalUnlock(hText);

    /* Open clipboard and clear it of it's contents */
    if (! ::OpenClipboard(hwnd())) {
        return;
    }
    EmptyClipboard();

    if (! ::SetClipboardData(CF_TEXT, hText)) {
        ::GlobalFree(hText);
    }
    ::CloseClipboard();

    strcpy(buf, "copied to clipboard");
    ::LoadString(g_hInstance, IDS_COPYDONE, buf, sizeof(buf));
    bka.SetMessageText(NULL, buf);
    return;
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
