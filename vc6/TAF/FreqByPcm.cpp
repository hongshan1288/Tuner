// FreqByPcm.cpp: implementation of the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreqByPcm.h"
#include "ComUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TFreqByPcm::TFreqByPcm()
{

	m_MinVV = 1000 ;
	m_FlatVV = 3200/4 ;

	m_DC_line_color = RGB( 100, 0, 0 ) ;

	m_Period_dn = 1 ;

	// init mode data
	m_PcmModeData_m = 0 ;
	m_PcmModeData_n = 0 ;

	SetPcmModeDataLen( 1000 ) ;

	m_PeakData_m = 0 ;
	m_PeakData_n = 0 ;
	SetPeakDataLen( 1000 ) ;


	m_PeriodData_m = 0 ;
	m_PeriodData_n = 0 ;
	Set_PeriodData_Len( 5000 ) ;

	m_pcm_data = NULL ;

	m_DCFlag = false ;
	m_MemFlag = true ;

//	m_pcm_data = g_int_data ;

}
//////////////////////////////////////////////////////////////////////
TFreqByPcm::~TFreqByPcm()
{

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::do_DrawText( char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign )
{
	if ( m_DCFlag )
		DrawText( g_waveForm_DC, pText, xx, yy, font_color, bk_color, text_align, text_valign ) ;
	if ( m_MemFlag )
		DrawText( g_waveForm_DC_mem, pText, xx, yy, font_color, bk_color, text_align, text_valign ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::do_DrawLine( long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode )
{
	if ( m_DCFlag )
		DrawLine( g_waveForm_DC, x1, y1, x2, y2, line_width, line_color, line_style, line_mode ) ;
	if ( m_MemFlag )
		DrawLine( g_waveForm_DC_mem, x1, y1, x2, y2, line_width, line_color, line_style, line_mode ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::do_Polyline( POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  )
{
	if ( m_DCFlag )
		PolyLine( g_waveForm_DC, xy, xy_nn, line_width, line_color, line_style, line_mode ) ;
	if ( m_MemFlag )
		PolyLine( g_waveForm_DC_mem, xy, xy_nn, line_width, line_color, line_style, line_mode ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::do_DrawBar( long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode )
{
	if ( m_DCFlag )
		DrawBar( g_waveForm_DC, x1, y1, x2, y2, bar_color, bar_style, bar_mode ) ;
	if ( m_MemFlag )
		DrawBar( g_waveForm_DC_mem, x1, y1, x2, y2, bar_color, bar_style, bar_mode ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::SetPcmModeDataLen( long nn )
{
	PcmModeData_Type	*pmd ;
	pmd = (PcmModeData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PcmModeData_Type) ) ;
	if ( m_PcmModeData_n>0 && m_PcmModeData != NULL ) 
	{
		CopyMemory( pmd, m_PcmModeData, m_PcmModeData_n*sizeof(PcmModeData_Type) ) ;
		FreeBuf( m_PcmModeData ) ;
	}
	m_PcmModeData = pmd ;
	m_PcmModeData_n = nn ;

}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::PushPcmModeData( long idx_s, long idx_e, long idx_f )
{
	PcmModeData_Type	*pmd ;

	pmd = m_PcmModeData ;
	pmd += m_PcmModeData_m++ ;

	pmd->idx_s = idx_s ;
	pmd->idx_e = idx_e ;
	pmd->idx_f = idx_f ;

	if ( m_PcmModeData_m>=m_PcmModeData_n )
		SetPcmModeDataLen( m_PcmModeData_n+1000 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_pmd_data( short *pcm_data, long pcm_len )
{
	long	i ;
	for ( i=0; i<pcm_len; i++ )
	{
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_flat_data( short *pcm_data, long pcm_len )
{
	long	vvv ;
	long	i ;

	for ( i=0; i<pcm_len; i++ )
	{
		vvv = pcm_data[i] ;
		vvv /= m_FlatVV ;
		vvv *= m_FlatVV ;
		pcm_data[i] = (short)(vvv) ;
	}
}
//////////////////////////////////////////////////////////////////////
bool TFreqByPcm::check_pcm_data_is_a_noise( short *pcm_data, long pcm_len )
{
	if ( pcm_len<=0 )
		return ( true ) ;

	long	i ;
	double	vv ;

	vv = 0 ;
	for ( i=0; i<pcm_len; i++ )
		vv += fabs(pcm_data[i]) ;
	vv /= pcm_len ;
	if ( vv<m_MinVV )
		return ( true ) ;
	else
		return ( false ) ;

}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::draw_WaveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dot_color )
{

	long	y0 = g_waveForm_bb / 2 ;

	do_Polyline( xy_data, xy_nn, line_width, waveForm_color, PS_SOLID, R2_COPYPEN ) ;
	if ( dot_show_flag>0 )
	{
		long	i, x, y ;

		for ( i=0; i<xy_nn; i++ )
		{
			x = xy_data[i].x ;
			y = xy_data[i].y ;
			do_DrawBar( x-1, y-1, x+1, y+1, dot_color, PS_SOLID, R2_COPYPEN ) ;
		}
	}
	DoEventProc() ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PcmData( short *pcm_data, long pcm_len, long show_color, long dot_color )
{
	do_DrawLine( 0, g_waveForm_bb / 2, g_waveForm_aa-1, g_waveForm_bb / 2, 1, m_DC_line_color, PS_SOLID, R2_COPYPEN ) ;

	g_waveForm_xy_nn = make_WaveForm_Data( pcm_data, pcm_len, 0, g_waveForm_aa, g_waveForm_xy ) ;
	draw_WaveForm_data( 1, g_waveForm_xy, g_waveForm_xy_nn, 1, show_color, dot_color ) ; 
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::SetPeakDataLen( long nn )
{
	PeakData_Type	*peak_d ;
	peak_d = (PeakData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PeakData_Type) ) ;
	if ( m_PeakData_n>0 && m_PeakData != NULL ) 
	{
		CopyMemory( peak_d, m_PeakData, m_PeakData_n*sizeof(PeakData_Type) ) ;
		FreeBuf( m_PeakData ) ;
	}
	m_PeakData = peak_d ;
	m_PeakData_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::PushPeakData( long x, short y, short f )
{
	PeakData_Type	*peak_d ;

	peak_d = m_PeakData ;
	peak_d += m_PeakData_m++ ;

	peak_d->x = x ;
	peak_d->y = y ;
	peak_d->f = f ;

	if ( m_PeakData_m>=m_PeakData_n )
		SetPeakDataLen( m_PeakData_n+1000 ) ;

	return ( m_PeakData_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_PeakData( short *pcm_data, long pcm_len )
{
	long	i, min_vv, max_vv ;
	short	f ;

	min_vv = 3000000 ;
	max_vv = -3000000 ;

	m_PeakData_m = 0 ;
	f = 0 ;
	for ( i=0; i<pcm_len-1; i++ )
	{
		if ( f>0 )
		{
			if ( pcm_data[i+1]>=pcm_data[i] )
				;
			else 
			{
				PushPeakData( i, pcm_data[i], f ) ;
				f = -1 ;
				if ( max_vv<pcm_data[i] )
				{
					max_vv = pcm_data[i] ;
					m_Max_PeakData_idx = m_PeakData_m-1 ;
				}
			}
		}
		else if ( f<0 )
		{
			if ( pcm_data[i+1]<=pcm_data[i] )
				;
			else 
			{
				PushPeakData( i, pcm_data[i], f ) ;
				f = 1 ;
				if ( min_vv>pcm_data[i] )
				{
					min_vv = pcm_data[i] ;
					m_Min_PeakData_idx = m_PeakData_m-1 ;
				}
			}
		}
		else
		{
			if ( pcm_data[i+1]>pcm_data[i] )
			{
				PushPeakData( i, pcm_data[i], 0 ) ;
				f = 1 ;
			}
			else if ( pcm_data[i+1]<pcm_data[i] )
			{
				PushPeakData( i, pcm_data[i], 0 ) ;
				f = -1 ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PeakData_Vars( long i, long x, long y )
{
	char	ss[100] ;
	long	show_color ;

	show_color = RGB(120,120,120) ;
	if ( i==m_Min_PeakData_idx )
	{
		m_Min_PeakData_yy = y ;
		sprintf( ss, "min" ) ;
		do_DrawText( ss, x, y, show_color, g_waveForm_backColor, 10, 20 ) ;
	}
	else if ( i==m_Max_PeakData_idx )
	{
		sprintf( ss, "max" ) ;
		do_DrawText( ss, x, y, show_color, g_waveForm_backColor, 10, -20 ) ;
	}
	if ( i==m_PeakData_is )
	{
		do_DrawLine( x, y, x, g_waveForm_bb, 1, show_color, PS_SOLID, R2_COPYPEN ) ;
		sprintf( ss, "S" ) ;
		do_DrawText( ss, x, g_waveForm_bb, show_color, g_waveForm_backColor, 10, -10 ) ;
	}
	if ( i==m_PeakData_ie )
	{
		do_DrawLine( x, y, x, g_waveForm_bb, 1, show_color, PS_SOLID, R2_COPYPEN ) ;
		sprintf( ss, "E" ) ;
		do_DrawText( ss, x, g_waveForm_bb, show_color, g_waveForm_backColor, -10, -10 ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PeakData( long pcm_len, long show_color )
{
	long	i, x, y, x0, y0, y00 ;
	long	fx, fy ;
	double	xx, dx ;
	char	ss[100] ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= pcm_len ;

	fx = 0 ;
	for ( i=0; i<m_PeakData_m; i++ )
	{
		y = get_waveForm_y( m_PeakData[i].y, y00 ) ;
		xx = dx*(m_PeakData[i].x-1) ;
		x = (long)floor(xx) ;
		if ( i>0 ) 
			do_DrawLine( x0, y0, x, y, 1, show_color, PS_SOLID, R2_COPYPEN ) ;
		do_DrawBar( x-1, y-1, x+1, y+1, show_color, PS_SOLID, R2_COPYPEN ) ;
		fy = -10*m_PeakData[i].f ;

		sprintf( ss, "%d", m_PeakData[i].y ) ;
		do_DrawText( ss, x, y, show_color, g_waveForm_backColor, fx, fy ) ;
		x0 = x ;
		y0 = y ;

		show_PeakData_Vars( i, x, y ) ;

	}

	do_DrawLine( 0, y00, g_waveForm_aa-1, y00, 1, RGB(120,0,0), PS_SOLID, R2_COPYPEN ) ; // dc_line

	y = m_Min_PeakData_yy ;
	do_DrawLine( 0, y, g_waveForm_aa-1, y, 1, RGB(120,0,120), PS_SOLID, R2_COPYPEN ) ; // min_line

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::Set_PeriodData_Len( long nn )
{
	PeriodData_Type	*pd ;
	pd = (PeriodData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PeriodData_Type) ) ;
	if ( m_PeriodData_n>0 && m_PeriodData != NULL ) 
	{
		CopyMemory( pd, m_PeriodData, m_PeriodData_n*sizeof(PeriodData_Type) ) ;
		FreeBuf( m_PeriodData ) ;
	}
	m_PeriodData = pd ;
	m_PeriodData_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::push_PeriodData( long ix, short yy )
{

	PeriodData_Type	*pd ;

	pd = m_PeriodData ;
	pd += m_PeriodData_m++ ;

	pd->ix = ix ;
	pd->yy = yy ;

	if ( m_PeriodData_m>=m_PeriodData_n )
		Set_PeriodData_Len( m_PeriodData_n+1000 ) ;

	return ( m_PeriodData_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::get_PeriodData_yy( short *pcm_data, long ix, long n )
{
	double	yy ;
	long	i ;

	yy = 0 ;
	for ( i=0; i<n; i++ )
		yy += pcm_data[i+ix] ;
	return ( long( yy/n ) ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::get_PeriodData_vars( short *pcm_data, long pcm_len )
{
	long	i ;
	for ( i=0; i<pcm_len; i++ )
	{
		if ( pcm_data[i]==0 )
		{
			m_Period_is = i ;
			break ;
		}
	}
	for ( i=pcm_len-1; i>=0; i-- )
	{
		if ( pcm_data[i]<=0 && pcm_data[i-1]>0 )
		if ( pcm_data[i]==0 )
		{
			m_Period_ie = i ;
			break ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_PeriodData( short *pcm_data, long pcm_len )
{
	long	i, ff, yy, y0 ;

	get_PeriodData_vars( pcm_data, pcm_len ) ;

	m_PeriodData_m = 0 ;

	ff = 0 ;

	i = m_Period_is ;
	yy = get_PeriodData_yy( pcm_data, i, m_Period_dn ) ;
	push_PeriodData( i, (short)yy ) ;
	ff = 0 ;
	for ( i=m_Period_is+1; i<m_Period_ie; i++ )
	{
		yy = get_PeriodData_yy( pcm_data, i, m_Period_dn ) ;
		if ( ff==0 )
		{
			if ( yy<0 )
				ff = -1 ;
			else if ( yy>0 )
				ff = 1 ;
		}
		else
		{
			if ( yy==0 )
			{
				push_PeriodData( i, (short)yy ) ;
				ff = 0 ;
			}
			else if ( y0<0 && yy>0 )
			{
				push_PeriodData( i, (short)yy ) ;
				ff = 1 ;
			}
			else if ( y0>0 && yy<0 )
			{
				push_PeriodData( i, (short)yy ) ;
				ff = -1 ;
			}
		}
		y0 = yy ;
	}
	i = m_Period_ie ;
	yy = get_PeriodData_yy( pcm_data, i, m_Period_dn ) ;
	push_PeriodData( i, (short)yy ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PeriodData( long show_color )
{
	long	i, ix, ix0 ;
	char	ss[50] ;
	for ( i=0; i<m_PeriodData_m; i++ )
	{
		ix = m_PeriodData[i].ix ;
		if ( i>0 )
			sprintf( ss, "%ld-%ld-%ld", i, ix, ix-ix0 ) ;
		else
			sprintf( ss, "%ld-%ld", i, ix ) ;
		draw_VerticalLine( ix, -1, RGB( 50, 50, 10 ), ss ) ;
		ix0 = ix ;
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::draw_VerticalLine( long ii, long fx, long show_color, char *sa )
{
	long	x, y, y00 ;
	double	xx, dx ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= m_pcm_len ;

	xx = dx*(ii-1) ;
	x = (long)floor(xx) ;
	y = get_waveForm_y( m_pcm_data[ii], y00 ) ;
	do_DrawLine( x, y, x, g_waveForm_bb, 1, show_color, PS_SOLID, R2_COPYPEN ) ;

	char	ss[50] ;
	sprintf( ss, "%d", m_pcm_data[ii] ) ;
	do_DrawText( ss, x, y, show_color, g_waveForm_backColor, fx, -10 ) ;
	if ( sa[0] != '\0' )
		do_DrawText( sa, x, g_waveForm_bb, show_color, g_waveForm_backColor, 10, -10 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc )
{

	if ( check_pcm_data_is_a_noise( pcm_data, pcm_len ) )
		return ( -1 ) ;

	m_dx = g_waveForm_aa ;
	m_dx /= pcm_len ;

	m_pcm_data = (short*)GlobalAlloc( GPTR, (pcm_len)*sizeof(short) ) ;
	__try
	{
		CopyMemory( m_pcm_data, pcm_data, pcm_len*sizeof(short) ) ;
		m_pcm_len = pcm_len ;

		show_PcmData( m_pcm_data, m_pcm_len, RGB(0,20,0), RGB(0,50,0) ) ;

		make_flat_data( m_pcm_data, m_pcm_len ) ;
		show_PcmData( m_pcm_data, m_pcm_len, RGB(50,50,50), RGB(100,100,100) ) ;

		make_PeriodData( m_pcm_data, m_pcm_len ) ;
		show_PeriodData( RGB(80,20,20) ) ;

//		make_DiffData() ;

	}
	__finally
	{
		FreeBuf( m_pcm_data ) ;
	}

	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

