/* $Id: panel.h,v 1.3 2005/05/24 03:45:26 woods Exp $ */

#ifndef PANEL_H
#define PANEL_H

#include <windows.h>
#include "vec.h"
#include "rectstr.h"
#include "panelwin.h"

BOOL InitHeader(HINSTANCE hInstance, const char *szIniFile);
BOOL DestroyHeader(void);
void ShowHeader(LPCTSTR lpMailID);
void ShowWindow(void);
void HideWindow(void);
void Redraw(void);
BOOL InitHeaederInstance(HINSTANCE hInstance, HWND hParentWnd);
BOOL LoadFont(LOGFONT *lf, const char *szIni);
BOOL SaveFont(LOGFONT *lf, const char *szIni);
void SaveWindowPos(const char *szIni);
UINT LoadAdjust(const char *szIni);
void SaveAdjust(UINT pAdjust, const char *szIni);
COLORREF LoadHeaderColor(const char *szIni);
void SaveHeaderColor(COLORREF col, const char *szIni);
COLORREF LoadBodyColor(const char *szIni);
void SaveBodyColor(COLORREF col, const char *szIni);
COLORREF LoadNameColor(const char *szIni);
void SaveNameColor(COLORREF col, const char *szIni);

RECT& addrect(RECT& dst, const RECT& src);
vec<char>& getLine(vec<char>& buf, char **pt);

#endif /* PANEL_H */
