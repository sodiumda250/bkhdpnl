/* $Id: panel.h,v 1.1 2005/05/12 06:19:55 woods Exp $ */

#ifndef PANEL_H
#define PANEL_H

#include <windows.h>
#include "vec.h"
#include "rectstr.h"
#include "panelwin.h"

BOOL InitHeader(HINSTANCE hInstance, const char *szIniFile);
BOOL DestroyHeader(void);
int ShowHeader(LPCTSTR lpMailID);
void ShowWindow(void);
void HideWindow(void);
BOOL InitHeaederInstance(HINSTANCE hInstance, HWND hParentWnd);
BOOL LoadFont(LOGFONT *lf, const char *szIni);
BOOL SaveFont(LOGFONT *lf, const char *szIni);
void SaveWindowPos(const char *szIni);
UINT LoadAdjust(const char *szIni);
void SaveAdjust(UINT pAdjust, const char *szIni);

RECT& addrect(RECT& dst, const RECT& src);
vec<char>& getLine(vec<char>& buf, char **pt);

#endif /* PANEL_H */
