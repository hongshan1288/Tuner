
#if !defined(__ComUtils)
#define __ComUtils

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#define FreeBuf( a ) { \
	if ( (a)==INVALID_HANDLE_VALUE || (a)==NULL ) \
		; \
	else \
	{ \
		GlobalFree( a ) ; \
		a=NULL ; \
	} \
}
#define FreeHandle( a ) { \
	if ( (a)==INVALID_HANDLE_VALUE || (a)==(void*)INVALID_FILE_SIZE || (a)==NULL ) \
		; \
	else \
	{ \
		CloseHandle( a ) ; \
		a=NULL ; \
	} \
}

// log_printf
#ifdef _DEBUG
	#define log_prt		log_printf_ex
#else
	#define log_prt		//
#endif

//////////////////////////////////////////////////////////////////////
extern HWND	g_CallBack_Handle ;
extern HWND	g_waveForm_wnd, g_dataInfo_wnd ;
extern HDC		g_waveForm_DC, g_dataInfo_DC ;
extern HDC		g_waveForm_DC_mem ;
extern long	g_waveForm_aa, g_waveForm_bb ;
extern long	g_dataInfo_aa, g_dataInfo_bb ;
extern long	g_waveForm_xy_nn, g_waveForm_xy_ca ;
extern long	g_waveForm_backColor, g_dataInfo_backColor ;
extern LPBYTE	g_waveForm_pan_dc_buf ;

extern short	*g_int_data ;
extern POINT	*g_waveForm_xy ;
extern long	g_waveForm_xy_len ;

extern char	g_logFile[500] ;
extern char	g_logFile2[500] ;
extern char	g_tjFile[500] ;
extern char	g_wavFile[500] ;


//////////////////////////////////////////////////////////////////////
extern void _Fire_Event( long StateNo, long StateValue, char *StateName, char *srcFile, long srcLine ) ;
#define PopEvent( sNo, sVal, sName )		_Fire_Event( (sNo), (sVal), (sName), __FILE__, __LINE__ )

extern BOOL DoEventProc() ;
//////////////////////////////////////////////////////////////////////
extern long FileSeek( HANDLE hf, DWORD offset, DWORD MoveMethod);
extern long Get_FileSize( char pFile[] );
extern long ReadBufFromFile( char *pFile, LPVOID Buf, long pos, long size );
extern long ReadBufFromFile( char *pFile, LPVOID Buf, long size );
extern long WriteBufToFile( char *pFile, LPVOID Buf, long size );
extern long AppendBufToFile( char *pFile, LPVOID Buf, long size );
extern void WriteLogFile( char *logFile, char *log_ss );
extern void log_printf_ex( char *log_file, char *fmt, ... );
//////////////////////////////////////////////////////////////////////

extern HPEN __CreatePen( long line_style, long line_width, long line_color );
extern void Get_Font_ab( HDC hdc, long *font_aa, long *font_bb );
extern void DrawBar( HDC hdc, long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode );
extern void DrawRect( HDC hdc, long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode );
extern void DrawLine( HDC hdc, long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode );
extern void PolyLine( HDC hdc, POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode ) ;
extern long Get_Text_AB( HDC hdc, char *text, long *aa, long *bb );
extern void DrawText( HDC hdc, char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign );
extern void SetFont( HDC hdc, long font_height, long font_style, char *font_name );
//////////////////////////////////////////////////////////////////////
extern inline short get_waveForm_y( long y, long y0 ) ;
extern long make_WaveForm_Data( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data ) ;
extern void show_waveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dc_line_color ) ;
extern void show_waveForm_xy( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dc_line_color ) ;

extern void draw_WaveForm_proc( short *pcm_data, long pcm_len, long pcm_color, long x_start, long x_end, long y_off, double y_amp ) ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
extern long SaveBMPFile24( char pFileName[], long width, long height, LPVOID BmpBuf ) ;
extern long SaveBMPFile32( char pFileName[], long width, long height, LPVOID BmpBuf ) ;
extern void SaveToBmpFile( char *waveFile, long nCount ) ;
//////////////////////////////////////////////////////////////////////
extern long make_pan_dc_memory_buf( HDC pan_dc, long aa, long bb ) ;
extern void clear_pan_dc_memory_buf() ;
extern void clear_waveForm_area() ;
//////////////////////////////////////////////////////////////////////


#endif // !defined(__ComUtils)
