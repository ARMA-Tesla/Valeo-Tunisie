/************************************************************************/
/*                                                                      */
/*  This example shows how you can change the attributes of a non-CVI   */
/*  including making the window a child of a CVI window                 */
/*                                                                      */
/* This must be in a OBJ for the RunStateChangeCallback to work properly*/
/************************************************************************/

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/

#include <windows.h>
#include <cvirte.h>   
#include <userint.h>  
#include <libsupp.h>  // for RunStateChangeCallback
#include <toolbox.h>  // for errChk()
#include <utility.h>  // for Beep()
#include "hwndutil.h"

/*----------------------------------------------------------------------------*/
/* defines                                                                    */
/*----------------------------------------------------------------------------*/
#define MAXBYTES 256
#define HWNDUTIL_TYPENAME "HWNDUTIL"

#if defined(_CVI_) && (_CVI_DEBUG_==1)
//    #error Module should be compiled without debugging to work properly in LW/CVI IDE. 
#endif


/*----------------------------------------------------------------------------*/
/* static variables                                                           */
/*----------------------------------------------------------------------------*/
typedef struct hwndInfo {
    HWND childHwnd                  ;
    LONG DefaultParentWndProc       ;
    LONG DefaultChildWndProc        ;
    int parentCallback              ;
    int renamed                     ;
    int resizable[2]                ;
    int movable[2]                  ;
    int titlebarVisible[2]          ;
    int floating[2]                 ;
    int parent[2]                   ;
    HWND parentHwnd[2]              ;
    int disabled[2]                 ;
    int sysmenu[2]                  ;
    char windowName[2][MAXBYTES]    ;
} hwndInfoRec;

static ListType sHWNDInfoList = NULL;
static SaveRetVal = -1;
/*----------------------------------------------------------------------------*/
/* Prototypes                                                                 */
/*----------------------------------------------------------------------------*/
static void CVICALLBACK DisplayMsg(unsigned int message, char * prebuf);
static void CVICALLBACK DisplayStyle(HWND hwnd); 
static int SetHWNDWindowParent(hwndInfoRec * hwndInfoPtr, int panelHandle);


/*----------------------------------------------------------------------------*/
/* ReAdoptChildWindow                                                         */
/*----------------------------------------------------------------------------*/
void CVICALLBACK ReAdoptChildWindow (void *callbackData)
{
    // Reset the panel to be a child of the parent
    SetHWNDWindowParent((hwndInfoRec *)callbackData, ((hwndInfoRec *)callbackData)->parent[1]);
    
    // Show the child window
    ShowWindow(((hwndInfoRec *)callbackData)->childHwnd, SW_SHOW);

}

/*----------------------------------------------------------------------------*/
/* ChainedParentWndProc                                                       */
/*----------------------------------------------------------------------------*/
static LRESULT CALLBACK ChainedParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT (CALLBACK *funcPtr) (HWND, UINT, WPARAM, LPARAM );
    LRESULT retval;
    int totalItems;
    int item, panelExists;
    hwndInfoRec * hwndInfoPtr = NULL;
    int BOLE;

    // Find hwndInfo in list 
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->parentHwnd[1]) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    /* Leave if not found */
    if (!hwndInfoPtr) 
        return( DefWindowProc( hwnd, message, wParam, lParam ) );

    funcPtr = (void*) hwndInfoPtr->DefaultParentWndProc;
    
    // DisplayMsg(message, "Parent:");
    
    switch(message) {
        case WM_DESTROY:
            // If parent is not valid remove chld from it, otherwise CVI 
            // is in the middle of destoying and recreating the window.  
            // We must setup an deferred call to reconnect the windows
            BOLE = SetBreakOnLibraryErrors (0);
            panelExists = (GetPanelAttribute (hwndInfoPtr->parent[1], ATTR_SIZABLE, &item)==0);
            SetBreakOnLibraryErrors (BOLE);
            
            if (!panelExists)
                HWNDUTIL_RemoveWindow(hwndInfoPtr->childHwnd);
            else 
            {
                // Disconnect child, hide, and reconnect later
                SetHWNDWindowParent(hwndInfoPtr, hwndInfoPtr->parent[0]);
                ShowWindow(hwndInfoPtr->childHwnd, SW_HIDE);
                PostDeferredCallToThread (ReAdoptChildWindow, (void *) hwndInfoPtr, CurrThreadId ());
            }    
                
            if (funcPtr)
                 return( (*funcPtr) ( hwnd, message, wParam, lParam ) );
            else return( DefWindowProc( hwnd, message, wParam, lParam ) );
                
            break;
        default:
            if (funcPtr)
                 return( (*funcPtr) ( hwnd, message, wParam, lParam ) );
            else return( DefWindowProc( hwnd, message, wParam, lParam ) );
            break;
    }
    return(0L);
}

#if CHAIN_PARENT_CALLBACK
/*----------------------------------------------------------------------------*/
/* ChainedCVIPanelCallback                                                    */
/*----------------------------------------------------------------------------*/
static int CVICALLBACK ChainedCVIPanelCallback (int panel, int event, void *callbackData,
                                              int eventData1, int eventData2)
{
    HWND hwnd = callbackData;
    
    switch (event) {
        case EVENT_DISCARD:
            if (hwnd)
            {
                HWNDUTIL_RemoveWindow(hwnd);
            }    
            break;
    }
    return 0;
}
#endif

/*----------------------------------------------------------------------------*/
/* ChainedChildWndProc                                                        */
/*----------------------------------------------------------------------------*/
static LRESULT CALLBACK ChainedChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT (CALLBACK *funcPtr) (HWND, UINT, WPARAM, LPARAM );
    LRESULT retval;
    int totalItems;
    int item;
    hwndInfoRec * hwndInfoPtr = NULL;

    // Find hwndInfo in list 
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->childHwnd) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    /* Leave if not found */
    if (!hwndInfoPtr) 
        return( DefWindowProc( hwnd, message, wParam, lParam ) );

    funcPtr = (void*) hwndInfoPtr->DefaultChildWndProc;
    
    // DisplayMsg(message, "Child: ");
    
    switch(message) {
        case WM_NCHITTEST:
            if (funcPtr)
                 retval = (*funcPtr) ( hwnd, message, wParam, lParam );
            else retval = DefWindowProc( hwnd, message, wParam, lParam );
            
            if ((!hwndInfoPtr->movable[1]) && (!hwndInfoPtr->resizable[1]) && (retval==HTCAPTION))
                 return HTBORDER;
            else return retval;    
        
            break;
            
        case WM_WINDOWPOSCHANGING:
            if (funcPtr) {
                if (lParam) {
                    if (!hwndInfoPtr->movable[1])
                        (*((LPWINDOWPOS)lParam)).flags = (*((LPWINDOWPOS)lParam)).flags | SWP_NOMOVE;
                }    
                return( (*funcPtr) ( hwnd, message, wParam, lParam ) );
            } else return( DefWindowProc( hwnd, message, wParam, lParam ) );
            break;
            
        default:
            if (funcPtr)
                 return( (*funcPtr) ( hwnd, message, wParam, lParam ) );
            else return( DefWindowProc( hwnd, message, wParam, lParam ) );
            break;
    }
    return(0L);
}

