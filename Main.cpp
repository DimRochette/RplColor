// Windows Header Files:
#include <windows.h>
#include "resource.h"
#include "TChar.h"
#include "commctrl.h"

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) | ((Color << 16) & 0xff0000)
TCHAR szClassName[] = _T("RPLCOLORTEST");

HINSTANCE g_hinstance; // not very clean but usefull for a sample


HBITMAP BlitReplaceColor (HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC=NULL);
HBITMAP ReplaceColor (HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC=NULL);
HBITMAP ReplaceColor (HINSTANCE Instance,WORD ResourseID,COLORREF cOldColor,COLORREF cNewColor);

//-----------------------------------------------------------------------------------
// BlitReplaceColor ( twice slower in my tests than ReplaceColor )
//
// hBmp		:	Source Bitmap
// cOldColor:	Color to replace in hBmp
// cNewColor:	Color used for replacement
// hBmpDC	:	DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode	:	HBITMAP of the modified bitmap or NULL for errors
//
//-----------------------------------------------------------------------------------
HBITMAP BlitReplaceColor (HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC)
{
	HBITMAP RetBmp=NULL;
	if (hBmp)
	{	
		HDC BufferDC=CreateCompatibleDC(NULL);	// DC for Source Bitmap
		if (BufferDC)
		{
			HBITMAP hTmpBitmap = (HBITMAP) NULL;
			if (hBmpDC)
				if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
				{
					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}
			HGDIOBJ PreviousBufferObject=SelectObject(BufferDC,hBmp);
			// here BufferDC contains the bitmap
			
			HDC DirectDC=CreateCompatibleDC(NULL);	// DC for working		
			if (DirectDC)
			{				
				HDC MaskDC=CreateCompatibleDC(NULL);	// DC for mask
				if (MaskDC)
				{
					// Get bitmap size
					BITMAP bm;
					GetObject(hBmp, sizeof(bm), &bm);
					
					// create a BITMAPINFO with minimal initilisation for the CreateDIBSection
					BITMAPINFO RGB32BitsBITMAPINFO; 
					ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
					RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
					RGB32BitsBITMAPINFO.bmiHeader.biWidth=bm.bmWidth;
					RGB32BitsBITMAPINFO.bmiHeader.biHeight=bm.bmHeight;
					RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
					RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;
					RECT tmprect={0,0,bm.bmWidth,bm.bmHeight};							

					HBITMAP bitmapTrans=CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
					DeleteObject(SelectObject (MaskDC,bitmapTrans));
							
					SetBkColor(BufferDC,cOldColor);					
					RetBmp=CreateBitmap(bm.bmWidth,bm.bmHeight,1,32,NULL);
					HGDIOBJ PreviousObject=SelectObject (DirectDC,RetBmp);
					HBRUSH sb=CreateSolidBrush(cNewColor);
					FillRect(DirectDC,&tmprect,sb);
					DeleteObject(sb);

					BitBlt(MaskDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC,0,0,SRCCOPY);
					BitBlt(DirectDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC,0,0,SRCINVERT);
					BitBlt(DirectDC,0,0,bm.bmWidth,bm.bmHeight,MaskDC,0,0,SRCAND);
					BitBlt(DirectDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC,0,0,SRCINVERT);				
				
					DeleteObject(PreviousObject);
					DeleteObject (bitmapTrans);
					DeleteDC(MaskDC);
				}
				// clean up
				DeleteDC(DirectDC);
			}
			// BufferDC is now useless
			if (hTmpBitmap)
			{
				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}
			SelectObject(BufferDC,PreviousBufferObject);
			DeleteDC(BufferDC);
		}
	}
	return RetBmp;
}

//-----------------------------------------------------------------------------------
// ReplaceColor 
//  ( 9 times faster in my test than loading and replacing with ReplaceColor)
//
// Simple mapping of the CreateMappedBitmap which loads a bitmap from resources
// and replace one color by another.
//
// Instance		: Instance of the module where to find the resource
// ResourseID	: Id of the resource to load
// cOldColor	: Color to replace
// cNewColor	: Color used for replacement
//
// Retcode	:	HBITMAP of the modified bitmap or NULL for errors
//
//-----------------------------------------------------------------------------------
HBITMAP ReplaceColor (HINSTANCE Instance,WORD ResourseID,COLORREF cOldColor,COLORREF cNewColor)
{
	COLORMAP tabcolormap;
	tabcolormap.from=cOldColor;
	tabcolormap.to=cNewColor;
	return CreateMappedBitmap(Instance,ResourseID,0,&tabcolormap,1);
}

