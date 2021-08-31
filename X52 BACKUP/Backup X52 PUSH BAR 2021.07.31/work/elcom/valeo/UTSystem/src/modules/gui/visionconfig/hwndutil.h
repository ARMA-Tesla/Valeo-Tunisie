#include <windows.h>

/*----------------------------------------------------------------------------*/
/* Exported Functions                                                         */
/*----------------------------------------------------------------------------*/
int HWNDUTIL_RemoveWindow(HWND hwnd);
int HWNDUTIL_AddWindow(HWND hwnd);
int HWNDUTIL_SetAttribute(HWND hwnd, int attribute, ...);
int HWNDUTIL_GetAttribute(HWND hwnd, int attribute, void *value);

#define HWNDUTIL_ATTR_RESIZABLE               1 // int
#ifdef _NI_mswin32_
  #define HWNDUTIL_ATTR_MOVABLE                 2 // int
#endif  
#define HWNDUTIL_ATTR_TILE_BAR_VISIBLE        3 // int
#define HWNDUTIL_ATTR_DISABLED                4 // int       - disables entire window
#define HWNDUTIL_ATTR_WINDOW_NAME             5 // char[256]
#define HWNDUTIL_ATTR_FLOATING                6 // int
#define HWNDUTIL_ATTR_PARENT_HANDLE           7 // int       - CVI panel Handle, zero means no parent
#define HWNDUTIL_ATTR_SYSMENU                 8 // int 

#define HWNDUTIL_ERR_OUT_OF_MEMORY            -1
#define HWNDUTIL_ERR_BAD_PARAMETER            -2
#define HWNDUTIL_ERR_BAD_HWND_HANDLE          -3