#ifdef _NI_mswin32_
/*----------------------------------------------------------------------------*/
/* InstallParentWndProc                                                       */
/*----------------------------------------------------------------------------*/
static int InstallParentWndProc(hwndInfoRec *hwndInfoPtr, int enable)
{
    LONG currentWndProc = 0;
    int status = TRUE;
    
    // Get current WndProc
    currentWndProc = GetWindowLong(hwndInfoPtr->parentHwnd[1], GWL_WNDPROC);
    if (currentWndProc == FALSE) goto Error;
    
    // Disable    
    if ((!enable) && (currentWndProc==(LONG)ChainedParentWndProc)) {
        status = SetWindowLong(hwndInfoPtr->parentHwnd[1], GWL_WNDPROC, (LONG)hwndInfoPtr->DefaultParentWndProc);
        if (status == FALSE) goto Error;
        hwndInfoPtr->DefaultParentWndProc = 0;
    }    
    // Enable
    if ((enable) && (currentWndProc!=(LONG)ChainedParentWndProc)) {
        hwndInfoPtr->DefaultParentWndProc = currentWndProc;
        status = SetWindowLong(hwndInfoPtr->parentHwnd[1], GWL_WNDPROC, (LONG)ChainedParentWndProc);
        
        if (status == FALSE) goto Error;
    }    
    
    return 0;
Error:
    status = GetLastError();
    return status;

}
#endif

#ifdef _NI_mswin32_
/*----------------------------------------------------------------------------*/
/* InstallChildWndProc                                                          */
/*----------------------------------------------------------------------------*/
static int InstallChildWndProc(hwndInfoRec *hwndInfoPtr, int enable)
{
    LONG currentWndProc = 0;
    int status = TRUE;
    
    // Get current WndProc
    currentWndProc = GetWindowLong(hwndInfoPtr->childHwnd, GWL_WNDPROC);
    if (currentWndProc == FALSE) goto Error;
    
    if ((!enable) && (currentWndProc==(LONG)ChainedChildWndProc)) {
        status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_WNDPROC, (LONG)hwndInfoPtr->DefaultChildWndProc);
        if (status == FALSE) goto Error;
        hwndInfoPtr->DefaultChildWndProc = 0;
    }    
    
    if ((enable) && (currentWndProc!=(LONG)ChainedChildWndProc)) {
        hwndInfoPtr->DefaultChildWndProc = currentWndProc;
        status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_WNDPROC, (LONG)ChainedChildWndProc);
        
        if (status == FALSE) goto Error;
    }    
    
    return 0;
Error:
    status = GetLastError();
    return status;

}
#endif


