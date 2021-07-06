// DoDrawUtils.cpp: implementation of the DoDrawUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DoDrawUtils.h"
#include "ComUtils.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool	m_DCFlag, m_MemFlag ;
long	m_LineMode, m_FlatVV ;
short	*m_pcm_data ;
long	m_pcm_len ;
//////////////////////////////////////////////////////////////////////
void do_DrawText( char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign )
{
	if ( m_DCFlag )
		DrawText( g_waveForm_DC, pText, xx, yy, font_color, bk_color, text_align, text_valign ) ;
	if ( m_MemFlag )
		DrawText( g_waveForm_DC_mem, pText, xx, yy, font_color, bk_color, text_align, text_valign ) ;
}
//////////////////////////////////////////////////////////////////////
void do_DrawLine( long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode )
{
	if ( m_DCFlag )
		DrawLine( g_waveForm_DC, x1, y1, x2, y2, line_width, line_color, line_style, line_mode ) ;
	if ( m_MemFlag )
		DrawLine( g_waveForm_DC_mem, x1, y1, x2, y2, line_width, line_color, line_style, line_mode ) ;
}
//////////////////////////////////////////////////////////////////////
void do_Polyline( POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  )
{
	if ( m_DCFlag )
		PolyLine( g_waveForm_DC, xy, xy_nn, line_width, line_color, line_style, line_mode ) ;
	if ( m_MemFlag )
		PolyLine( g_waveForm_DC_mem, xy, xy_nn, line_width, line_color, line_style, line_mode ) ;
}
//////////////////////////////////////////////////////////////////////
void do_DrawBar( long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode )
{
	if ( m_DCFlag )
		DrawBar( g_waveForm_DC, x1, y1, x2, y2, bar_color, bar_style, bar_mode ) ;
	if ( m_MemFlag )
		DrawBar( g_waveForm_DC_mem, x1, y1, x2, y2, bar_color, bar_style, bar_mode ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long draw_VLine( double xx, long y1, long y2, long xoff, long fx, long show_color, char *sa )
{
	long	x, y00 ;
	double	dx ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= m_pcm_len ;

	x = (long)floor(xx*dx) ;
	x += xoff ;
	y1 = get_waveForm_y( y1, y00 ) ;
	y2 = get_waveForm_y( y2, y00 ) ;

	do_DrawLine( x, y1, x, y2, 1, show_color, m_LineMode, R2_COPYPEN ) ;

	if ( sa[0] != '\0' )
		do_DrawText( sa, x, y2, show_color, g_waveForm_backColor, 10, fx ) ;

	return ( x ) ;
}
//////////////////////////////////////////////////////////////////////
void draw_VerticalLine( long ii, long xoff, long fx, long show_color, char *sa )
{
	draw_VLine( (double)(ii-1), m_pcm_data[ii], -32000, xoff, fx, show_color, sa ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
double get_xx( long i, long y0, long yy )
{
	double	kk, xx ;

	if ( yy==0 )
		xx = i ;
	else if ( y0==0 )
		xx = i-1 ;
	else
	{
		kk = -(yy-y0) ;
		kk = 1 / kk ;
		xx = kk*y0 ;
		xx += i-1 ;
	}
	return ( xx ) ;
}
//////////////////////////////////////////////////////////////////////
short get_yy( double xx )
{
	double	kk, yy, x1, x2, y1, y2 ;
	long	ix ;

	ix = (long)xx ;
	x1 = ix ;
	x2 = ix+1 ;

	y1 = m_pcm_data[ix] ;
	y2 = m_pcm_data[ix+1] ;

	if ( y1==y2 )
		yy = y1 ;
	else if ( fabs(xx-x1)<0.0001 )
		yy = y1 ;
	else
	{
		kk = (y2-y1) ;
		yy = kk*(xx-x1)+y1 ;
	}
	return ( (short)yy ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
inline long get_sign( long yy )
{
	if ( yy>0 )
		return ( 1 ) ;
	else if ( yy<0 )
		return ( -1 ) ;
	else
		return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
