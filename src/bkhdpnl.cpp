////////////////////////////////////////////////////////////////////////////////////
// $Id: bkhdpnl.cpp,v 1.9 2005/05/12 02:51:27 woods Exp $
//
// Template file for plugin.
//
// You can modify and redistribute this file without any permission.
//
// Note:
// Create a sub folder under "PlugInSDK" folder. e.g. "PlugInSDK\MyProject\" and
// place your project files there.

static char id[] = "$Id: bkhdpnl.cpp,v 1.9 2005/05/12 02:51:27 woods Exp $";

#include <windows.h>
#include "PluginSDK/BeckyAPI.h"
#include "PluginSDK/BkCommon.h"

#include "resource.h"
#include "bkhdpnl.h"
#include "panel.h"
#include "dialog.h"
#include "hookproc.h"

#define DLLEXPORT __declspec(dllexport)

/// @brief ベンダ名
static char Vendor[] = "woods";
/// @brief バージョン情報
static char Version[] = RELEASE_VERSION;
/// @brief プラグインの説明
static char Description[] = "Show header list with other windows";

CBeckyAPI bka; // You can have only one instance in a project.

HINSTANCE g_hInstance = NULL;

char szIni[_MAX_PATH+2]; // Ini file to save your plugin settings.

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
/// @brief DLL entry point
BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	g_hInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				if (!bka.InitAPI()) {
					return FALSE;
				}
				GetModuleFileName((HINSTANCE)hModule, szIni, _MAX_PATH);
				LPSTR lpExt = strrchr(szIni, '.');
				if (lpExt) {
					strcpy(lpExt, ".ini");
				} else {
					// just in case
					strcat(szIni, ".ini");
				}
			}
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks from Becky!