/*----------------------------------------------------------------------------*/
/* GetHWNDTitleBarVisible                                                     */
/*----------------------------------------------------------------------------*/
static int GetHWNDTitleBarVisible(hwndInfoRec * hwndInfoPtr)
{
    int status = TRUE;
    int changeValue = WS_DLGFRAME;
    
    /* Change the window's name */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    return ((status & changeValue)!=0);
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* SetHWNDTitleBarVisible                                                    */
/*----------------------------------------------------------------------------*/
static int SetHWNDTitleBarVisible(hwndInfoRec * hwndInfoPtr, int enable)
{
    int status = TRUE;
    int value;
    int changeValue = WS_DLGFRAME;
    RECT rect;
    
    /* Change the window's value */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    if (!enable)
         value = status & (~changeValue);
    else value = status | changeValue;
    
    status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE, value);
    if (status == FALSE) goto Error;
    
    // Force Window to redraw    
#ifdef _NI_mswin32_
    status = GetWindowRect(hwndInfoPtr->childHwnd, &rect);
    if (status == FALSE) goto Error;
#else
    GetWindowRect(hwndInfoPtr->childHwnd, &rect);
#endif
    status = SetWindowPos(hwndInfoPtr->childHwnd, 0, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top-1, SWP_NOZORDER | SWP_NOMOVE );    
    if (status == FALSE) goto Error;
    status = SetWindowPos(hwndInfoPtr->childHwnd, 0, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOMOVE );    
    if (status == FALSE) goto Error;
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* GetHWNDResizable                                                           */
/*----------------------------------------------------------------------------*/
static int GetHWNDResizable(hwndInfoRec * hwndInfoPtr)
{
    int status = TRUE;
    int changeValue = WS_THICKFRAME;
    
    /* Change the window's name */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    return ((status & changeValue)!=0);
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* SetHWNDResizable                                                           */
/*----------------------------------------------------------------------------*/
static int SetHWNDResizable(hwndInfoRec * hwndInfoPtr, int enable)
{
    int status = TRUE;
    int value;
    int changeValue = WS_THICKFRAME;
    
    /* Change the window's name */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    if (!enable)
         value = status & (~changeValue);
    else value = status | changeValue;
    
    status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE, value);
    if (status == FALSE) goto Error;
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* GetHWNDDisabled                                                            */
/*----------------------------------------------------------------------------*/
static int GetHWNDDisabled(hwndInfoRec * hwndInfoPtr)
{
    int status = TRUE;
    int changeValue = WS_DISABLED;
    
    /* Change the window's name */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    return ((status & changeValue)!=0);
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* SetHWNDDisabled                                                            */
/*----------------------------------------------------------------------------*/
static int SetHWNDDisabled(hwndInfoRec * hwndInfoPtr, int disable)
{
    int status = TRUE;
    int value;
    int changeValue = WS_DISABLED;
    
    /* Change the window's name */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    if (disable)
         value = status | changeValue;
    else value = status & (~changeValue);
    
    status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE, value);
    if (status == FALSE) goto Error;
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* GetHWNDSysMenuState                                                        */
/*----------------------------------------------------------------------------*/
static int GetHWNDSysMenuState(hwndInfoRec * hwndInfoPtr)
{
    int status = TRUE;
    int changeValue = WS_SYSMENU;
    
    /* Get window's style */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    return ((status & changeValue)!=0);
Error :
    return -1;
    
}


/*----------------------------------------------------------------------------*/
/* SetHWNDSysMenuState                                                        */
/*----------------------------------------------------------------------------*/
static int SetHWNDSysMenuState(hwndInfoRec * hwndInfoPtr, int enable)
{
    int status = TRUE;
    int value;
    int changeValue = WS_SYSMENU;
    RECT rect;
    
    /* Get window's style */
    status = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
    if (status == FALSE) goto Error;
    
    if (!enable)
         value = status & (~changeValue);
    else value = status | changeValue;
    
    status = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE, value);
    if (status == FALSE) goto Error;
    
    // Force Window to redraw    
#ifdef _NI_mswin32_
    status = GetWindowRect(hwndInfoPtr->childHwnd, &rect);
    if (status == FALSE) goto Error;
#else    
    GetWindowRect(hwndInfoPtr->childHwnd, &rect);
#endif    
    status = SetWindowPos(hwndInfoPtr->childHwnd, 0, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top-1, SWP_NOZORDER | SWP_NOMOVE );    
    if (status == FALSE) goto Error;
    status = SetWindowPos(hwndInfoPtr->childHwnd, 0, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOMOVE );    
    if (status == FALSE) goto Error;
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* GetHWNDWindowName                                                          */
/*----------------------------------------------------------------------------*/
static int GetHWNDWindowName(hwndInfoRec * hwndInfoPtr, char *buffer, int bufsize)
{
    return GetWindowText(hwndInfoPtr->childHwnd, buffer, bufsize);
}


/*----------------------------------------------------------------------------*/
/* SetHWNDWindowName                                                          */
/*----------------------------------------------------------------------------*/
static int SetHWNDWindowName(hwndInfoRec * hwndInfoPtr, char *newWindowName)
{
    int status = TRUE;
    
    if (!newWindowName)    
        newWindowName = hwndInfoPtr->windowName[0];
        
    /* Change the window's name */
#ifdef _NI_mswin32_
    status = SetWindowText(
#else
    SetWindowText(
#endif        
        hwndInfoPtr->childHwnd,          /* handle of window or control */
        newWindowName               /* address of string           */
        );
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* GetHWNDWindowFloatingState                                                 */
/*----------------------------------------------------------------------------*/
static int GetHWNDWindowFloatingState(hwndInfoRec * hwndInfoPtr)
{
    return hwndInfoPtr->floating[1];
}


/*----------------------------------------------------------------------------*/
/* SetHWNDWindowFloatingState                                                 */
/*----------------------------------------------------------------------------*/
static int SetHWNDWindowFloatingState(hwndInfoRec * hwndInfoPtr, int newFloatingState)
{
    int status = TRUE;

    /* Change the window's name */
#ifdef _NI_mswin32_
    status = SetWindowPos(
#else
    SetWindowPos(
#endif        
        hwndInfoPtr->childHwnd,  /* handle of window */
        (newFloatingState)?HWND_TOPMOST:HWND_NOTOPMOST, /* placement-order handle */
        0,  /* horizontal position */
        0,  /* vertical position   */
        100,    /* width */
        100,    /* height*/
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER    /* window-positioning flags */
       );        
       
    if (status == FALSE) goto Error;
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/ 
/* GetHWNDWindowParent                                                    */
/*----------------------------------------------------------------------------*/
static int GetHWNDWindowParent(hwndInfoRec * hwndInfoPtr)
{
    /* Get the window's parent */
    return hwndInfoPtr->parent[1];
}


/*----------------------------------------------------------------------------*/ 
/* SetHWNDWindowParent                                                    */
/*----------------------------------------------------------------------------*/
static int SetHWNDWindowParent(hwndInfoRec * hwndInfoPtr, int panelHandle)
{
    HWND previousParent;
    DWORD style = 0;
    int status, value;
    
    /* Get CVI Panel's Windows Handle */
    if (panelHandle!=0) {
        GetPanelAttribute (panelHandle, ATTR_SYSTEM_WINDOW_HANDLE, &value);
        if (!value) 
            goto Error;
        
        hwndInfoPtr->parentHwnd[1] = (HWND) value;
        
        // DisplayStyle(hwndInfoPtr->parentHwnd[1]);
        // DisplayStyle(hwndInfoPtr->childHwnd);
        
        previousParent = SetParent(hwndInfoPtr->childHwnd, hwndInfoPtr->parentHwnd[1]);
        hwndInfoPtr->parentHwnd[0] = previousParent;
        
        // Set WS_CHILD style 
        style = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE );
        style = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE,  style | WS_CHILD);
        
        // style = GetWindowLong(hwndInfoPtr->childHwnd, GWL_EXSTYLE );
        // style = SetWindowLong(hwndInfoPtr->childHwnd, GWL_EXSTYLE,  style | WS_EX_NOPARENTNOTIFY);
    }
    else {
        previousParent = SetParent(hwndInfoPtr->childHwnd, hwndInfoPtr->parentHwnd[0]);
                
        // Set WS_CHILD style if necessary
        style = GetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE);
        style = SetWindowLong(hwndInfoPtr->childHwnd, GWL_STYLE,  style & (~WS_CHILD));
        
        // style = GetWindowLong(hwndInfoPtr->childHwnd, GWL_EXSTYLE );
        // style = SetWindowLong(hwndInfoPtr->childHwnd, GWL_EXSTYLE,  style & (~WS_EX_NOPARENTNOTIFY));
    }   
    
    return 0;
Error:
    return -1;
}


/*----------------------------------------------------------------------------*/
/* This must be in a OBJ for it to work properly                              */
/*----------------------------------------------------------------------------*/
#ifdef _CVI_
void CVICALLBACK __RunStateChangeCallback(int runState) 
{
    int error = 0;
    int item;
    int totalItems;
    hwndInfoRec * hwndInfoPtr = NULL;

    switch (runState) {
        case kRunState_Stop:
            /* Loop thru all items */
            totalItems = ListNumItems (sHWNDInfoList);
            for (item=totalItems;(item>0);item--) 
            {
                hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
                if (hwndInfoPtr->childHwnd) 
                {
                    HWNDUTIL_RemoveWindow(hwndInfoPtr->childHwnd);
                }    
            }
        break;
    }
    
Error:
    return;
}
#endif /* _CVI_ */


//-----------------------------------------------------------------------------
// HWNDUTIL_SetAttributeFromParmInfo
//-----------------------------------------------------------------------------
int HWNDUTIL_SetAttributeFromParmInfo(HWND hwnd, int attribute, va_list parmInfo)
{
    int error = 0;
    int item;
    int totalItems;
    hwndInfoRec * hwndInfoPtr = NULL;
    int intValue;
    char *stringValue;

    /* Loop to find if hwndInfo already exists in list */
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->childHwnd) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    if (!hwndInfoPtr) 
    {
        error = HWNDUTIL_ERR_BAD_HWND_HANDLE;
        goto Error;
    }    
    
    switch (attribute) {
        case HWNDUTIL_ATTR_RESIZABLE:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->resizable[1]!=intValue) {
                errChk(SetHWNDResizable(hwndInfoPtr, intValue));
                hwndInfoPtr->resizable[1]=intValue;
            }    
            break;    
#ifdef _NI_mswin32_
        case HWNDUTIL_ATTR_MOVABLE:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->movable[1]!=intValue) {
                errChk(InstallChildWndProc(hwndInfoPtr, !intValue) );
                hwndInfoPtr->movable[1]=intValue;
            }    
            break;
#endif            
        case HWNDUTIL_ATTR_TILE_BAR_VISIBLE:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->titlebarVisible[1]!=intValue) {
                errChk(SetHWNDTitleBarVisible(hwndInfoPtr, intValue));
                hwndInfoPtr->titlebarVisible[1]=intValue;
            }    
            break;    
            
        case HWNDUTIL_ATTR_DISABLED:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->disabled[1]!=intValue) {
                errChk(SetHWNDDisabled(hwndInfoPtr, intValue));
                hwndInfoPtr->disabled[1]=intValue;
            }    
            break;    
            
        case HWNDUTIL_ATTR_WINDOW_NAME:
            stringValue = va_arg(parmInfo,char *);
            errChk(SetHWNDWindowName(hwndInfoPtr, stringValue));
            strcpy(hwndInfoPtr->windowName[1], (stringValue)?stringValue:hwndInfoPtr->windowName[0]);
            
            if (stringValue)
                hwndInfoPtr->renamed=1;
            
            break;    
            
        case HWNDUTIL_ATTR_FLOATING:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->floating[1]!=intValue) {
                errChk(SetHWNDWindowFloatingState(hwndInfoPtr, intValue));
                hwndInfoPtr->floating[1]=intValue;
            }    
            break;    
            
        case HWNDUTIL_ATTR_PARENT_HANDLE:
            intValue = va_arg(parmInfo,int);
            
            if (hwndInfoPtr->parent[1]!=intValue) 
            {
                // Unchain old parent wndproc and callback if it exists
                if (hwndInfoPtr->parentCallback)
                {
                    // Unchain our parent window procedure from CVI's
                    error = InstallParentWndProc(hwndInfoPtr, 0);
                    
                    // Unchain Parent CVI Callback
#if CHAIN_PARENT_CALLBACK
                    error = UnchainPanelCallback (hwndInfoPtr->parent[1], HWNDUTIL_TYPENAME);
#endif                    
                    
                    hwndInfoPtr->parentCallback = 0;
                }  
                
                // Set new parent state
                errChk(SetHWNDWindowParent(hwndInfoPtr, intValue));
                hwndInfoPtr->parent[1]=intValue;
                
                // Chain window procedure if making it a child                
                if (hwndInfoPtr->parent[1]) 
                {
                    // Chain Parent CVI Callback
#if CHAIN_PARENT_CALLBACK
                    error = ChainPanelCallback (hwndInfoPtr->parent[1], 
                        ChainedCVIPanelCallback, hwndInfoPtr->childHwnd, HWNDUTIL_TYPENAME);
                    if (error) 
                        break;
#endif                        
                    // Chain our parent window procedure from CVI's
                    error = InstallParentWndProc(hwndInfoPtr, 1);
                    
                    hwndInfoPtr->parentCallback = 1;
                }    
            }    
            break;    
            
        case HWNDUTIL_ATTR_SYSMENU:
            intValue = va_arg(parmInfo,int);
            if (hwndInfoPtr->sysmenu[1]!=intValue) {
                errChk(SetHWNDSysMenuState(hwndInfoPtr, intValue));
                hwndInfoPtr->sysmenu[1]=intValue;
            }    
            break;    
        default:
            error = HWNDUTIL_ERR_BAD_PARAMETER;
            break;
    }        