//-----------------------------------------------------------------------------------
// ReplaceColor
//
// Author	:	Dimitri Rochette drochette@coldcat.fr
// Specials Thanks to Joe Woodbury for his comments and code corrections
//
// Includes :	Only <windows.h>
//
// hBmp		:	Source Bitmap
// cOldColor:	Color to replace in hBmp
// cNewColor:	Color used for replacement
// hBmpDC	:	DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode	:	HBITMAP of the modified bitmap or NULL for errors
//
//-----------------------------------------------------------------------------------
HBITMAP ReplaceColor (HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC)
{
	HBITMAP RetBmp=NULL;
	if (hBmp)
	{	
		HDC BufferDC=CreateCompatibleDC(NULL);	// DC for Source Bitmap
		if (BufferDC)
		{
			HBITMAP hTmpBitmap = (HBITMAP) NULL;
			if (hBmpDC)
				if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
				{
					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}
			HGDIOBJ PreviousBufferObject=SelectObject(BufferDC,hBmp);
			// here BufferDC contains the bitmap
			
			HDC DirectDC=CreateCompatibleDC(NULL);	// DC for working		
			if (DirectDC)
			{
				// Get bitmap size
				BITMAP bm;
				GetObject(hBmp, sizeof(bm), &bm);
				
				// create a BITMAPINFO with minimal initilisation for the CreateDIBSection
				BITMAPINFO RGB32BitsBITMAPINFO; 
				ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
				RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
				RGB32BitsBITMAPINFO.bmiHeader.biWidth=bm.bmWidth;
				RGB32BitsBITMAPINFO.bmiHeader.biHeight=bm.bmHeight;
				RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
				RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;
				UINT * ptPixels;	// pointer used for direct Bitmap pixels access

				HBITMAP DirectBitmap= CreateDIBSection(DirectDC, (BITMAPINFO *)&RGB32BitsBITMAPINFO, DIB_RGB_COLORS,(void **)&ptPixels, NULL, 0);
				if (DirectBitmap)
				{
					// here DirectBitmap!=NULL so ptPixels!=NULL no need to test
					HGDIOBJ PreviousObject=SelectObject(DirectDC, DirectBitmap);
					BitBlt(DirectDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC,0,0,SRCCOPY);					
					// here the DirectDC contains the bitmap

					// Convert COLORREF to RGB (Invert RED and BLUE)
					cOldColor=COLORREF2RGB(cOldColor);
					cNewColor=COLORREF2RGB(cNewColor);

					// After all the inits we can do the job : Replace Color
					for (int i=((bm.bmWidth*bm.bmHeight)-1);i>=0;i--)
					{
						if (ptPixels[i]==cOldColor) ptPixels[i]=cNewColor;
					}
					// little clean up
					// Don't delete the result of SelectObject because it's our modified bitmap (DirectBitmap)
					SelectObject(DirectDC,PreviousObject);
					
					// finish
					RetBmp=DirectBitmap;
				}
				// clean up
				DeleteDC(DirectDC);
			}			
			if (hTmpBitmap)
			{
				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}
			SelectObject(BufferDC,PreviousBufferObject);
			// BufferDC is now useless
			DeleteDC(BufferDC);
		}
	}
	return RetBmp;
}

	/*---------------------------------------------------------------------------
	// Short Sample of the usage of ReplaceColor

	HBITMAP hBmp2 = LoadBitmap(g_hinstance,MAKEINTRESOURCE(IDB_SAMPLEBITMAP));
	HBITMAP hBmp = ReplaceColor(hBmp2,0xff0000,0x00ff00); // replace blue by green
	DeleteObject(hBmp2);

	// Use your modified Bitmap here 

	DeleteObject(hBmp);
	-----------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------------
// WndProc
//
// Sample window message loop
// This function is here only for illustrating the use of ReplaceColor
//-----------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) 
	{
		case WM_NCCREATE:
			{
			InitCommonControls();
			// We will create an ImageList and use transparency on purple
			HBITMAP hBmp2 = LoadBitmap(g_hinstance,MAKEINTRESOURCE(IDB_SAMPLEBITMAP));
			HBITMAP hBmp = ReplaceColor(hBmp2,0xff0000,0x00ff00); // replace blue by green
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);			
			HIMAGELIST ImgList=ImageList_Create(bm.bmWidth,bm.bmHeight,ILC_COLOR32|ILC_MASK,8,8);
			ImageList_Add(ImgList,hBmp2,NULL);	// original image
			ImageList_AddMasked(ImgList,hBmp,0xff00ff);		
			DeleteObject(hBmp);
			hBmp = ReplaceColor(hBmp2,0x0000ff,0x00ff00);	// replace red by green
			ImageList_AddMasked(ImgList,hBmp,0xff00ff);
			DeleteObject(hBmp);
			hBmp = ReplaceColor(hBmp2,0x000000,0x00ff00);	// replace black by green
			ImageList_AddMasked(ImgList,hBmp,0xff00ff);
			DeleteObject(hBmp);
			hBmp = ReplaceColor(hBmp2,0xff00ff,0x00ff00);	// replace purple by green 
			ImageList_AddMasked(ImgList,hBmp,0xff0000);		// use blue for mask this time
			DeleteObject(hBmp);
			DeleteObject(hBmp2);
			// save the imagelistref on GWLP_USERDATA
			SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ImgList);
			return (DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;
		case WM_PAINT:
			{				
				hdc = BeginPaint (hWnd, &ps);
				
				RECT Rect;
				GetClientRect(hWnd,&Rect);
				HGDIOBJ PreviousObj=SelectObject(hdc,CreateHatchBrush(HS_BDIAGONAL,0x0));
				PatBlt(hdc,Rect.left,Rect.top,Rect.right,Rect.bottom,PATCOPY);
				SelectObject(hdc,PreviousObj);

				HIMAGELIST ImgList=(HIMAGELIST)GetWindowLongPtr(hWnd,GWLP_USERDATA);
				TCHAR szBuffer[512];
				if (ImgList)
				{
					for (int i=0;i<5;i++)
					{
						ImageList_DrawEx(ImgList,i,hdc,i*48,0,0,0,CLR_NONE ,CLR_NONE,ILD_NORMAL );
						LoadString(g_hinstance,IDS_HELPSTRING01+i,szBuffer,(sizeof(szBuffer)/sizeof(szBuffer[0])-1));
						TextOut(hdc,0,64+i*16,szBuffer,static_cast<int>(_tcslen(szBuffer)) );
					}
				}
				EndPaint (hWnd, &ps);
			}
			break;

		case WM_KEYDOWN:
			if (wParam != VK_ESCAPE)
				break;

		case WM_DESTROY:
			{
				HIMAGELIST ImgList=(HIMAGELIST)GetWindowLongPtr(hWnd,GWLP_USERDATA);
				if (ImgList)
				{
					ImageList_Destroy(ImgList);
				}
				PostQuitMessage(0);
			}
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	
	return (0);
}


//-----------------------------------------------------------------------------------
// WinMain
//-----------------------------------------------------------------------------------
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;
	MSG msg;

	g_hinstance=hInstance;

	ZeroMemory(&wc, sizeof(wc));
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = szClassName;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.hInstance     = hInstance;
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.cbClsExtra	=	0;
	wc.cbWndExtra	=	0;

	if (RegisterClass(&wc))
	{

		hWnd = CreateWindow(szClassName, _T("Replace color"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 320, 240, NULL, NULL, hInstance, NULL);
		if (hWnd)
		{
			ShowWindow(hWnd, nCmdShow);
			UpdateWindow(hWnd);

			// Main message loop:
			while (GetMessage(&msg, NULL, 0, 0)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			return (msg.wParam!=0);
		}
	}

	return -1;
}