////////////////////////////////////////////////////////////////////////
/// @brief Called when the program is started and the main window is created.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnStart()
{
	/*
	Since BKC_OnStart is called after Becky!'s main window is
	created, at least BKC_OnMenuInit with BKC_MENU_MAIN is called
	before BKC_OnStart. So, do not assume BKC_OnStart is called
	prior to any other callback.
	*/
	// Always return 0.

    HWND hWnd;
    HWND hMain, hTree, hList, hView;

    g_hInstance = GetModuleHandle(NULL);

    bka.GetWindowHandles(&hMain, &hTree, &hList, &hView);
    InitHeader(GetModuleHandle(NULL), szIni);
    InitHeaederInstance(GetModuleHandle(NULL), hMain);

    hWnd = ::GetFocus();
    ShowHeader(NULL);
    ::SetFocus(hWnd);

    hookMailWindow(hMain);

	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when the main window is closing.
/// @retval -1 if you don't want to quit.
/// @retval 0 if you want to quit.
DLLEXPORT int WINAPI BKC_OnExit()
{
	// Return -1 if you don't want to quit.

    SaveWindowPos(szIni);

	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when menu is intialized.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnMenuInit(HWND hWnd, HMENU hMenu, int nType)
{
	switch (nType) {
	case BKC_MENU_MAIN:
		{
			/* Sample of adding menu items
			HMENU hSubMenu = GetSubMenu(hMenu, 4);
			// Define CmdProc as "void WINAPI CmdProc(HWND, LPARAM)"
			UINT nID = bka.RegisterCommand("Information about this Command", nType,CmdProc);
			AppendMenu(hSubMenu, MF_STRING, nID, "&Menu item");
			*/
			/* If needed, you can register the command UI update callback.
			// Define CmdUIProc as "UINT WINAPI CmdUIProc(HWND, LPARAM)"
			bka.RegisterUICallback(nID, CmdUIProc);
			*/
		}
		break;
	case BKC_MENU_LISTVIEW:
		break;
	case BKC_MENU_TREEVIEW:
		break;
	case BKC_MENU_MSGVIEW:
		break;
	case BKC_MENU_MSGEDIT:
		break;
	case BKC_MENU_TASKTRAY:
		break;
	case BKC_MENU_COMPOSE:
		break;
	case BKC_MENU_COMPEDIT:
		break;
	case BKC_MENU_COMPREF:
		break;
	default:
		break;
	}
	// Always return 0.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a folder is opened.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnOpenFolder(LPCTSTR lpFolderID)
{
	// Always return 0.
    HWND hWnd;

    hWnd = ::GetFocus();
    ShowHeader(NULL);
    ::SetFocus(hWnd);

	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a mail is selected.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnOpenMail(LPCTSTR lpMailID)
{
	// Always return 0.
    HWND  hWnd;

    hWnd = ::GetFocus();
    ShowHeader(lpMailID);
    ::SetFocus(hWnd);

	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called every minute.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnEveryMinute()
{
	// Always return 0.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a compose windows is opened.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnOpenCompose(HWND hWnd, int nMode/* See COMPOSE_MODE_* in BeckyApi.h */)
{
	// Always return 0.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when the composing message is saved.
/// @retval -1 if you do not want to send it yet.
DLLEXPORT int WINAPI BKC_OnOutgoing(HWND hWnd, int nMode/* 0:SaveToOutbox, 1:SaveToDraft, 2:SaveToReminder*/) 
{
	// Return -1 if you do not want to send it yet.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a key is pressed.
/// @retval TRUE if you want to suppress subsequent command associated to this key.
DLLEXPORT int WINAPI BKC_OnKeyDispatch(HWND hWnd, int nKey/* virtual key code */, int nShift/* Shift state. 0x40=Shift, 0x20=Ctrl, 0x60=Shift+Ctrl, 0xfe=Alt*/)
{
	// Return TRUE if you want to suppress subsequent command associated to this key.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a message is retrieved and saved to a folder
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnRetrieve(LPCTSTR lpMessage/* Message source*/, LPCTSTR lpMailID/* Mail ID*/)
{
	// Always return 0.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when a message is spooled
/// @retval BKC_ONSEND_PROCESSED, if you have processed this message
/// and don't need Becky! to send it.
/// Becky! will move this message to Sent box when the sending
/// operation is done.
/// CAUTION: You are responsible for the destination of this
/// message if you return BKC_ONSEND_PROCESSED.
/// @retval BKC_ONSEND_ERROR, if you want to cancel the sending operation.
/// You are responsible for displaying an error message.
/// @retval 0 to proceed the sending operation.
DLLEXPORT int WINAPI BKC_OnSend(LPCTSTR lpMessage/* Message source */)
{
	// Return BKC_ONSEND_PROCESSED, if you have processed this message
	// and don't need Becky! to send it.
	// Becky! will move this message to Sent box when the sending
	// operation is done.
	// CAUTION: You are responsible for the destination of this
	// message if you return BKC_ONSEND_PROCESSED.

	// Return BKC_ONSEND_ERROR, if you want to cancel the sending operation.
	// You are responsible for displaying an error message.

	// Return 0 to proceed the sending operation.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when all messages are retrieved
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnFinishRetrieve(int nNumber/* Number of messages*/)
{
	// Always return 0.
	return 0;
}


////////////////////////////////////////////////////////////////////////
/// @brief Called when plug-in setup is needed.
/// @return Return nonzero if you have processed.
DLLEXPORT int WINAPI BKC_OnPlugInSetup(HWND hWnd)
{
	// Return nonzero if you have processed.
	// return 1;
    HWND  hMain, hTree, hList, hView;
    char buf[1024];
    LPCSTR lpMailID;
    UINT chk = 0;

    chk = LoadAdjust(szIni);
    ::GetPrivateProfileString("BkHdPnl", "HeaderList", "X-Mailer",
                              buf, sizeof(buf) - 1, szIni);
    DoSetting(g_hInstance, hWnd, buf, sizeof(buf), &chk);
    ::WritePrivateProfileString("BkHdPnl", "HeaderList",
                          buf, szIni);
    SaveAdjust(chk, szIni);

    DestroyHeader();
    InitHeader(GetModuleHandle(NULL), szIni);
    bka.GetWindowHandles(&hMain, &hTree, &hList, &hView);
    InitHeaederInstance(GetModuleHandle(NULL), hMain);
    lpMailID = bka.GetCurrentMail();
    ShowHeader(lpMailID);
    return 1;
}


////////////////////////////////////////////////////////////////////////
typedef struct tagBKPLUGININFO
{
	char szPlugInName[80]; // Name of the plug-in
	char szVendor[80]; // Name of the vendor
	char szVersion[80]; // Version string
	char szDescription[256]; // Short description about this plugin
} BKPLUGININFO, *LPBKPLUGININFO;

/// @brief Called when plug-in information is being retrieved.
/// @return Always return 0.
DLLEXPORT int WINAPI BKC_OnPlugInInfo(LPBKPLUGININFO lpPlugInInfo)
{
	/* You MUST specify at least szPlugInName and szVendor.
	   otherwise Becky! will silently ignore your plug-in.
	strcpy(lpPlugInInfo->szPlugInName, "Becky! PGP plug-in");
	strcpy(lpPlugInInfo->szVendor, "RimArts, Inc.");
	strcpy(lpPlugInInfo->szVersion, "1.0");
	strcpy(lpPlugInInfo->szDescription, "Enables Becky! to use PGP.");
	*/
    strcpy(lpPlugInInfo->szPlugInName, "Show HeaderList");
    strcpy(lpPlugInInfo->szVendor, Vendor);
    strcpy(lpPlugInInfo->szVersion, Version);
    strcpy(lpPlugInInfo->szDescription, Description);

    ::LoadString(g_hInstance, IDS_PLUGINNAME,
                 lpPlugInInfo->szPlugInName, sizeof(lpPlugInInfo->szPlugInName));
    ::LoadString(g_hInstance, IDS_VENDOR,
                 lpPlugInInfo->szVendor, sizeof(lpPlugInInfo->szVendor));
    ::LoadString(g_hInstance, IDS_VERSION,
                 lpPlugInInfo->szVersion, sizeof(lpPlugInInfo->szVersion));
    ::LoadString(g_hInstance, IDS_DESCRIPTION,
                 lpPlugInInfo->szDescription, sizeof(lpPlugInInfo->szDescription));
	// Always return 0.
	return 0;
}

////////////////////////////////////////////////////////////////////////
/// @brief Called when drag and drop operation occurs.
///	@param lpTgt:	A folder ID of the target folder.
///			You can assume it is a root mailbox, if the string
///			contains only one '\' character.
///	@param lpSrc:	Either a folder ID or mail IDs. Multiple mail IDs are
///			separated by '\n' (0x0a).
///			You can assume it is a folder ID, if the string
///			doesn't contain '?' character.
///	@param nCount:	Number of items to be dropped.
///			It can be more than one, if you drop mail items.
///	@param dropEffect: Type of drag and drop operation
///			<ul><li>1: Copy</li>
///			    <li>2: Move</li>
///			    <li>4: Link (Used for filtering setup in Becky!)</li></ul>
DLLEXPORT int WINAPI BKC_OnDragDrop(LPCSTR lpTgt, LPCSTR lpSrc, int nCount, int dropEffect)
{
	/*
	lpTgt:	A folder ID of the target folder.
			You can assume it is a root mailbox, if the string
			contains only one '\' character.
	lpSrc:	Either a folder ID or mail IDs. Multiple mail IDs are
			separated by '\n' (0x0a).
			You can assume it is a folder ID, if the string
			doesn't contain '?' character.
	nCount:	Number of items to be dropped.
			It can be more than one, if you drop mail items.
	dropEffect: Type of drag and drop operation
			1: Copy
			2: Move
			4: Link (Used for filtering setup in Becky!)
	*/
	// If you want to cancel the default drag and drop action,
	// return -1;
	// Do not assume the default action (copy, move, etc.) is always
	// processed, because other plug-ins might cancel the operation.
	return 0;
}


////////////////////////////////////////////////////////////////////////
/// @brief Called when a message was retrieved and about to be filtered.
/// @retval BKC_FILTER_DEFAULT	Do nothing and apply default filtering rules.
/// @retval BKC_FILTER_PASS		Apply default filtering rules after applying the rule it returns.
/// @retval BKC_FILTER_DONE		Do not apply default rules.
/// @retval BKC_FILTER_NEXT		Request Becky! to call this callback again so that another rules can be added.
DLLEXPORT int WINAPI BKC_OnBeforeFilter(LPCSTR lpMessage, int* lpnAction, char** lppParam)
{
	/*
    lpMessage: A complete message source, which ends with
    "<CRLF>.<CRLF>".
    lpnAction:	[out] Returns the filtering action to be applied.
    	#define ACTION_NOTHING		-1	// Do nothing
		#define ACTION_MOVEFOLDER	0	// Move to a folder
		#define ACTION_COLORLABEL	1	// Set the color label
		#define ACTION_SETFLAG		2	// Set the flag
		#define ACTION_SOUND		3	// Make a sound
		#define ACTION_RUNEXE		4	// Run executable file
		#define ACTION_REPLY		5	// Reply to the message
		#define ACTION_FORWARD		6	// Forward the message
		#define ACTION_LEAVESERVER	7	// Leave/delete on the server.
		#define ACTION_ADDHEADER	8	// Add "X-" header to the top of the message. (Plug-in only feature)
	lppParam:	[out] Returns the pointer to the filtering parameter string.
		ACTION_MOVEFOLDER:	Folder name.
							e.g. XXXXXXXX.mb\FolderName\
							or \FolderName\ (begin with '\') to use
							the mailbox the message belongs.
		ACTION_COLORLABEL:	Color code(BGR) in hexadecimal. e.g. 0088FF
		ACTION_SETFLAG:		"F" to set flag. "R" to set read.
		ACTION_SOUND:		Name of the sound file.
		ACTION_RUNEXE:		Command line to execute. %1 will be replaced with the path to the file that contains the entire message.
		ACTION_REPLY:		Path to the template file without extension.
								e.g. #Reply\MyReply
		ACTION_FORWARD:		Path to the template file without extension. + "*" + Address to forward.
								e.g. #Forward\MyForward*mail@address
									 *mail@address (no template)
		ACTION_LEAVESERVER:	The string consists of one or two decimals. The second decimal is optional.
							Two decimals must be separated with a space.
							First decimal	1: Delete the message from the server.
											0: Leave the message on the server.
							Second decimal	1: Do not store the message to the folder.
											0: Store the message to the folder. (default action)
							e.g. 0 (Leave the message on the server.)
								 1 1 (Delete the message on the server and do not save. (Means KILL))
		ACTION_ADDHEADER	"X-Header:data" that will be added at the top of the incoming message.
							You can specify multiple headers by separating CRLF, but each header must
							begin with "X-". e.g. "X-Plugindata1: test\r\nX-Plugindata2: test2";
	*/
	
	/* Return values
	BKC_FILTER_DEFAULT	Do nothing and apply default filtering rules.
	BKC_FILTER_PASS		Apply default filtering rules after applying the rule it returns.
	BKC_FILTER_DONE		Do not apply default rules.
	BKC_FILTER_NEXT		Request Becky! to call this callback again so that another rules can be added.
	*/
    return BKC_FILTER_DEFAULT;
}

#ifdef __cplusplus
}
#endif