Error:        
    return error;
}    


//-----------------------------------------------------------------------------
// HWNDUTIL_SetAttribute
//-----------------------------------------------------------------------------
int HWNDUTIL_SetAttribute(HWND hwnd, int attribute, ...)
{
    va_list parmInfo;
    int error = 0;
    
    // Find the parmInfo
    va_start(parmInfo,attribute);
    // Set the attribute given the parmInfo
    error = HWNDUTIL_SetAttributeFromParmInfo(hwnd, attribute, parmInfo);
    
    va_end(parmInfo);
    
    return error;
}


//-----------------------------------------------------------------------------
// HWNDUTIL_GetAttribute
//-----------------------------------------------------------------------------
int HWNDUTIL_GetAttribute(HWND hwnd, int attribute, void *value)
{
    int error = 0;
    int item;
    int totalItems;
    hwndInfoRec * hwndInfoPtr = NULL;
    
    if (!value)
    {
        error = HWNDUTIL_ERR_BAD_PARAMETER;
        goto Error;
    }    

    /* Loop to find if hwndInfo already exists in list */
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->childHwnd) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    if (!hwndInfoPtr) 
    {
        error = HWNDUTIL_ERR_BAD_HWND_HANDLE;
        goto Error;
    }    
    
    switch (attribute)
    {
        case HWNDUTIL_ATTR_RESIZABLE:
            *(int *)value = hwndInfoPtr->resizable[1];
            break;    
#ifdef _NI_mswin32_
        case HWNDUTIL_ATTR_MOVABLE:
            *(int *)value = hwndInfoPtr->movable[1];
            break;
#endif            
        case HWNDUTIL_ATTR_TILE_BAR_VISIBLE:
            *(int *)value = hwndInfoPtr->titlebarVisible[1];
            break;    
            
        case HWNDUTIL_ATTR_DISABLED:
            *(int *)value = hwndInfoPtr->disabled[1];
            break;    
            
        case HWNDUTIL_ATTR_WINDOW_NAME:
            strcpy((char *) value, hwndInfoPtr->windowName[1]);
            break;    
            
        case HWNDUTIL_ATTR_FLOATING:
            *(int *)value = hwndInfoPtr->floating[1];
            break;    
            
        case HWNDUTIL_ATTR_PARENT_HANDLE:
            *(int *)value = hwndInfoPtr->parent[1];
            break;    
            
        case HWNDUTIL_ATTR_SYSMENU:
            *(int *)value = hwndInfoPtr->sysmenu[1];
            break;    
            
        default:
            error = HWNDUTIL_ERR_BAD_PARAMETER;
            break;
    }        
    
Error:    
    return error;
}


//-----------------------------------------------------------------------------
// HWNDUTIL_RemoveWindow
//-----------------------------------------------------------------------------
int HWNDUTIL_RemoveWindow(HWND hwnd)
{
    int error = 0;
    int item;
    int totalItems;
    hwndInfoRec * hwndInfoPtr = NULL;
    
    if (!hwnd)
    {
        error = HWNDUTIL_ERR_BAD_HWND_HANDLE;
        goto Error;
    }    
    
    /* Loop to find if hwndInfo already exists in list */
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) 
    {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->childHwnd) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    if (!hwndInfoPtr)  
    {
        error = HWNDUTIL_ERR_BAD_HWND_HANDLE;
        goto Error;
    }
    
    // Restore window
    
    if (hwndInfoPtr->resizable[0] != hwndInfoPtr->resizable[1])
    {
        SetHWNDResizable(hwndInfoPtr, hwndInfoPtr->resizable[0]);
        hwndInfoPtr->resizable[1] = hwndInfoPtr->resizable[0];
    }    

#ifdef _NI_mswin32_
    if (hwndInfoPtr->movable[0] != hwndInfoPtr->movable[1])
    {
        InstallChildWndProc(hwndInfoPtr, 0);
        hwndInfoPtr->movable[1] = hwndInfoPtr->movable[0];
    }    
#endif            
        
    if (hwndInfoPtr->titlebarVisible[0] != hwndInfoPtr->titlebarVisible[1])
    {
        SetHWNDTitleBarVisible(hwndInfoPtr, hwndInfoPtr->titlebarVisible[0]);
        hwndInfoPtr->titlebarVisible[1] = hwndInfoPtr->titlebarVisible[0];
    }    
        
    if (hwndInfoPtr->disabled[0] != hwndInfoPtr->disabled[1])
    {
        SetHWNDDisabled(hwndInfoPtr, hwndInfoPtr->disabled[0]);
        hwndInfoPtr->disabled[1] = hwndInfoPtr->disabled[0];
    }    

    if (hwndInfoPtr->windowName[0] && hwndInfoPtr->windowName[1] && 
        strcmp(hwndInfoPtr->windowName[0], hwndInfoPtr->windowName[1]))
    {
        SetHWNDWindowName(hwndInfoPtr, hwndInfoPtr->windowName[0]);
        strcpy(hwndInfoPtr->windowName[1], hwndInfoPtr->windowName[0]);
    }    

    if (hwndInfoPtr->floating[0] != hwndInfoPtr->floating[1])
    {
        SetHWNDWindowFloatingState(hwndInfoPtr, hwndInfoPtr->floating[0]);
        hwndInfoPtr->floating[1] = hwndInfoPtr->floating[0];
    }    

    if (hwndInfoPtr->parent[0] != hwndInfoPtr->parent[1])
    {
        // Unchain panel callback
        if (hwndInfoPtr->parentCallback)
        {
            // Unchain our parent window procedure from CVI's
            error = InstallParentWndProc(hwndInfoPtr, 0);
#if CHAIN_PARENT_CALLBACK
            error = UnchainPanelCallback (hwndInfoPtr->parent[1], HWNDUTIL_TYPENAME);
#endif            
            hwndInfoPtr->parentCallback = 0;
        }    
        
        InstallChildWndProc(hwndInfoPtr, 0);
        SetHWNDWindowParent(hwndInfoPtr, hwndInfoPtr->parent[0]);
        hwndInfoPtr->parent[1] = hwndInfoPtr->parent[0];
    }    
        
    if (hwndInfoPtr->sysmenu[0] != hwndInfoPtr->sysmenu[1])
    {
        SetHWNDSysMenuState(hwndInfoPtr, hwndInfoPtr->sysmenu[0]);
        hwndInfoPtr->sysmenu[1] = hwndInfoPtr->sysmenu[0];
    }    
        
    // Delete item
    ListRemoveItem (sHWNDInfoList, NULL, item);
    hwndInfoPtr = NULL;
    
    /* Delete list of hwndInfo when empty */
    totalItems = ListNumItems (sHWNDInfoList);
    if (!totalItems) 
    {
        /* Dispose of list */
        ListDispose(sHWNDInfoList);
        sHWNDInfoList = NULL;
    }   
