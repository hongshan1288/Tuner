// ComUtils.cpp: implementation of the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ComUtils.h"
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
HWND	g_CallBack_Handle=NULL ;
HWND	g_waveForm_wnd, g_dataInfo_wnd ;
HDC		g_waveForm_DC, g_dataInfo_DC ;
HDC		g_waveForm_DC_mem ;
long	g_waveForm_aa, g_waveForm_bb ;
long	g_dataInfo_aa, g_dataInfo_bb ;
long	g_waveForm_backColor, g_dataInfo_backColor ;

LPBYTE	g_waveForm_pan_dc_buf ;
long	g_waveForm_pan_dc_size ;
long	g_waveForm_pan_aa, g_waveForm_pan_bb ;

short	*g_int_data ;

POINT	*g_waveForm_xy ;
long	g_waveForm_xy_len ;
long	g_waveForm_xy_nn, g_waveForm_xy_ca ;

char	g_logFile[500] ;
char	g_logFile2[500] ;
char	g_tjFile[500] ;

char	g_wavFile[500] ;

//////////////////////////////////////////////////////////////////////
void __Fire_Event( long StateNo, long StateValue ) 
{
	if ( g_CallBack_Handle!=NULL )
		SendMessage( g_CallBack_Handle, WM_USER+500, StateNo, StateValue ) ;
}
/////////////////////////////////////////////////////////////////////////////
void _Fire_Event( long StateNo, long StateValue, char *StateName, char *srcFile, long srcLine ) 
{
	__Fire_Event( StateNo, StateValue ) ;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
BOOL DoEventProc()
{
		MSG msg; 

	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
	{ 
		if ( msg.message==WM_QUIT ) 
			return FALSE; 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return ( TRUE ) ;
} 
/////////////////////////////////////////////////////////////////////////////
long FileSeek( HANDLE hf, DWORD offset, DWORD MoveMethod)
{
	DWORD	FilePos ;

	FilePos = SetFilePointer( hf, offset, 0, MoveMethod ) ;
	if ( FilePos<0 ) 
		return ( -1 ) ;
	else
		return ( FilePos ) ;
}
/////////////////////////////////////////////////////////////////////////////
long Get_FileSize( char pFile[] )
{
	HANDLE	FileHandle ;
	long	dPos ;

	FileHandle = CreateFile( pFile, 
								GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0 ) ;

	if ( FileHandle==INVALID_HANDLE_VALUE )
		return ( -1 ) ; //打不开返回

	dPos = FileSeek( FileHandle, 0, 2 ) ;
	CloseHandle( FileHandle ) ;
	return( dPos ) ;
}
/////////////////////////////////////////////////////////////////////////////
long ReadBufFromFile( char *pFile, LPVOID Buf, long pos, long size )
{
	HANDLE	FileHandle ;
	DWORD	dBytes ;

	FileHandle = CreateFile( pFile, 
								GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0 ) ;

	if ( FileHandle==INVALID_HANDLE_VALUE )
		return ( -1 ) ; //打不开返回

	FileSeek( FileHandle, pos, 0 ) ;
	ReadFile( FileHandle, (LPVOID)Buf, size, &dBytes, 0 ) ;
	CloseHandle( FileHandle ) ;
	return( dBytes ) ;
}
/////////////////////////////////////////////////////////////////////////////
long ReadBufFromFile( char *pFile, LPVOID Buf, long size )
{
	HANDLE	FileHandle ;
	DWORD	dBytes ;

	FileHandle = CreateFile( pFile, 
								GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0 ) ;

	if ( FileHandle==INVALID_HANDLE_VALUE )
		return ( -1 ) ; //打不开返回

	ReadFile( FileHandle, (LPVOID)Buf, size, &dBytes, 0 ) ;
	CloseHandle( FileHandle ) ;
	return( dBytes ) ;
}
/////////////////////////////////////////////////////////////////////////////
long WriteBufToFile( char *pFile, LPVOID Buf, long size )
{
	HANDLE	FileHandle ;
	DWORD	dBytes ;

	FileHandle = CreateFile( pFile, 
								GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								0 ) ;

	if ( FileHandle==INVALID_HANDLE_VALUE )
		return ( -1 ) ; //打不开返回

	WriteFile( FileHandle, (LPVOID)Buf, size, &dBytes, 0 ) ;
	CloseHandle( FileHandle ) ;
	return( 0 ) ;
}
/////////////////////////////////////////////////////////////////////////////
long AppendBufToFile( char *pFile, LPVOID Buf, long size )
{
	HANDLE	FileHandle ;
	DWORD	dBytes ;

	FileHandle = CreateFile( pFile, 
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0 ) ;

	if ( FileHandle==INVALID_HANDLE_VALUE )
	{
		FileHandle = CreateFile( pFile, 
									GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									0,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									0 ) ;

		if ( FileHandle==INVALID_HANDLE_VALUE )
			return ( -1 ) ; //创建也打不开则返回

	}

	FileSeek( FileHandle, 0, 2 ) ;
	WriteFile( FileHandle, (LPVOID)Buf, size, &dBytes, 0 ) ;
	CloseHandle( FileHandle ) ;
	return( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void WriteLogFile( char *logFile, char *log_ss )
{
	AppendBufToFile( logFile, log_ss, strlen(log_ss) ) ;
}
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static	char	__log_ss[5000] ;
void log_printf_ex( char *log_file, char *fmt, ... )
{
//	if ( g_print_flag==0 )
//		return ;
	if ( log_file[0]=='\0' )
		return ;
	va_list args;
	va_start(args, fmt);
	vsprintf( __log_ss, fmt, args );
	WriteLogFile( log_file, __log_ss );
	va_end( args );
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/**
GetTextMetrics，该函数把程序当前的字体信息，存放到TEXTMETRIC

函数原型：BOOL GetTextMetrics(HDC hdc, LPTEXTMETRIC lptm)；

TEXTMETRIC是在WINGDI.H中定义的型态的结构。


TEXTMETRIC是在WINGDI.H中定义的型态的结构。
TEXTMETRIC结构有20个栏位：
typedef struct tagTEXTMETRIC { // tm
LONG tmHeight; //字符高度
LONG tmAscent; //字符上部高度(基线以上)
LONG tmDescent; //字符下部高度(基线以下)
LONG tmInternalLeading, //由tmHeight定义的字符高度的顶部空间数目
LONG tmExternalLeading, //夹在两行之间的空间数目
LONG tmAveCharWidth, //平均字符宽度
LONG tmMaxCharWidth, //最宽字符的宽度
LONG tmWeight; //字体的粗细轻重程度
LONG tmOverhang, //加入某些拼接字体上的附加高度
LONG tmDigitizedAspectX, //字体设计所针对的设备水平方向
LONG tmDigitizedAspectY, //字体设计所针对的设备垂直方向
BCHAR tmFirstChar; //为字体定义的第一个字符
BCHAR tmLastChar; //为字体定义的最后一个字符
BCHAR tmDefaultChar; //字体中所没有字符的替代字符
BCHAR tmBreakChar; //用于拆字的字符
BYTE tmItalic, //字体为斜体时非零
BYTE tmUnderlined, //字体为下划线时非零
BYTE tmStruckOut, //字体被删去时非零
BYTE tmPitchAndFamily, //字体间距(低4位)和族(高4位)
BYTE tmCharSet; //字体的字符集
} TEXTMETRIC;
**/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void Rect_expand_xy( long *x1, long *y1, long *x2, long *y2, long dd )
{

	if ( dd==0 )
		return ;

	long	dx, da ;

	if ( dd==1 )
	{
		dx = dd ;
		da = 0 ;
	}
	else
	{
		dx = dd/2 ;
		da = 1 ;
		if ( (dd%2)>0 )
		{
			dx++ ;
			da=0 ;
		}
	}

	*x1 -= dx ;
	*y1 -= dx ;
	*x2 += dx+da ;
	*y2 += dx+da ;

}
////////////////////////////////////////////////////////////////////////////////
void set_poly_xy2( POINT *poly_xy, long x1, long y1, long x2, long y2, long dd )
{

	Rect_expand_xy( &x1, &y1, &x2, &y2, dd ) ;

	poly_xy[0].x = x1 ;
	poly_xy[0].y = y1 ;

	poly_xy[1].x = x2 ;
	poly_xy[1].y = y1 ;

	poly_xy[2].x = x2 ;
	poly_xy[2].y = y2 ;

	poly_xy[3].x = x1;
	poly_xy[3].y = y2 ;

	poly_xy[4].x = x1 ;
	poly_xy[4].y = y1 ;

}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
HPEN __CreatePen( long line_style, long line_width, long line_color )
{
	HPEN	hpen ;
	if ( line_style==0 )
		hpen = CreatePen( PS_SOLID, line_width, line_color ) ;
	else
	{
		LOGBRUSH	logBrush ;
		long		dd = line_width / 3 ;

		logBrush.lbStyle = PS_SOLID ;
		logBrush.lbColor = line_color ;
		logBrush.lbHatch = 0 ;

		if ( line_style==1 )
		{
/**
			line_style_dd[0] = 0xffff0000 ;
			line_style_dd[1] = 0xffff0000 ;
			line_style_dd[2] = 0xffff0000 ;
			line_style_dd[3] = 0xffff0000 ;
			hpen = ExtCreatePen( PS_USERSTYLE|PS_GEOMETRIC|PS_ENDCAP_SQUARE, line_width, &logBrush, 4, line_style_dd );
**/
			hpen = ExtCreatePen( PS_DASH | PS_GEOMETRIC | PS_ENDCAP_ROUND, line_width, &logBrush, 0, NULL );
		}
		else if ( line_style==2 )
		{
			hpen = ExtCreatePen( PS_DOT | PS_GEOMETRIC | PS_ENDCAP_ROUND, line_width, &logBrush, 0, NULL );
		}
		else
		{
			hpen = ExtCreatePen( PS_DASHDOT | PS_GEOMETRIC | PS_ENDCAP_ROUND, line_width, &logBrush, 0, NULL );
//			hpen = ExtCreatePen( PS_USERSTYLE|PS_GEOMETRIC|PS_ENDCAP_FLAT, line_width, &logBrush, 4, line_style_dd );
		}
	}
	return ( hpen ) ;
}
////////////////////////////////////////////////////////////////////////////////
void PolyLine( HDC hdc, POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  )
{

	HPEN	hpen = __CreatePen( line_style, line_width, line_color ) ;
	SetROP2( hdc, line_mode ) ; //R2_XORPEN 异或
	SelectObject( hdc, hpen );    // 指定画笔
	::Polyline( hdc, xy, xy_nn ) ;
	DeleteObject( hpen ) ;

}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Get_Font_ab( HDC hdc, long *font_aa, long *font_bb )
{
	TEXTMETRIC TextMetric;
	GetTextMetrics( hdc, &TextMetric ) ;
	*font_aa = TextMetric.tmAveCharWidth ;
	*font_bb = TextMetric.tmHeight ;
}
////////////////////////////////////////////////////////////////////////////////
void DrawBar( HDC hdc, long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode )
{

	long	rop2, bar_width ;
	HPEN	hpen, hpenOld ;
	LOGBRUSH lb ;

	lb.lbStyle = bar_style;
	lb.lbColor = bar_color;
	lb.lbHatch = 0 ;

	bar_width = 1 ;
	hpen = ExtCreatePen(PS_GEOMETRIC | bar_style | PS_ENDCAP_FLAT, bar_width, &lb, 0, NULL);
	hpenOld = (HPEN)SelectObject( hdc, hpen );    // 指定画笔
	rop2 = GetROP2( hdc ) ;
	SetROP2( hdc, bar_mode ) ;		//R2_XORPEN 异或

	long	y ;
	for ( y=y1; y<=y2; y++ )
	{
		::MoveToEx( hdc, x1, y, NULL ) ;
		::LineTo( hdc, x2, y ) ;
	}

	SelectObject( hdc, hpenOld );		// 恢复原来的画笔
	SetROP2( hdc, rop2 ) ;			// 恢复
	DeleteObject( hpen ) ;

}
////////////////////////////////////////////////////////////////////////////////
void DrawRect( HDC hdc, long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode )
{

	POINT	poly_xy[5] ;
	set_poly_xy2( poly_xy, x1, y1, x2, y2, line_width ) ;

	long	rop2 ;
	HPEN	hpen, hpenOld ;

	hpen = __CreatePen( line_style, line_width, line_color ) ; // PS_SOLID
//	hpen = __CreatePen( line_width, line_color, line_style ) ;
	hpenOld = (HPEN)SelectObject( hdc, hpen );    // 指定画笔
	rop2 = GetROP2( hdc ) ;
	SetROP2( hdc, line_mode ) ;		//R2_XORPEN 异或

	::Polyline( hdc, poly_xy, 5 ) ;

	SelectObject( hdc, hpenOld );		// 恢复原来的画笔
	SetROP2( hdc, rop2 ) ;			// 恢复
	DeleteObject( hpen ) ;
}
//////////////////////////////////////////////////////////////////////
void DrawLine( HDC hdc, long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode )
{
	HPEN	hpen = __CreatePen( line_style, line_width, line_color ) ;
	SetROP2( hdc, line_mode ) ; //R2_XORPEN 异或
	SelectObject( hdc, hpen );    // 指定画笔
	if ( x1==x2 && y1==y2 )
		::SetPixel( hdc, x1, y1, line_color ) ;
	else
	{
		::MoveToEx( hdc, x1, y1, NULL ) ;
		::LineTo( hdc, x2, y2 ) ;
	}
	DeleteObject( hpen ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long Get_Text_AB( HDC hdc, char *text, long *aa, long *bb )
{
	SIZE	a_size ;
	
	if ( GetTextExtentPoint32( hdc, text, strlen(text), &a_size ) )
	{
		*aa = a_size.cx ;
		*bb = a_size.cy ;
		return ( 1 ) ;
	}
	else
		return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void DrawText( HDC hdc, char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign )
{
	long	ta, tb ;

	Get_Text_AB( hdc, pText, &ta, &tb ) ;

	if ( text_align==0 )
		xx -= ta/2 ;
	else if ( text_align>0 )
		xx += text_align ;
	else if ( text_align<0 )
		xx -= ta - text_align ;

	if ( text_valign==0 )
		yy -= tb/2 ;
	else if ( text_valign>0 )
		yy += text_valign ;
	else if ( text_valign<0 )
		yy -= tb - text_valign ;

	SetTextAlign( hdc, TA_LEFT ) ;
	if ( bk_color==0 )
		SetBkMode( hdc, TRANSPARENT ) ;
	else
	{
		SetBkMode( hdc, OPAQUE ) ;
	}
	SetTextColor( hdc, font_color ) ;
	SetBkColor( hdc, bk_color ) ;
	::TextOut( hdc, xx, yy, pText, strlen(pText) ) ;

}
//////////////////////////////////////////////////////////////////////
void SetFont( HDC hdc, long font_height, long font_style, char *font_name )
{
	HFONT hfont = CreateFont(
							-font_height,// nHeight 负数表明直接用它实际的高度，而不是转换成字号的高度
							0,			// nWidth 
							0,			// nEscapement 
							0,			// nOrientation 
							font_style, // FW_HEAVY,	//FW_BOLD,	// nWeight 
							FALSE,		// bItalic 
							FALSE,		// bUnderline 
							0,			// cStrikeOut 
							ANSI_CHARSET, // nCharSet 
							OUT_DEFAULT_PRECIS, // nOutPrecision 
							CLIP_DEFAULT_PRECIS, // nClipPrecision 
							DEFAULT_QUALITY, // nQuality 
							DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
							font_name 
						); // lpszFac
	SelectObject( hdc, hfont ) ;
	DeleteObject( hfont ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void draw_WaveForm_proc( short *pcm_data, long pcm_len, long pcm_color, long x_start, long x_end, long y_off, double y_amp )
{
	long	i, ii, y0 ;
	double	xx, yy, yb, dx_inc ;

	y0 = g_waveForm_bb / 2 ;
	yb = y0 ;

	dx_inc = g_waveForm_aa ;
	dx_inc /= pcm_len ;

	ii = 0 ;
	xx = 0 ;
	for ( i=x_start; i<=x_end; i++ )
	{
		xx = i ;
		xx *= dx_inc ;
		g_waveForm_xy[ii].x = (long)floor(xx) ;

		yy = pcm_data[i] ;
		yy *= y_amp ;
		yy += y_off ;

		if ( yy>32700 )
			yy = 32700 ;
		if ( yy<-32700 )
			yy = -32700 ;

		yy *= yb ;
		yy /= 32768 ;

		yy += y0 ;
		yy = yb*2-yy ;

		g_waveForm_xy[ii].y = (long)floor(yy) ;

		ii++ ;

	}

	PolyLine( g_waveForm_DC, g_waveForm_xy, ii, 1, pcm_color, PS_SOLID, R2_COPYPEN ) ;
	for ( i=0; i<ii; i++ )
	{
		DrawLine( g_waveForm_DC, g_waveForm_xy[i].x, g_waveForm_xy[i].y, g_waveForm_xy[i].x, g_waveForm_xy[i].y, 1, RGB(0,250,200), PS_SOLID, R2_COPYPEN ) ;
	}

	if ( y_off!=0 )
	{
		y0 += (short)((y_off*yb)/32768) ;
		DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, 1, RGB(240,0,0), PS_SOLID, R2_COPYPEN ) ;
	}

}
//////////////////////////////////////////////////////////////////////
inline short get_waveForm_y( long y, long y0 )
{
	double	yy ;
	yy = y ;
	yy = (yy*y0/32768) ;
	y = (long)floor(yy) + y0 ;
	y = y0*2-y ;
	return ( (short)y ) ;
}
//////////////////////////////////////////////////////////////////////
long make_WaveForm_Data( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data )
{
	long	i, ii, x, y, x0, y0, min_y, max_y, a_max ;
	double	xx, dx ;

	y0 = g_waveForm_bb / 2 ;

	dx = aaa ;
	dx /= pcm_len ;

	a_max = 100000000 ;

	if ( dx<1 )
	{
		min_y = a_max ;
		max_y = -a_max ;
	}

	x0 = 0 ;
	xx = 0 ;
	x = 0 ;
	ii = 0 ;

	xy_data[ii].x = 0 ;

	for ( i=0; i<pcm_len; i++ )
	{
		y = pcm_data[i] ;

		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			xx = i*dx ;
			x = (long)floor(xx) ;

			if ( x>x0 )
			{
				if ( min_y>0 && max_y>0 )
				{
					xy_data[ii].x = x0 + xxx ;
					xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
					ii++ ;
				}
				else if ( min_y<0 && max_y<0 )
				{
					xy_data[ii].x = x0 + xxx ;
					xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
					ii++ ;
				}
				else
				{
					xy_data[ii].x = x0 + xxx ;
					xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
					ii++ ;

					xy_data[ii].x = x0 + xxx ;
					xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
					ii++ ;
				}
				min_y = a_max ;
				max_y = -a_max ;
				x0 = x ;
			}
		}
		else
		{
			xy_data[ii].x = x0 + xxx ;
			xy_data[ii].y = get_waveForm_y( y, y0 ) ;
			ii ++ ;
			xx =i*dx ;
			x0 = (long)floor(xx) ;
		}
	}
	if ( dx<1 )
	{
		if ( min_y != a_max && max_y != -a_max )
		{
			xy_data[ii].x = x0 + xxx ;
			xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
			ii++ ;
			xy_data[ii].x = x0 + xxx ;
			xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
			ii++ ;
		}
	}
	xy_data[ii].x = x0 + xxx ;
	xy_data[ii].y = get_waveForm_y( 0, y0 ) ; ;
	ii++ ;

	return ( ii ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_waveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dc_line_color )
{

	long	y0 = g_waveForm_bb / 2 ;

	PolyLine( g_waveForm_DC, xy_data, xy_nn, line_width, waveForm_color, PS_SOLID, R2_COPYPEN ) ;
	DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, line_width, dc_line_color, PS_SOLID, R2_COPYPEN ) ;

	if ( dot_show_flag>0 )
	{
		long	i, x, y, dot_color ;

		dot_color = waveForm_color | 0x040404 ;
		for ( i=0; i<xy_nn; i++ )
		{
			x = xy_data[i].x ;
			y = xy_data[i].y ;
			DrawBar( g_waveForm_DC, x-1, y-1, x+1, y+1, dot_color, PS_SOLID, R2_COPYPEN ) ;
		}
	}
	DoEventProc() ;
}
//////////////////////////////////////////////////////////////////////
void show_waveForm_xy( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dc_line_color )
{
	show_waveForm_data( dot_show_flag, xy_data, xy_nn, line_width, waveForm_color, dc_line_color ) ;
}
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long SaveBMPFile_ex( char pFileName[], long bits, long width, long height, LPVOID BmpBuf )
{

	BITMAPFILEHEADER	bmpFileHeader;
    HANDLE				bmpHandle ;
	long				aBmpDataSize ;
	BITMAPINFO			mBmpInfo ;


	mBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
//	mBmpInfo.bmiHeader.biWidth = 100 ;
//	mBmpInfo.bmiHeader.biHeight = 100 ;
	mBmpInfo.bmiHeader.biPlanes = 1 ;
	mBmpInfo.bmiHeader.biBitCount = (WORD)bits ; 
	mBmpInfo.bmiHeader.biClrImportant = 0 ;
	mBmpInfo.bmiHeader.biXPelsPerMeter = 0 ;
	mBmpInfo.bmiHeader.biYPelsPerMeter = 0 ; 
	mBmpInfo.bmiHeader.biCompression = BI_RGB ;
	mBmpInfo.bmiHeader.biClrUsed = 0 ;
	mBmpInfo.bmiHeader.biClrImportant = 0 ;

	mBmpInfo.bmiHeader.biWidth = width ;
	mBmpInfo.bmiHeader.biHeight = height ;



	bmpHandle = CreateFile( pFileName, 
								 GENERIC_WRITE,
								 FILE_SHARE_READ | FILE_SHARE_WRITE,
								 0,
								 CREATE_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,
								 0 ) ;
	if ( bmpHandle==NULL ) 
		return ( -2 ) ;

	if ( bits==24 )
		aBmpDataSize = abs(width * height) * 3 ;
	else
		aBmpDataSize = abs(width * height) * 4 ;

	bmpFileHeader.bfType = 0x4d42; //"BM"
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + aBmpDataSize ;

	DWORD	dWriteBytes, dBytes ;

	// Write BMP File Header
	dWriteBytes = sizeof(BITMAPFILEHEADER) ;
	if ( !WriteFile( bmpHandle, (LPVOID)(&bmpFileHeader), dWriteBytes, &dBytes, 0 ))
	{
		FreeHandle( bmpHandle ) ;
		return ( -3 ) ;
	}

	// Write BMP Header Info
	dWriteBytes = sizeof(BITMAPINFOHEADER) ;
	if ( !WriteFile( bmpHandle, (LPVOID)(&(mBmpInfo.bmiHeader)), dWriteBytes, &dBytes, 0 ))
	{
		FreeHandle( bmpHandle ) ;
		return ( -4 ) ;
	}

	// Write BMP Bits Data
	dWriteBytes = aBmpDataSize ;
	if ( !WriteFile( bmpHandle, (LPVOID)(BmpBuf), dWriteBytes, &dBytes, 0 ))
	{
		FreeHandle( bmpHandle ) ;
		return ( -5 ) ;
	}

    FreeHandle( bmpHandle ) ;

	return ( 0 ) ;
}
/////////////////////////////////////////////////////////////////////////////
long SaveBMPFile24( char pFileName[], long width, long height, LPVOID BmpBuf )
{
	DeleteFile( pFileName ) ;
	return ( SaveBMPFile_ex( pFileName, 24, width, height, BmpBuf ) ) ;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long SaveBMPFile32( char pFileName[], long width, long height, LPVOID BmpBuf )
{
	DeleteFile( pFileName ) ;
	return ( SaveBMPFile_ex( pFileName, 32, width, height, BmpBuf ) ) ;
}
//////////////////////////////////////////////////////////////////////
long make_pan_dc_memory_buf( HDC pan_dc, long aa, long bb )
{

	// 1.首先获得兼容的 HDC
	HDC hDC = pan_dc ;

	// 2.创建兼容的HDC
	HDC memDC = CreateCompatibleDC(hDC);

    BITMAPINFO	bitmapinfo ;
	BITMAP		BM ;

    // 3.设置位图BITMAP的参数
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapinfo.bmiHeader.biBitCount = 24 ;
    bitmapinfo.bmiHeader.biWidth = aa ;
    bitmapinfo.bmiHeader.biHeight = bb ;
    bitmapinfo.bmiHeader.biPlanes = 1 ;
    bitmapinfo.bmiHeader.biCompression=BI_RGB ;
    bitmapinfo.bmiHeader.biXPelsPerMeter=0 ;
    bitmapinfo.bmiHeader.biYPelsPerMeter=0 ;
    bitmapinfo.bmiHeader.biClrUsed=0;
    bitmapinfo.bmiHeader.biClrImportant=0;
    bitmapinfo.bmiHeader.biSizeImage = 3*aa*bb ;

    // 4.创建位图区（在内存中）
    HBITMAP memBM = ::CreateDIBSection( memDC, &(bitmapinfo), 0, NULL, 0, 0);
 
    // 5.将兼容DC与内存位图连接起来
    SelectObject(memDC, memBM) ;

    // 6.设法获取位图缓冲区地址
	GetObject( memBM, sizeof( BITMAP ), &(BM) ) ;
	g_waveForm_pan_dc_buf = (LPBYTE)BM.bmBits ;

	g_waveForm_DC_mem = memDC ;
	g_waveForm_pan_dc_size = bitmapinfo.bmiHeader.biSizeImage ;

/**
// 此为测试代码	

	memset( g_waveForm_pan_dc_buf, 0x88, bitmapinfo.bmiHeader.biSizeImage ) ;
	SelectObject( memDC, GetStockObject(GRAY_BRUSH)); 
	Rectangle( memDC, 0, 0, 50, 30 ) ;
SaveBMPFile24( "z:\\hhs-1.bmp", aa, bb, g_waveForm_pan_dc_buf ) ;
**/
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_pan_dc_memory_buf()
{
	memset( g_waveForm_pan_dc_buf, 0x00, g_waveForm_pan_dc_size ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_waveForm_area()
{
	clear_pan_dc_memory_buf() ; 
	DrawBar( g_waveForm_DC, 0, 0, g_waveForm_aa-1, g_waveForm_bb-1, g_waveForm_backColor, PS_SOLID, R2_COPYPEN ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
