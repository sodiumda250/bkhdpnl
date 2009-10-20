/* $Id: panelwin.h,v 1.6 2005/05/12 04:33:12 woods Exp $ */

#ifndef PANELWIN_H
#define PANELWIN_H

#include <windows.h>
#include "vec.h"
#include "rectstr.h"

/// ウィンドウ定義構造体
class PanelWindow {
// class member
    /// モジュールインスタンス
    static HINSTANCE c_instance;

    /// ウィンドウの大きさの横の最小値(枠の大きさなど)
    static int c_addx;
    /// ウィンドウの大きさの縦の最小値(タイトルバー、枠の大きさなど)
    static int c_addy;

    /// 表示用フォントハンドラ
    static HFONT c_hFont;

    /// 表示領域調整フラグ 0:調整しない それ以外:調整する
    static UINT c_adjust;

    /// ヘッダ表示ウィンドウのWindow Class
    static TCHAR szWindowClass[100];

    /// ウィンドウ管理用
    static vec<PanelWindow*> window;

// instance member
    /// ウィンドウハンドル
    HWND m_hwnd;
    /// タイトル文字列
    vec<char> m_title;
    /// ウィンドウ内表示文字
    rectstr m_body;
    /// ウィンドウの表示領域
    RECT m_rect;
    /// 左クリック位置
    POINT m_clickpoint;
    /// マウスカーソルの位置
    POINT m_mousepoint;
    /// ドラッグ中フラグ
    int m_clicked;
    /// 表示フラグ
    int m_show;
    /// 表示ヘッダリスト
    vec< vec<char> > m_header;
public:
// class method
    /**
     * @brief クラス情報の初期化
     *
     * モジュールインスタンス情報をクラス変数に設定する。
     */
    static void initClass(HINSTANCE hInstance) {
        c_instance = hInstance;
        MyRegisterClass(hInstance);
    };
    static ATOM MyRegisterClass(HINSTANCE hInstance);
    /**
     * @brief ヘッダ表示ウィンドウクラスの登録
     * @param hInstance : Window Procedureのあるモジュールのインスタンス
     * @return ウィンドウクラスを識別するアトム
     *
     * ヘッダ表示ウィンドウのウィンドウクラスを登録する。
     */
    /** @brief ウィンドウの大きさの横の最小値(枠の大きさなど) */
    static const int addx(){return c_addx;};
    /** @brief ウィンドウの大きさの縦の最小値(タイトルバー、枠の大きさなど) */
    static const int addy(){return c_addy;};

    /**
     * @brief ウィンドウハンドルからウィンドウインスタンスを求める
     * @param hwnd ウィンドウハンドル
     */
    static PanelWindow& getWindow(const HWND hwnd) {
        for (vec<PanelWindow>::iterator i = window.begin(); i < window.end(); i++) {
            if (window[i]->hwnd() == hwnd) {
                return *(window[i]);
            }
        }
        return *(window[0]);
    };

    /** @brief ウィンドウクラス名を取得する */
    static const char* WindowClass() { return szWindowClass;};
    /**
     * @brief ウィンドウクラス名を設定する
     * @param name ウィンドウクラス名
     */
    static void setWindowClass(char *name) {
        strncpy(szWindowClass, name, sizeof(szWindowClass));
    };

    /// @brief 設定されているフォントを削除する
    static void deleteFont(void) {
        if (c_hFont != NULL) {
            ::DeleteObject(c_hFont);
            c_hFont = NULL;
        }
    }

    /**
     * @brief フォントを設定する
     * @param f 設定するフォントハンドル
     */
    static void setFont(const HFONT f) {
        deleteFont();
        c_hFont = f;
    }
    /**
     * @brief フォントを設定する
     * @param logfont 設定するフォントのフォント構造体
     */
    static void setFont(const LOGFONT& logfont) {
        setFont(CreateFontIndirect(&logfont));
    }
    /**
     * @brief フォントを設定する
     * @param lp 設定するフォントのフォント構造体のポインタ
     */
    static void setFont(const LOGFONT* const lp) {
        setFont(CreateFontIndirect(lp));
    }
    /// @brief 設定されたフォントハンドルを返却する
    static const HFONT font(void) { return c_hFont;};

    /// @brief ウィンドウサイズ自動調整有無を返却する
    static UINT adjust(){return c_adjust;};
    /**
     * @brief ウィンドウサイズ自動調整有無を設定する
     * @param pAdjust 自動調整の有無
     */
    static void setadjust(const UINT pAdjust) {c_adjust = pAdjust;};