Error:        
    return error;
}


//-----------------------------------------------------------------------------
// HWNDUTIL_AddWindow
//-----------------------------------------------------------------------------
int HWNDUTIL_AddWindow(HWND hwnd)
{
    int error = 0;
    int item;
    int totalItems;
    hwndInfoRec * hwndInfoPtr = NULL;
    
    if (!hwnd)
    {
        error = HWNDUTIL_ERR_BAD_HWND_HANDLE;
        goto Error;
    }    
        
    /* Create list of hwndInfo if not already created */
    if (!sHWNDInfoList)
        sHWNDInfoList = ListCreate (sizeof(hwndInfoRec));
        
    if (!sHWNDInfoList) {
        error = HWNDUTIL_ERR_OUT_OF_MEMORY;
        goto Error;
    }
    
    /* Loop to find if hwndInfo already exists in list */
    totalItems = ListNumItems (sHWNDInfoList);
    for (item=1;(item<=totalItems);item++) {
        hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, item);
        if (hwnd == hwndInfoPtr->childHwnd) 
        {
            break;
        }    
        hwndInfoPtr = NULL;
    }   
    
    /* Add new hwndInfo to List if not found */
    if (!hwndInfoPtr) 
    {
        if (hwndInfoPtr = calloc(sizeof(hwndInfoRec), 1) ) 
        { 
            hwndInfoPtr->childHwnd                   = (HWND)hwnd;
            hwndInfoPtr->DefaultChildWndProc         = 0;
            
            if (!ListInsertItem (sHWNDInfoList, hwndInfoPtr, END_OF_LIST))
            {
                error = HWNDUTIL_ERR_OUT_OF_MEMORY;
                if (hwndInfoPtr) {
                    free (hwndInfoPtr);
                    hwndInfoPtr = NULL;
                }   
                goto Error;
            }
            
            if (hwndInfoPtr) {
                free (hwndInfoPtr);
                hwndInfoPtr = NULL;
            }   
            
            // Get list pointer
            hwndInfoPtr = ListGetPtrToItem (sHWNDInfoList, END_OF_LIST);
            
            // Get window attribute values
            hwndInfoPtr->renamed                = 0;
            hwndInfoPtr->resizable[0]           = hwndInfoPtr->resizable[1]        = GetHWNDResizable(hwndInfoPtr);
            hwndInfoPtr->movable[0]             = hwndInfoPtr->movable[1]          = 1;
            hwndInfoPtr->titlebarVisible[0]     = hwndInfoPtr->titlebarVisible[1]  = GetHWNDTitleBarVisible(hwndInfoPtr);
            hwndInfoPtr->floating[0]            = hwndInfoPtr->floating[1]         = GetHWNDWindowFloatingState(hwndInfoPtr);
            hwndInfoPtr->parent[0]              = hwndInfoPtr->parent[1]           = 0;
            hwndInfoPtr->disabled[0]            = hwndInfoPtr->disabled[1]         = GetHWNDDisabled(hwndInfoPtr);
            hwndInfoPtr->sysmenu[0]             = hwndInfoPtr->sysmenu[1]          = GetHWNDSysMenuState(hwndInfoPtr);
            
            GetHWNDWindowName(hwndInfoPtr, hwndInfoPtr->windowName[0], MAXBYTES);
            strcpy(hwndInfoPtr->windowName[1], hwndInfoPtr->windowName[0]);
        }       
        else 
            error = HWNDUTIL_ERR_OUT_OF_MEMORY;
    }
    
Error:
        
    return error;   
}


//----------------------------------------------------------------------------
// Debug printf routine                                                               
//----------------------------------------------------------------------------
// Define the behavior of the this source 
#define MYOUTPUTSIZE 8192
// #define MYOUTPUTFUNC(buf) printf OutputDebugString(buf)
#define MYOUTPUTFUNC(buf) printf(buf)

//----------------------------------------------------------------------------
// Debug printf routine                                                               
//----------------------------------------------------------------------------
static int CVIANSI MYPrintf(const char *buf, ...)
{
    int error;
    char msvcbuf[MYOUTPUTSIZE];
    va_list argList;
    
    va_start(argList, buf);
    error = vsprintf(msvcbuf, buf, argList);
    MYOUTPUTFUNC(msvcbuf);
    va_end(argList);
    
    return error;
}

