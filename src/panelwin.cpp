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

// ソース内グローバル

PanelWindow *Panel = NULL;

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

static void Redraw(void);

/**
 * @brief ヘッダ表示ウィンドウ情報の初期化
 * @param szIniFile iniファイルのパス名
 * @retval TRUE 初期化成功
 * @retval FALSE 初期化失敗
 *
 * ヘッダ表示ウィンドウ情報を初期化する。
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
 * @brief ヘッダ表示ウィンドウ情報のデストラクタ
 * @retval TRUE 破壊成功
 * @retval FALSE 破壊失敗
 *
 * ヘッダ表示ウィンドウ情報を破棄する。
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
 * @brief ヘッダ情報の表示
 * @param lpMailID : メールID
 * @return 表示ウィンドウの個数
 *
 * 指定されたメールのヘッダ情報からヘッダ表示ウィンドウを表示する。
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
        AdjustWindow();
    }
}

/**
 * @brief ヘッダ表示ウィンドウの表示
 *
 * 非アクティブ化で非表示になっているヘッダ表示ウィンドウを表示する。
 */
void ShowWindow(void)
{
    if (Panel != NULL) {
        Panel->ShowWindow();
    }
}

/**
 * @brief ヘッダ表示ウィンドウの非表示化
 *
 * メインウィンドウの非アクティブ化に伴ってヘッダ表示ウィンドウを非表示にする。
 */
void HideWindow(void)
{
    if (Panel != NULL) {
        Panel->HideWindow();
    }
}

/**
 * @brief ヘッダ表示ウィンドウクラスの登録
 * @param hInstance : Window Procedureのあるモジュールのインスタンス
 * @return ウィンドウクラスを識別するアトム
 *
 * ヘッダ表示ウィンドウのウィンドウクラスを登録する。
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
 * @brief ヘッダ表示用ウィンドウの作成
 * @param hInstance : Window Procedureのあるモジュールのインスタンス
 * @param hParentWnd : 親ウィンドウ
 * @retval TRUE 成功
 * @retval FALSE 失敗
 *
 * ヘッダ表示ウィンドウを作成する。
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
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
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
 * @brief フォント情報ロード
 * @param lf フォント情報をロードするためのLOGFONT構造体を示すポインタ
 * @param szIni iniファイルのパス名
 * @retval TRUE フォント情報ロード成功
 * @retval FALSE フォント情報ロード失敗
 *
 * フォント情報をiniファイルからロードする。
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
 * @brief フォント情報セーブ
 * @param lf フォント情報にセーブするLOGFONT構造体を示すポインタ
 * @param szIni iniファイルのパス名
 * @retval TRUE フォント情報セーブ成功
 * @retval FALSE フォント情報セーブ失敗
 *
 * フォント情報をiniファイルにセーブする。
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
 * @brief ヘッダ表示ウィンドウの再描画
 *
 * ヘッダ表示ウィンドウを全て再描画する。
 */
static void Redraw(void)
{
    Panel->redraw();
}

/**
 * @brief ヘッダ表示ウィンドウ位置のセーブ
 * @param szIni iniファイルのパス名
 *
 * ヘッダ表示ウィンドウの位置をiniファイルにセーブする。
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
 * @brief ウィンドウ自動調整情報ロード
 * @param szIni iniファイルのパス名
 *
 * ウィンドウの大きさを自動調整するかどうかの情報をロードする。
 */ 
UINT LoadAdjust(const char *szIni)
{
    UINT ret = ::GetPrivateProfileInt("BkHdPnl", "AdjustWindow", 0, szIni);

    PanelWindow::setadjust(ret);
    return ret;
}

/**
 * @brief ウィンドウ自動調整情報セーブ
 * @param pAdjust 自動調整フラグ(0:自動調整しない、それ以外:自動調整する)
 * @param szIni iniファイルのパス名
 *
 * ウィンドウの大きさを自動調整するかどうかの情報をiniファイルにセーブする。
 */ 
void SaveAdjust(UINT pAdjust, const char *szIni)
{
    char buf[32];

    PanelWindow::setadjust(pAdjust);
    sprintf(buf, "%d", pAdjust);
    ::WritePrivateProfileString("BkHdPnl", "AdjustWindow", buf, szIni);
}

/**
 * @brief RECTの範囲の合成
 * @param dst 合成対象のRECT構造体ポインタ
 * @param src 合成するRECT構造体ポインタ
 * @return 合成したRECT(dstと同じもの)
 *
 * RECTの範囲を合成し、２つのRECTがともに含まれるRECTを作成しdstに設定する。
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
