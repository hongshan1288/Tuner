// DoDrawUtils.h: interface for the DoDrawUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DoDrawUtils__INCLUDED_)
#define DoDrawUtils__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
extern bool	m_DCFlag, m_MemFlag ;
extern long	m_LineMode, m_FlatVV ;
extern short	*m_pcm_data ;
extern long	m_pcm_len ;
//////////////////////////////////////////////////////////////////////
extern void do_DrawText( char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign );
extern void do_DrawLine( long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode );
extern void do_Polyline( POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  );
extern void do_DrawBar( long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode );
//////////////////////////////////////////////////////////////////////
extern long draw_VLine( double xx, long y1, long y2, long xoff, long fx, long show_color, char *sa ) ;
extern void draw_VerticalLine( long ii, long xoff, long fx, long show_color, char *sa ) ;
//////////////////////////////////////////////////////////////////////
extern double get_xx( long i, long y0, long yy ) ;
extern short get_yy( double xx ) ;
extern long get_sign( long yy ) ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(DoDrawUtils__INCLUDED_)