static void CVICALLBACK DisplayMsg(UINT message, char *prefix) 
{
    unsigned uMsg = (unsigned) message; 
    MYPrintf("%s : %x : ",prefix, uMsg);
    switch (uMsg) {
    
        case 0x0000: MYPrintf("Message Received: WM_NULL                      \n"); break;
        case 0x0001: MYPrintf("Message Received: WM_CREATE                    \n"); break;
        case 0x0002: MYPrintf("Message Received: WM_DESTROY                   \n"); break;
        case 0x0003: MYPrintf("Message Received: WM_MOVE                      \n"); break;
        case 0x0005: MYPrintf("Message Received: WM_SIZE                      \n"); break;
        case 0x0006: MYPrintf("Message Received: WM_ACTIVATE                  \n"); break;
        case 0x0007: MYPrintf("Message Received: WM_SETFOCUS                  \n"); break;
        case 0x0008: MYPrintf("Message Received: WM_KILLFOCUS                 \n"); break;
        case 0x000A: MYPrintf("Message Received: WM_ENABLE                    \n"); break;
        case 0x000B: MYPrintf("Message Received: WM_SETREDRAW                 \n"); break;
        case 0x000C: MYPrintf("Message Received: WM_SETTEXT                   \n"); break;
        case 0x000D: MYPrintf("Message Received: WM_GETTEXT                   \n"); break;
        case 0x000E: MYPrintf("Message Received: WM_GETTEXTLENGTH             \n"); break;
        case 0x000F: MYPrintf("Message Received: WM_PAINT                     \n"); break;
        case 0x0010: MYPrintf("Message Received: WM_CLOSE                     \n"); break;
        case 0x0011: MYPrintf("Message Received: WM_QUERYENDSESSION           \n"); break;
        case 0x0012: MYPrintf("Message Received: WM_QUIT                      \n"); break;
        case 0x0013: MYPrintf("Message Received: WM_QUERYOPEN                 \n"); break;
        case 0x0014: MYPrintf("Message Received: WM_ERASEBKGND                \n"); break;
        case 0x0015: MYPrintf("Message Received: WM_SYSCOLORCHANGE            \n"); break;
        case 0x0016: MYPrintf("Message Received: WM_ENDSESSION                \n"); break;
        case 0x0018: MYPrintf("Message Received: WM_SHOWWINDOW                \n"); break;
        case 0x001A: MYPrintf("Message Received: WM_SETTINGCHANGE             \n"); break;
        case 0x001B: MYPrintf("Message Received: WM_DEVMODECHANGE             \n"); break;
        case 0x001C: MYPrintf("Message Received: WM_ACTIVATEAPP               \n"); break;
        case 0x001D: MYPrintf("Message Received: WM_FONTCHANGE                \n"); break;
        case 0x001E: MYPrintf("Message Received: WM_TIMECHANGE                \n"); break;
        case 0x001F: MYPrintf("Message Received: WM_CANCELMODE                \n"); break;
        case 0x0020: MYPrintf("Message Received: WM_SETCURSOR                 \n"); break;
        case 0x0021: MYPrintf("Message Received: WM_MOUSEACTIVATE             \n"); break;
        case 0x0022: MYPrintf("Message Received: WM_CHILDACTIVATE             \n"); break;
        case 0x0023: MYPrintf("Message Received: WM_QUEUESYNC                 \n"); break;
        case 0x0024: MYPrintf("Message Received: WM_GETMINMAXINFO             \n"); break;
        case 0x0026: MYPrintf("Message Received: WM_PAINTICON                 \n"); break;
        case 0x0027: MYPrintf("Message Received: WM_ICONERASEBKGND            \n"); break;
        case 0x0028: MYPrintf("Message Received: WM_NEXTDLGCTL                \n"); break;
        case 0x002A: MYPrintf("Message Received: WM_SPOOLERSTATUS             \n"); break;
        case 0x002B: MYPrintf("Message Received: WM_DRAWITEM                  \n"); break;
        case 0x002C: MYPrintf("Message Received: WM_MEASUREITEM               \n"); break;
        case 0x002D: MYPrintf("Message Received: WM_DELETEITEM                \n"); break;
        case 0x002E: MYPrintf("Message Received: WM_VKEYTOITEM                \n"); break;
        case 0x002F: MYPrintf("Message Received: WM_CHARTOITEM                \n"); break;
        case 0x0030: MYPrintf("Message Received: WM_SETFONT                   \n"); break;
        case 0x0031: MYPrintf("Message Received: WM_GETFONT                   \n"); break;
        case 0x0032: MYPrintf("Message Received: WM_SETHOTKEY                 \n"); break;
        case 0x0033: MYPrintf("Message Received: WM_GETHOTKEY                 \n"); break;
        case 0x0037: MYPrintf("Message Received: WM_QUERYDRAGICON             \n"); break;
        case 0x0039: MYPrintf("Message Received: WM_COMPAREITEM               \n"); break;
        case 0x0041: MYPrintf("Message Received: WM_COMPACTING                \n"); break;
        case 0x0044: MYPrintf("Message Received: WM_COMMNOTIFY                \n"); break;
        case 0x0046: MYPrintf("Message Received: WM_WINDOWPOSCHANGING         \n"); break;
        case 0x0047: MYPrintf("Message Received: WM_WINDOWPOSCHANGED          \n"); break;
        case 0x0048: MYPrintf("Message Received: WM_POWER                     \n"); break;
        case 0x004A: MYPrintf("Message Received: WM_COPYDATA                  \n"); break;
        case 0x004B: MYPrintf("Message Received: WM_CANCELJOURNAL             \n"); break;
        case 0x004E: MYPrintf("Message Received: WM_NOTIFY                    \n"); break;
        case 0x0050: MYPrintf("Message Received: WM_INPUTLANGCHANGEREQUEST    \n"); break;
        case 0x0051: MYPrintf("Message Received: WM_INPUTLANGCHANGE           \n"); break;
        case 0x0052: MYPrintf("Message Received: WM_TCARD                     \n"); break;
        case 0x0053: MYPrintf("Message Received: WM_HELP                      \n"); break;
        case 0x0054: MYPrintf("Message Received: WM_USERCHANGED               \n"); break;
        case 0x0055: MYPrintf("Message Received: WM_NOTIFYFORMAT              \n"); break;
        case 0x007B: MYPrintf("Message Received: WM_CONTEXTMENU               \n"); break;
        case 0x007C: MYPrintf("Message Received: WM_STYLECHANGING             \n"); break;
        case 0x007D: MYPrintf("Message Received: WM_STYLECHANGED              \n"); break;
        case 0x007E: MYPrintf("Message Received: WM_DISPLAYCHANGE             \n"); break;
        case 0x007F: MYPrintf("Message Received: WM_GETICON                   \n"); break;
        case 0x0080: MYPrintf("Message Received: WM_SETICON                   \n"); break;
        case 0x0081: MYPrintf("Message Received: WM_NCCREATE                  \n"); break;
        case 0x0082: MYPrintf("Message Received: WM_NCDESTROY                 \n"); break;
        case 0x0083: MYPrintf("Message Received: WM_NCCALCSIZE                \n"); break;
        case 0x0084: MYPrintf("Message Received: WM_NCHITTEST                 \n"); break;
        case 0x0085: MYPrintf("Message Received: WM_NCPAINT                   \n"); break;
        case 0x0086: MYPrintf("Message Received: WM_NCACTIVATE                \n"); break;
        case 0x0087: MYPrintf("Message Received: WM_GETDLGCODE                \n"); break;
        case 0x00A0: MYPrintf("Message Received: WM_NCMOUSEMOVE               \n"); break;
        case 0x00A1: MYPrintf("Message Received: WM_NCLBUTTONDOWN             \n"); break;
        case 0x00A2: MYPrintf("Message Received: WM_NCLBUTTONUP               \n"); break;
        case 0x00A3: MYPrintf("Message Received: WM_NCLBUTTONDBLCLK           \n"); break;
        case 0x00A4: MYPrintf("Message Received: WM_NCRBUTTONDOWN             \n"); break;
        case 0x00A5: MYPrintf("Message Received: WM_NCRBUTTONUP               \n"); break;
        case 0x00A6: MYPrintf("Message Received: WM_NCRBUTTONDBLCLK           \n"); break;
        case 0x00A7: MYPrintf("Message Received: WM_NCMBUTTONDOWN             \n"); break;
        case 0x00A8: MYPrintf("Message Received: WM_NCMBUTTONUP               \n"); break;
        case 0x00A9: MYPrintf("Message Received: WM_NCMBUTTONDBLCLK           \n"); break;
        case 0x0100: MYPrintf("Message Received: WM_KEYFIRST                  \n"); break;
        //case 0x0100: MYPrintf("Message Received: WM_KEYDOWN                   \n"); break;
        case 0x0101: MYPrintf("Message Received: WM_KEYUP                     \n"); break;
        case 0x0102: MYPrintf("Message Received: WM_CHAR                      \n"); break;
        case 0x0103: MYPrintf("Message Received: WM_DEADCHAR                  \n"); break;
        case 0x0104: MYPrintf("Message Received: WM_SYSKEYDOWN                \n"); break;
        case 0x0105: MYPrintf("Message Received: WM_SYSKEYUP                  \n"); break;
        case 0x0106: MYPrintf("Message Received: WM_SYSCHAR                   \n"); break;
        case 0x0107: MYPrintf("Message Received: WM_SYSDEADCHAR               \n"); break;
        case 0x0108: MYPrintf("Message Received: WM_KEYLAST                   \n"); break;
        case 0x010D: MYPrintf("Message Received: WM_IME_STARTCOMPOSITION      \n"); break;
        case 0x010E: MYPrintf("Message Received: WM_IME_ENDCOMPOSITION        \n"); break;
        case 0x010F: MYPrintf("Message Received: WM_IME_COMPOSITION           \n"); break;
        //case 0x010F: MYPrintf("Message Received: WM_IME_KEYLAST               \n"); break;
        case 0x0110: MYPrintf("Message Received: WM_INITDIALOG                \n"); break;
        case 0x0111: MYPrintf("Message Received: WM_COMMAND                   \n"); break;
        case 0x0112: MYPrintf("Message Received: WM_SYSCOMMAND                \n"); break;
        case 0x0113: MYPrintf("Message Received: WM_TIMER                     \n"); break;
        case 0x0114: MYPrintf("Message Received: WM_HSCROLL                   \n"); break;
        case 0x0115: MYPrintf("Message Received: WM_VSCROLL                   \n"); break;
        case 0x0116: MYPrintf("Message Received: WM_INITMENU                  \n"); break;
        case 0x0117: MYPrintf("Message Received: WM_INITMENUPOPUP             \n"); break;
        case 0x011F: MYPrintf("Message Received: WM_MENUSELECT                \n"); break;
        case 0x0120: MYPrintf("Message Received: WM_MENUCHAR                  \n"); break;
        case 0x0121: MYPrintf("Message Received: WM_ENTERIDLE                 \n"); break;
        case 0x0132: MYPrintf("Message Received: WM_CTLCOLORMSGBOX            \n"); break;
        case 0x0133: MYPrintf("Message Received: WM_CTLCOLOREDIT              \n"); break;
        case 0x0134: MYPrintf("Message Received: WM_CTLCOLORLISTBOX           \n"); break;
        case 0x0135: MYPrintf("Message Received: WM_CTLCOLORBTN               \n"); break;
        case 0x0136: MYPrintf("Message Received: WM_CTLCOLORDLG               \n"); break;
        case 0x0137: MYPrintf("Message Received: WM_CTLCOLORSCROLLBAR         \n"); break;
        case 0x0138: MYPrintf("Message Received: WM_CTLCOLORSTATIC            \n"); break;
        case 0x0200: MYPrintf("Message Received: WM_MOUSEFIRST                \n"); break;
        //case 0x0200: MYPrintf("Message Received: WM_MOUSEMOVE                 \n"); break;
        case 0x0201: MYPrintf("Message Received: WM_LBUTTONDOWN               \n"); break;
        case 0x0202: MYPrintf("Message Received: WM_LBUTTONUP                 \n"); break;
        case 0x0203: MYPrintf("Message Received: WM_LBUTTONDBLCLK             \n"); break;
        case 0x0204: MYPrintf("Message Received: WM_RBUTTONDOWN               \n"); break;
        case 0x0205: MYPrintf("Message Received: WM_RBUTTONUP                 \n"); break;
        case 0x0206: MYPrintf("Message Received: WM_RBUTTONDBLCLK             \n"); break;
        case 0x0207: MYPrintf("Message Received: WM_MBUTTONDOWN               \n"); break;
        case 0x0208: MYPrintf("Message Received: WM_MBUTTONUP                 \n"); break;
        case 0x0209: MYPrintf("Message Received: WM_MBUTTONDBLCLK             \n"); break;
        //case 0x0209: MYPrintf("Message Received: WM_MOUSELAST                 \n"); break;
        case 0x0210: MYPrintf("Message Received: WM_PARENTNOTIFY              \n"); break;
        case 0x0211: MYPrintf("Message Received: WM_ENTERMENULOOP             \n"); break;
        case 0x0212: MYPrintf("Message Received: WM_EXITMENULOOP              \n"); break;
        //case 11: MYPrintf("Message Received:   \n"); break;
        default:
            MYPrintf("Message Received: Unknown %x \n", uMsg); break;
     }    
}