    /**
     * @brief ウィンドウの大きさの最小値を設定する
     * @param x 横の最小値(枠の大きさなど)
     * @param y 横の最小値(タイトルバー、枠の大きさなど)
     */
    static void setadd(const int x, const int y) {
        c_addx = x;
        c_addy = y;
    }

// instance method
    // メッセージハンドラ
    LRESULT OnChar(WPARAM wParam, LPARAM lParam);
    LRESULT OnMove(WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
    LRESULT OnActivate(WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
    LRESULT OnNCLButtonDblClk(WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
    LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);

    /// @brief ウィンドウハンドルを返却する
    HWND hwnd(void){return m_hwnd;};
    /// @brief ウィンドウハンドルを設定する
    void sethwnd(const HWND hWnd){m_hwnd = hWnd;};
    /// @brief ウィンドウのタイトルを返却する
    const char* title(void){return m_title.c_str();};
    /// @brief ウィンドウのタイトルを設定する
    void settitle(const char *str) {m_title = str;};
    /// @brief ウィンドウの表示文字列インスタンスを返却する
    rectstr& body(void){return m_body;};

    /// @brief 表示ヘッダのリストを返却する
    vec< vec<char> >& header() {return m_header;};
    /// @brief 表示ヘッダのリストのn番目のヘッダを返却する
    vec<char>& header(int index) {return m_header[index];};

    /**
     * @brief ウィンドウの表示文字インスタンスを返却する
     * @param index 文字のインデックス
     */
    rectchar& body(const int index){return m_body[index];};
    /// @brief ウィンドウ表示位置を返却する
    RECT& rect(void){return m_rect;};
    /// @brief マウスクリックされた位置を返却する
    POINT& clickpoint(void){return m_clickpoint;};
    /// @brief マウスクリックされた位置を保存する
    void setclickpoint(const POINTS& p) {
        m_clickpoint.x = p.x;
        m_clickpoint.y = p.y;
    };
    /// @brief マウスが移動したポイントを返却する
    const POINT& mousepoint(void){return m_mousepoint;};
    /// @brief クリック中かどうかを返却する
    int clicked(void){return m_clicked;};
    /// @brief クリック中かどうかを設定する
    void setclicked(const int f){m_clicked = f;};
    /// @brief ウィンドウを表示するかどうかの情報を返却する
    int show(void){return m_show;};
    /// @brief ウィンドウを表示するかどうかの情報を設定する
    void setshow(const int f){m_show = f;};
    /// @brief 初期化を行う
    void init(void) {
        m_hwnd = NULL;
        m_rect.top = 0;
        m_rect.left = 0;
        m_rect.bottom = 200;
        m_rect.right = 50;
        memset(&(m_clickpoint), 0, sizeof(m_clickpoint));
        m_clicked = 0;
        m_title = "Header List";
    };
    /// @brief 解放を行う
    void free(void) {
        if (m_hwnd != NULL) {
            ::DestroyWindow(m_hwnd);
            m_hwnd = NULL;
        }
    }
    /**
     * @brief コンストラクタ
     *
     * クラス変数にインスタンスを登録する
     */
    PanelWindow(void) {
        PanelWindow* p = this;
        window.add(p);
        init();
    };
    /**
     * @brief デストラクタ
     *
     * クラス変数からインスタンスを削除する
     */
    ~PanelWindow(void) {
        PanelWindow* p = this;
        free();
        window.del(p);
    };

    /// @brief ウィンドウの作成
    HWND createWindow(HWND hParentWnd) {
        m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW,
                        PanelWindow::WindowClass(),
                        title(),
                        WS_POPUP | WS_BORDER | WS_CAPTION | WS_THICKFRAME,
                        rect().left,
                        rect().top,
                        rect().right - rect().left,
                        rect().bottom - rect().top,
                        hParentWnd, NULL, c_instance, NULL);
        return m_hwnd;
    };

    /// @brief 再描画を行う
    void redraw(void) {
        ::InvalidateRect(m_hwnd, NULL, TRUE);
        ::UpdateWindow(m_hwnd);
    };
    /**
     * @brief 指定範囲の再描画を行う
     * @param rect 再描画範囲
     */
    void redraw(RECT& rect) {
        ::InvalidateRect(m_hwnd, &rect, FALSE);
        ::UpdateWindow(m_hwnd);
    }

    /**
     * @brief ヘッダ表示ウィンドウのサイズ調整
     *
     * 指定されたウィンドウの表示に必要な大きさを計算して、
     * ウィンドウの大きさを調整する。
     */
    void AdjustWindow(UINT uFlags = 0) {
        const int len = body().length();

        PanelWindow::setadd(
               ::GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1,
               ::GetSystemMetrics(SM_CYSMCAPTION) +
               ::GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1
        );

        rect().right = rect().left +
                          body().rect().right + PanelWindow::addx();
        rect().bottom = rect().top +
                           body().rect().bottom + PanelWindow::addy();

        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     ((len == 0) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW) | uFlags);
        redraw();
    };

    /**
     * @brief 指定されたメールの内容でウィンドウを表示する
     * @param lpMailID メールID
     *
     * 指定されたメールIDのメールヘッダ情報をウィンドウに設定して表示する。
     */
    void ShowHeader(LPCTSTR lpMailID);

    /**
     * @brief ウィンドウの再表示
     *
     * 非表示になっているヘッダ表示ウィンドウを再表示する。
     */
    void ShowWindow(void) {
        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     ((body().length() == 0) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW) |
                         SWP_NOACTIVATE);
        redraw();
    };

    /**
     * @brief ウィンドウの非表示化
     *
     * ウィンドウを非表示にする。
     */
    void HideWindow(void)
    {
        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     SWP_HIDEWINDOW | SWP_NOACTIVATE);
    };

    void SetClipBoardText(void);
};

#endif /* PANELWIN_H */
