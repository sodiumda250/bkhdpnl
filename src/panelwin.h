/* $Id: panelwin.h,v 1.6 2005/05/12 04:33:12 woods Exp $ */

#ifndef PANELWIN_H
#define PANELWIN_H

#include <windows.h>
#include "vec.h"
#include "rectstr.h"

/// �E�B���h�E��`�\����
class PanelWindow {
// class member
    /// ���W���[���C���X�^���X
    static HINSTANCE c_instance;

    /// �E�B���h�E�̑傫���̉��̍ŏ��l(�g�̑傫���Ȃ�)
    static int c_addx;
    /// �E�B���h�E�̑傫���̏c�̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�)
    static int c_addy;

    /// �\���p�t�H���g�n���h��
    static HFONT c_hFont;

    /// �\���̈撲���t���O 0:�������Ȃ� ����ȊO:��������
    static UINT c_adjust;

    /// �w�b�_�\���E�B���h�E��Window Class
    static TCHAR szWindowClass[100];

    /// �E�B���h�E�Ǘ��p
    static vec<PanelWindow*> window;

// instance member
    /// �E�B���h�E�n���h��
    HWND m_hwnd;
    /// �^�C�g��������
    vec<char> m_title;
    /// �E�B���h�E���\������
    rectstr m_body;
    /// �E�B���h�E�̕\���̈�
    RECT m_rect;
    /// ���N���b�N�ʒu
    POINT m_clickpoint;
    /// �}�E�X�J�[�\���̈ʒu
    POINT m_mousepoint;
    /// �h���b�O���t���O
    int m_clicked;
    /// �\���t���O
    int m_show;
    /// �\���w�b�_���X�g
    vec< vec<char> > m_header;
public:
// class method
    /**
     * @brief �N���X���̏�����
     *
     * ���W���[���C���X�^���X�����N���X�ϐ��ɐݒ肷��B
     */
    static void initClass(HINSTANCE hInstance) {
        c_instance = hInstance;
        MyRegisterClass(hInstance);
    };
    static ATOM MyRegisterClass(HINSTANCE hInstance);
    /**
     * @brief �w�b�_�\���E�B���h�E�N���X�̓o�^
     * @param hInstance : Window Procedure�̂��郂�W���[���̃C���X�^���X
     * @return �E�B���h�E�N���X�����ʂ���A�g��
     *
     * �w�b�_�\���E�B���h�E�̃E�B���h�E�N���X��o�^����B
     */
    /** @brief �E�B���h�E�̑傫���̉��̍ŏ��l(�g�̑傫���Ȃ�) */
    static const int addx(){return c_addx;};
    /** @brief �E�B���h�E�̑傫���̏c�̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�) */
    static const int addy(){return c_addy;};

    /**
     * @brief �E�B���h�E�n���h������E�B���h�E�C���X�^���X�����߂�
     * @param hwnd �E�B���h�E�n���h��
     */
    static PanelWindow& getWindow(const HWND hwnd) {
        for (vec<PanelWindow>::iterator i = window.begin(); i < window.end(); i++) {
            if (window[i]->hwnd() == hwnd) {
                return *(window[i]);
            }
        }
        return *(window[0]);
    };

    /** @brief �E�B���h�E�N���X�����擾���� */
    static const char* WindowClass() { return szWindowClass;};
    /**
     * @brief �E�B���h�E�N���X����ݒ肷��
     * @param name �E�B���h�E�N���X��
     */
    static void setWindowClass(char *name) {
        strncpy(szWindowClass, name, sizeof(szWindowClass));
    };

    /// @brief �ݒ肳��Ă���t�H���g���폜����
    static void deleteFont(void) {
        if (c_hFont != NULL) {
            ::DeleteObject(c_hFont);
            c_hFont = NULL;
        }
    }

    /**
     * @brief �t�H���g��ݒ肷��
     * @param f �ݒ肷��t�H���g�n���h��
     */
    static void setFont(const HFONT f) {
        deleteFont();
        c_hFont = f;
    }
    /**
     * @brief �t�H���g��ݒ肷��
     * @param logfont �ݒ肷��t�H���g�̃t�H���g�\����
     */
    static void setFont(const LOGFONT& logfont) {
        setFont(CreateFontIndirect(&logfont));
    }
    /**
     * @brief �t�H���g��ݒ肷��
     * @param lp �ݒ肷��t�H���g�̃t�H���g�\���̂̃|�C���^
     */
    static void setFont(const LOGFONT* const lp) {
        setFont(CreateFontIndirect(lp));
    }
    /// @brief �ݒ肳�ꂽ�t�H���g�n���h����ԋp����
    static const HFONT font(void) { return c_hFont;};

    /// @brief �E�B���h�E�T�C�Y���������L����ԋp����
    static UINT adjust(){return c_adjust;};
    /**
     * @brief �E�B���h�E�T�C�Y���������L����ݒ肷��
     * @param pAdjust ���������̗L��
     */
    static void setadjust(const UINT pAdjust) {c_adjust = pAdjust;};