static void CVICALLBACK DisplayStyle(HWND hwnd) 
{
    int value;
    
    MYPrintf("HWND : %8x\n", hwnd);
 
    value = GetWindowLong(hwnd, GWL_STYLE);
    
    if ((value & WS_OVERLAPPED        )== WS_OVERLAPPED      ) MYPrintf("GWL_STYLE  %x: WS_OVERLAPPED          \n", value);
    if ((value & WS_POPUP             )== WS_POPUP           ) MYPrintf("GWL_STYLE  %x: WS_POPUP               \n", value);
    if ((value & WS_CHILD             )== WS_CHILD           ) MYPrintf("GWL_STYLE  %x: WS_CHILD               \n", value);
    if ((value & WS_MINIMIZE          )== WS_MINIMIZE        ) MYPrintf("GWL_STYLE  %x: WS_MINIMIZE            \n", value);
    if ((value & WS_VISIBLE           )== WS_VISIBLE         ) MYPrintf("GWL_STYLE  %x: WS_VISIBLE             \n", value);
    if ((value & WS_DISABLED          )== WS_DISABLED        ) MYPrintf("GWL_STYLE  %x: WS_DISABLED            \n", value);
    if ((value & WS_CLIPSIBLINGS      )== WS_CLIPSIBLINGS    ) MYPrintf("GWL_STYLE  %x: WS_CLIPSIBLINGS        \n", value);
    if ((value & WS_CLIPCHILDREN      )== WS_CLIPCHILDREN    ) MYPrintf("GWL_STYLE  %x: WS_CLIPCHILDREN        \n", value);
    if ((value & WS_MAXIMIZE          )== WS_MAXIMIZE        ) MYPrintf("GWL_STYLE  %x: WS_MAXIMIZE            \n", value);
    if ((value & WS_CAPTION           )== WS_CAPTION         ) MYPrintf("GWL_STYLE  %x: WS_CAPTION             \n", value);
    if ((value & WS_BORDER            )== WS_BORDER          ) MYPrintf("GWL_STYLE  %x: WS_BORDER              \n", value);
    if ((value & WS_DLGFRAME          )== WS_DLGFRAME        ) MYPrintf("GWL_STYLE  %x: WS_DLGFRAME            \n", value);
    if ((value & WS_VSCROLL           )== WS_VSCROLL         ) MYPrintf("GWL_STYLE  %x: WS_VSCROLL             \n", value);
    if ((value & WS_HSCROLL           )== WS_HSCROLL         ) MYPrintf("GWL_STYLE  %x: WS_HSCROLL             \n", value);
    if ((value & WS_SYSMENU           )== WS_SYSMENU         ) MYPrintf("GWL_STYLE  %x: WS_SYSMENU             \n", value);
    if ((value & WS_THICKFRAME        )== WS_THICKFRAME      ) MYPrintf("GWL_STYLE  %x: WS_THICKFRAME          \n", value);
    if ((value & WS_GROUP             )== WS_GROUP           ) MYPrintf("GWL_STYLE  %x: WS_GROUP               \n", value);
    if ((value & WS_TABSTOP           )== WS_TABSTOP         ) MYPrintf("GWL_STYLE  %x: WS_TABSTOP             \n", value);
    if ((value & WS_MINIMIZEBOX       )== WS_MINIMIZEBOX     ) MYPrintf("GWL_STYLE  %x: WS_MINIMIZEBOX         \n", value);
    if ((value & WS_MAXIMIZEBOX       )== WS_MAXIMIZEBOX     ) MYPrintf("GWL_STYLE  %x: WS_MAXIMIZEBOX         \n", value);
    if ((value & WS_TILED             )== WS_TILED           ) MYPrintf("GWL_STYLE  %x: WS_TILED               \n", value);
    if ((value & WS_ICONIC            )== WS_ICONIC          ) MYPrintf("GWL_STYLE  %x: WS_ICONIC              \n", value);
    if ((value & WS_SIZEBOX           )== WS_SIZEBOX         ) MYPrintf("GWL_STYLE  %x: WS_SIZEBOX             \n", value);
    if ((value & WS_TILEDWINDOW       )== WS_TILEDWINDOW     ) MYPrintf("GWL_STYLE  %x: WS_TILEDWINDOW         \n", value);
    if ((value & WS_OVERLAPPEDWINDOW  )== WS_OVERLAPPEDWINDOW) MYPrintf("GWL_STYLE  %x: WS_OVERLAPPEDWINDOW    \n", value);
    if ((value & WS_POPUPWINDOW       )== WS_POPUPWINDOW     ) MYPrintf("GWL_STYLE  %x: WS_POPUPWINDOW         \n", value);
    if ((value & WS_CHILDWINDOW       )== WS_CHILDWINDOW     ) MYPrintf("GWL_STYLE  %x: WS_CHILDWINDOW         \n", value);

    if ((value & DS_ABSALIGN       )== DS_ABSALIGN     ) MYPrintf("GWL_EXSTYLE %x: DS_ABSALIGN      \n", value);
    if ((value & DS_SYSMODAL       )== DS_SYSMODAL     ) MYPrintf("GWL_EXSTYLE %x: DS_SYSMODAL      \n", value);
    if ((value & DS_LOCALEDIT      )== DS_LOCALEDIT    ) MYPrintf("GWL_EXSTYLE %x: DS_LOCALEDIT     \n", value);
    if ((value & DS_SETFONT        )== DS_SETFONT      ) MYPrintf("GWL_EXSTYLE %x: DS_SETFONT       \n", value);
    if ((value & DS_MODALFRAME     )== DS_MODALFRAME   ) MYPrintf("GWL_EXSTYLE %x: DS_MODALFRAME    \n", value);
    if ((value & DS_NOIDLEMSG      )== DS_NOIDLEMSG    ) MYPrintf("GWL_EXSTYLE %x: DS_NOIDLEMSG     \n", value);
    if ((value & DS_SETFOREGROUND  )== DS_SETFOREGROUND) MYPrintf("GWL_EXSTYLE %x: DS_SETFOREGROUND \n", value);
    if ((value & DS_3DLOOK         )== DS_3DLOOK       ) MYPrintf("GWL_EXSTYLE %x: DS_3DLOOK        \n", value);
    if ((value & DS_FIXEDSYS       )== DS_FIXEDSYS     ) MYPrintf("GWL_EXSTYLE %x: DS_FIXEDSYS      \n", value);
    if ((value & DS_NOFAILCREATE   )== DS_NOFAILCREATE ) MYPrintf("GWL_EXSTYLE %x: DS_NOFAILCREATE  \n", value);
    if ((value & DS_CONTROL        )== DS_CONTROL      ) MYPrintf("GWL_EXSTYLE %x: DS_CONTROL       \n", value);
    if ((value & DS_CENTER         )== DS_CENTER       ) MYPrintf("GWL_EXSTYLE %x: DS_CENTER        \n", value);
    if ((value & DS_CENTERMOUSE    )== DS_CENTERMOUSE  ) MYPrintf("GWL_EXSTYLE %x: DS_CENTERMOUSE   \n", value);
    if ((value & DS_CONTEXTHELP    )== DS_CONTEXTHELP  ) MYPrintf("GWL_EXSTYLE %x: DS_CONTEXTHELP   \n", value);

    
    value = GetWindowLong(hwnd, GWL_EXSTYLE);
    
    if ((value & WS_EX_DLGMODALFRAME   )== WS_EX_DLGMODALFRAME    ) MYPrintf("GWL_EXSTYLE %x: WS_EX_DLGMODALFRAME    \n", value);
    if ((value & WS_EX_NOPARENTNOTIFY  )== WS_EX_NOPARENTNOTIFY   ) MYPrintf("GWL_EXSTYLE %x: WS_EX_NOPARENTNOTIFY   \n", value);
    if ((value & WS_EX_TOPMOST         )== WS_EX_TOPMOST          ) MYPrintf("GWL_EXSTYLE %x: WS_EX_TOPMOST          \n", value);
    if ((value & WS_EX_ACCEPTFILES     )== WS_EX_ACCEPTFILES      ) MYPrintf("GWL_EXSTYLE %x: WS_EX_ACCEPTFILES      \n", value);
    if ((value & WS_EX_TRANSPARENT     )== WS_EX_TRANSPARENT      ) MYPrintf("GWL_EXSTYLE %x: WS_EX_TRANSPARENT      \n", value);
    if ((value & WS_EX_MDICHILD        )== WS_EX_MDICHILD         ) MYPrintf("GWL_EXSTYLE %x: WS_EX_MDICHILD         \n", value);
    if ((value & WS_EX_TOOLWINDOW      )== WS_EX_TOOLWINDOW       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_TOOLWINDOW       \n", value);
    if ((value & WS_EX_WINDOWEDGE      )== WS_EX_WINDOWEDGE       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_WINDOWEDGE       \n", value);
    if ((value & WS_EX_CLIENTEDGE      )== WS_EX_CLIENTEDGE       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_CLIENTEDGE       \n", value);
    if ((value & WS_EX_CONTEXTHELP     )== WS_EX_CONTEXTHELP      ) MYPrintf("GWL_EXSTYLE %x: WS_EX_CONTEXTHELP      \n", value);
    if ((value & WS_EX_RIGHT           )== WS_EX_RIGHT            ) MYPrintf("GWL_EXSTYLE %x: WS_EX_RIGHT            \n", value);
    if ((value & WS_EX_LEFT            )== WS_EX_LEFT             ) MYPrintf("GWL_EXSTYLE %x: WS_EX_LEFT             \n", value);
    if ((value & WS_EX_RTLREADING      )== WS_EX_RTLREADING       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_RTLREADING       \n", value);
    if ((value & WS_EX_LTRREADING      )== WS_EX_LTRREADING       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_LTRREADING       \n", value);
    if ((value & WS_EX_LEFTSCROLLBAR   )== WS_EX_LEFTSCROLLBAR    ) MYPrintf("GWL_EXSTYLE %x: WS_EX_LEFTSCROLLBAR    \n", value);
    if ((value & WS_EX_RIGHTSCROLLBAR  )== WS_EX_RIGHTSCROLLBAR   ) MYPrintf("GWL_EXSTYLE %x: WS_EX_RIGHTSCROLLBAR   \n", value);
    if ((value & WS_EX_CONTROLPARENT   )== WS_EX_CONTROLPARENT    ) MYPrintf("GWL_EXSTYLE %x: WS_EX_CONTROLPARENT    \n", value);
    if ((value & WS_EX_STATICEDGE      )== WS_EX_STATICEDGE       ) MYPrintf("GWL_EXSTYLE %x: WS_EX_STATICEDGE       \n", value);
    if ((value & WS_EX_APPWINDOW       )== WS_EX_APPWINDOW        ) MYPrintf("GWL_EXSTYLE %x: WS_EX_APPWINDOW        \n", value);
    if ((value & WS_EX_OVERLAPPEDWINDOW)== WS_EX_OVERLAPPEDWINDOW ) MYPrintf("GWL_EXSTYLE %x: WS_EX_OVERLAPPEDWINDOW \n", value);
    if ((value & WS_EX_PALETTEWINDOW   )== WS_EX_PALETTEWINDOW    ) MYPrintf("GWL_EXSTYLE %x: WS_EX_PALETTEWINDOW    \n", value);
    
}    

