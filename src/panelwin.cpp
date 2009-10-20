/*
 * $Id: panelwin.cpp,v 1.15 2005/05/12 04:33:12 woods Exp $
 */

static char id[] = "$Id: panelwin.cpp,v 1.15 2005/05/12 04:33:12 woods Exp $";

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <ctype.h>
#include "resource.h"
#include "bkhdpnl.h"
#include "panelwin.h"
#include "panel.h"

// PanelWindowクラスのstaticメンバ初期化
HINSTANCE PanelWindow::c_instance = 0;
int PanelWindow::c_addx = 0;
int PanelWindow::c_addy = 0;
HFONT PanelWindow::c_hFont = 0;
UINT PanelWindow::c_adjust = 0;
TCHAR PanelWindow::szWindowClass[100] = "";
vec<PanelWindow*> PanelWindow::window;

// このコード モジュールに含まれる関数の前宣言:
extern "C" {
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

/**
 * @brief ヘッダ情報の表示
 * @param lpMailID : メールID
 *
 * 指定されたメールのヘッダ情報からヘッダ表示ウィンドウを表示する。
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

    // 文字列の幅と高さを計算します。
    HDC hdc = GetDC(hwnd());
    ::SelectObject(hdc, font());
    POINT ptr;

    ptr.x = 1;
    ptr.y = 1;
    body().setrect(hdc, ptr);
    ReleaseDC(hwnd(), hdc);
    ShowWindow();
    if (adjust() && (body().length() > 0)) {
        AdjustWindow(SWP_NOACTIVATE);
    }
}

/**
 * @brief ヘッダ表示ウィンドウクラスの登録
 * @param hInstance : Window Procedureのあるモジュールのインスタンス
 * @return ウィンドウクラスを識別するアトム
 *
 * ヘッダ表示ウィンドウのウィンドウクラスを登録する。
 */
ATOM PanelWindow::MyRegisterClass(HINSTANCE hInstance)
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
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
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
 * @brief ヘッダ表示ウィンドウメッセージハンドラ
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのウィンドウメッセージを処理する。
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
        // メニュー選択の解析:
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
        // ウィンドウの大きさ、位置をセーブする。
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
 * @brief WM_CHARイベント処理
 * @param wParam : メッセージのパラメータ(character code)
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_CHARメッセージを処理する。
 * Ctrl-Cが入力されたら指定された文字列をクリップボードにコピーする。
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
 * @brief WM_MOVE/WM_SIZEイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_MOVE/WM_SIZEメッセージを処理する。
 * 移動／サイズ変更後のウィンドウ位置範囲を記録する。
 */
LRESULT PanelWindow::OnMove(WPARAM wParam, LPARAM lParam)
{
    ::GetWindowRect(hwnd(), &(rect()));

    return 0;
}

/**
 * @brief WM_ACTIVATEイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_ACTIVATEメッセージを処理する。
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
 * @brief WM_PAINTイベント処理
 * @param wParam : メッセージのパラメータ(the device context to draw in)
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_PAINTメッセージを処理し、
 * ヘッダ表示ウィンドウを描画する。
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
                char* buf = new char[rectstr::tabsize() + 1];
                memset(buf, ' ', rectstr::tabsize());
                buf[rectstr::tabsize()] = 0;
                ::DrawText(hdc, buf,
                         rectstr::tabsize(),
                         &(body(j).rect()), DT_LEFT);
                delete[] buf;
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
 * @brief WM_LBUTTONDOWNイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_LBUTTONDOWN)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(mouse clicked point)
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONDOWNイベントを処理する。
 * マウスクリックフラグをセットし、クリック位置を記録し、選択文字列をクリアする。
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
 * @brief WM_LBUTTONUPイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(mouse clicked point)
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONUPイベントを処理する。
 * クリックフラグをリセットする。
 */
LRESULT PanelWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    ::ReleaseCapture();
    setclicked(0);

    return 0;
}

/**
 * @brief WM_MOUSEMOVEイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(point of mouse cursor)
 *
 * ヘッダ表示ウィンドウのWM_MOUSEMOVEイベントを処理し、
 * マウスがクリックされているときは文字列選択処理を行う。
 */
LRESULT PanelWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    size_t j;
    RECT mrect;
    RECT crect;

    if (clicked() && DragDetect(hwnd(), clickpoint())) {
        // ドラッグ時処理
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
 * @brief WM_LBUTTONDBLCLKイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONDBLCLKイベントを処理し、
 * 全文字を選択状態にする。
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
 * @brief WM_NCLBUTTONDBLCLKイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_NCLBUTTONDBLCLKイベントを処理し、
 * ウィンドウの大きさを調整する。
 */
LRESULT PanelWindow::OnNCLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    AdjustWindow();

    return 0;
}

/**
 * @brief WM_KILLFOCUSイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_KILLFOCUSイベントを処理し、
 * 文字の選択状態を解除する。
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
 * @brief WM_DESTROYイベント処理
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_DESTROYイベントを処理し、
 * ウィンドウ破壊時の処理を実行する。
 */
LRESULT PanelWindow::OnDestroy(WPARAM wParam, LPARAM lParam)
{
    m_hwnd = NULL;
    return 0;
}

/**
 * @brief クリップボードへコピー
 * @param hwnd コピー対象のウィンドウのハンドル
 *
 * ヘッダ表示ウィンドウのテキストをクリップボードにコピーする。
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
    int line = -1;
    for (j = 0; j < len; j++) {
        if (line == -1) {
            line = body(j).rect().top;
        } else if (line != body(j).rect().top) {
            blen += 2;
            line = body(j).rect().top;
        }
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
    line = -1;
    for (j = 0; j < len; j++) {
        if (body(j).reverse() != 0) {
            if (line == -1) {
                line = body(j).rect().top;
            } else if (line != body(j).rect().top) {
                memcpy(pText + pt, "\r\n", 2);
                pt += 2;
                line = body(j).rect().top;
            }
            if ((body(j).str()[0] != '\r') && (body(j).str()[0] != '\n')) {
                memcpy(pText + pt, body(j).str(), body(j).length());
                pt += body(j).length();
            }
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
