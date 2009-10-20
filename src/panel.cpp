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

// ソース内グローバル

PanelWindow *Panel = NULL;

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

/**
 * @brief 文字列から1行分だけ切り出す。
 * @param buf 切り出した1行分の文字列
 * @param pt 切り出し対象の文字列ポインタへのポインタ
 * @return 切り出した1行分の文字列
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