    /**
     * @brief �E�B���h�E�̑傫���̍ŏ��l��ݒ肷��
     * @param x ���̍ŏ��l(�g�̑傫���Ȃ�)
     * @param y ���̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�)
     */
    static void setadd(const int x, const int y) {
        c_addx = x;
        c_addy = y;
    }

// instance method
    // ���b�Z�[�W�n���h��
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

    /// @brief �E�B���h�E�n���h����ԋp����
    HWND hwnd(void){return m_hwnd;};
    /// @brief �E�B���h�E�n���h����ݒ肷��
    void sethwnd(const HWND hWnd){m_hwnd = hWnd;};
    /// @brief �E�B���h�E�̃^�C�g����ԋp����
    const char* title(void){return m_title.c_str();};
    /// @brief �E�B���h�E�̃^�C�g����ݒ肷��
    void settitle(const char *str) {m_title = str;};
    /// @brief �E�B���h�E�̕\��������C���X�^���X��ԋp����
    rectstr& body(void){return m_body;};

    /// @brief �\���w�b�_�̃��X�g��ԋp����
    vec< vec<char> >& header() {return m_header;};
    /// @brief �\���w�b�_�̃��X�g��n�Ԗڂ̃w�b�_��ԋp����
    vec<char>& header(int index) {return m_header[index];};

    /**
     * @brief �E�B���h�E�̕\�������C���X�^���X��ԋp����
     * @param index �����̃C���f�b�N�X
     */
    rectchar& body(const int index){return m_body[index];};
    /// @brief �E�B���h�E�\���ʒu��ԋp����
    RECT& rect(void){return m_rect;};
    /// @brief �}�E�X�N���b�N���ꂽ�ʒu��ԋp����
    POINT& clickpoint(void){return m_clickpoint;};
    /// @brief �}�E�X�N���b�N���ꂽ�ʒu��ۑ�����
    void setclickpoint(const POINTS& p) {
        m_clickpoint.x = p.x;
        m_clickpoint.y = p.y;
    };
    /// @brief �}�E�X���ړ������|�C���g��ԋp����
    const POINT& mousepoint(void){return m_mousepoint;};
    /// @brief �N���b�N�����ǂ�����ԋp����
    int clicked(void){return m_clicked;};
    /// @brief �N���b�N�����ǂ�����ݒ肷��
    void setclicked(const int f){m_clicked = f;};
    /// @brief �E�B���h�E��\�����邩�ǂ����̏���ԋp����
    int show(void){return m_show;};
    /// @brief �E�B���h�E��\�����邩�ǂ����̏���ݒ肷��
    void setshow(const int f){m_show = f;};
    /// @brief ���������s��
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
    /// @brief ������s��
    void free(void) {
        if (m_hwnd != NULL) {
            ::DestroyWindow(m_hwnd);
            m_hwnd = NULL;
        }
    }
    /**
     * @brief �R���X�g���N�^
     *
     * �N���X�ϐ��ɃC���X�^���X��o�^����
     */
    PanelWindow(void) {
        PanelWindow* p = this;
        window.add(p);
        init();
    };
    /**
     * @brief �f�X�g���N�^
     *
     * �N���X�ϐ�����C���X�^���X���폜����
     */
    ~PanelWindow(void) {
        PanelWindow* p = this;
        free();
        window.del(p);
    };

    /// @brief �E�B���h�E�̍쐬
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

    /// @brief �ĕ`����s��
    void redraw(void) {
        ::InvalidateRect(m_hwnd, NULL, TRUE);
        ::UpdateWindow(m_hwnd);
    };
    /**
     * @brief �w��͈͂̍ĕ`����s��
     * @param rect �ĕ`��͈�
     */
    void redraw(RECT& rect) {
        ::InvalidateRect(m_hwnd, &rect, FALSE);
        ::UpdateWindow(m_hwnd);
    }

    /**
     * @brief �w�b�_�\���E�B���h�E�̃T�C�Y����
     *
     * �w�肳�ꂽ�E�B���h�E�̕\���ɕK�v�ȑ傫�����v�Z���āA
     * �E�B���h�E�̑傫���𒲐�����B
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
     * @brief �w�肳�ꂽ���[���̓��e�ŃE�B���h�E��\������
     * @param lpMailID ���[��ID
     *
     * �w�肳�ꂽ���[��ID�̃��[���w�b�_�����E�B���h�E�ɐݒ肵�ĕ\������B
     */
    void ShowHeader(LPCTSTR lpMailID);

    /**
     * @brief �E�B���h�E�̍ĕ\��
     *
     * ��\���ɂȂ��Ă���w�b�_�\���E�B���h�E���ĕ\������B
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
     * @brief �E�B���h�E�̔�\����
     *
     * �E�B���h�E���\���ɂ���B
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
