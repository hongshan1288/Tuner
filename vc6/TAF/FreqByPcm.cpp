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

	m_LineMode = PS_SOLID ;

	m_DC_line_color = RGB( 100, 0, 0 ) ;

	m_Period_dn = 1 ;

	m_PeriodData_m = 0 ;
	m_PeriodData_n = 0 ;
	Set_PeriodData_Len( 1000 ) ;

	m_SameData_m = 0 ;
	m_SameData_n = 0 ;
	set_SameData_Len( 500 ) ;

	m_pcm_data = NULL ;

	m_DCFlag = false ;
	m_MemFlag = true ;

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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
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
long TFreqByPcm::push_PeriodData( long ix, short ff )
{

	PeriodData_Type	*pd ;

	pd = m_PeriodData ;
	pd += m_PeriodData_m++ ;

	pd->ix = ix ;
	pd->ff = ff ;

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
	long	i, ff ;
	ff = 0 ;
	for ( i=0; i<pcm_len; i++ )
	{
		if ( pcm_data[i]==0 )
		{
			if ( ff==1 )
			{
				m_Period_is = i ;
				break ;
			}
		}
		else
			ff = 1 ;
	}
	ff = 0 ;
	for ( i=pcm_len-1; i>=0; i-- )
	{
		if ( pcm_data[i]==0 )
		{
			if ( ff==1 )
			{
				m_Period_ie = i ;
				break ;
			}
		}
		else
			ff = 1 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_PeriodData( short *pcm_data, long pcm_len )
{
	long	i, ff, yy, y0 ;

	get_PeriodData_vars( pcm_data, pcm_len ) ;

	m_PeriodData_m = 0 ;

	push_PeriodData( m_Period_is, 0 ) ;
	ff = 0 ;
	for ( i=m_Period_is+1; i<m_Period_ie; i+=m_Period_dn )
	{
		yy = pcm_data[i];
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
				push_PeriodData( i, (short)ff ) ;
				ff = 0 ;
			}
			else if ( y0<0 && yy>0 )
			{
				if ( abs(y0)<abs(yy) )
					push_PeriodData( i-1, -1 ) ;
				else
					push_PeriodData( i, -1 ) ;
				ff = 1 ;
			}
			else if ( y0>0 && yy<0 )
			{
				if ( abs(y0)<abs(yy) )
					push_PeriodData( i-1, 1 ) ;
				else
					push_PeriodData( i, 1 ) ;
				ff = -1 ;
			}
		}
		y0 = yy ;
	}
	push_PeriodData( m_Period_ie, 100 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PeriodData( long show_color )
{
	long	i, ix ;
	char	ss[50] ;
	m_LineMode = PS_SOLID ;
	for ( i=0; i<m_PeriodData_m; i++ )
	{
		ix = m_PeriodData[i].ix ;
		sprintf( ss, "%ld-%ld-%ld-%ld", i, ix, m_PeriodData[i].in, m_PeriodData[i].dy_sum ) ;
		if ( ( i % 2 )==0 )
			draw_VerticalLine( ix, 0, -5, RGB( 50, 50, 10 ), ss ) ;
		else
			draw_VerticalLine( ix, 0, -20, RGB( 50, 50, 10 ), ss ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::draw_VerticalLine( long ii, long xoff, long fx, long show_color, char *sa )
{
	long	x, y, y00 ;
	double	xx, dx ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= m_pcm_len ;

	xx = dx*(ii-1) ;
	x = (long)floor(xx) ;
	x += xoff ;
	y = get_waveForm_y( m_pcm_data[ii], y00 ) ;
	do_DrawLine( x, y, x, g_waveForm_bb, 1, show_color, m_LineMode, R2_COPYPEN ) ;

	if ( sa[0] != '\0' )
		do_DrawText( sa, x, g_waveForm_bb, show_color, g_waveForm_backColor, 10, fx ) ;
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
void TFreqByPcm::make_dy_data( short *pcm_data, long pcm_len )
{
	long	i, dy ;
	m_dy_data[0] = 0 ;
	for ( i=1; i<pcm_len; i++ )
	{
		dy = (pcm_data[i]-pcm_data[i-1]) ;
		dy /= m_FlatVV ;
		m_dy_data[i] = (char)dy ;
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_dy_data( long show_color )
{
	long	i, x, y, y00 ;
	double	xx, dx ;
	char	ss[50] ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= m_pcm_len ;

	for ( i=m_Period_is; i<m_Period_ie; i++ )
	{
		xx = dx*(i-1) ;
		x = (long)floor(xx) ;
		y = get_waveForm_y( m_pcm_data[i], y00 ) ;
		sprintf( ss, "%d", m_dy_data[i] ) ;
		do_DrawText( ss, x, y, show_color, g_waveForm_backColor, 0, -5 ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_PeriodData_dySum()
{
	long	i, k, dy_sum ;
	m_PeriodData[0].dy_sum = 0 ;
	for ( i=1; i<m_PeriodData_m; i++ )
	{
		dy_sum = 0 ;
		for ( k=m_PeriodData[i-1].ix; k<m_PeriodData[i].ix; k++ )
			dy_sum += abs(m_dy_data[k]) ;
		m_PeriodData[i].dy_sum = dy_sum ;
		m_PeriodData[i].in = m_PeriodData[i].ix-m_PeriodData[i-1].ix ;
	}
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc )
{

	if ( check_pcm_data_is_a_noise( pcm_data, pcm_len ) )
		return ( -1 ) ;

	m_dx = g_waveForm_aa ;
	m_dx /= pcm_len ;

	m_pcm_data = (short*)GlobalAlloc( GPTR, (pcm_len)*sizeof(short) ) ;
	m_dy_data = (char*)GlobalAlloc( GPTR, (pcm_len)*sizeof(char) ) ;

	__try
	{
		CopyMemory( m_pcm_data, pcm_data, pcm_len*sizeof(short) ) ;
		m_pcm_len = pcm_len ;

		show_PcmData( m_pcm_data, m_pcm_len, RGB(0,20,0), RGB(0,50,0) ) ;

		make_flat_data( m_pcm_data, m_pcm_len ) ;
		show_PcmData( m_pcm_data, m_pcm_len, RGB(50,50,50), RGB(100,100,100) ) ;

		make_PeriodData( m_pcm_data, m_pcm_len ) ;

		make_dy_data( m_pcm_data, m_pcm_len ) ;
		make_PeriodData_dySum() ;

		show_dy_data( RGB(40,40,50) ) ;
		show_PeriodData( RGB(80,20,20) ) ;

		make_SameData() ;
		make_SameData2() ;

		show_NextData( 0, 20, RGB(80,20,120), PS_SOLID ) ;
		show_NextData( 1, 40, RGB(20,80,80), PS_DASH ) ;

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
void TFreqByPcm::set_SameData_Len( long nn )
{
	SameData_Type	*sd ;
	sd = (SameData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(SameData_Type) ) ;
	if ( m_SameData_n>0 && m_SameData != NULL ) 
	{
		CopyMemory( sd, m_SameData, m_SameData_n*sizeof(SameData_Type) ) ;
		FreeBuf( m_SameData ) ;
	}
	m_SameData = sd ;
	m_SameData_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::push_SameData( long cur_ii, long next_ii, long pd_nn, long dy_tot )
{

	SameData_Type	*sd ;

	sd = m_SameData ;
	sd += m_SameData_m++ ;

	sd->cur_ii = cur_ii ;
	sd->next_ii = next_ii ;
	sd->pd_nn = pd_nn ;

	sd->in_tot = m_PeriodData[cur_ii].in ;
	sd->dy_sum = m_PeriodData[cur_ii].dy_sum ;
	sd->dy_tot = dy_tot ;

	if ( m_SameData_m>=m_SameData_n )
		set_SameData_Len( m_SameData_n+500 ) ;

	return ( m_SameData_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::get_DyTot( long i1, long i2, long pd_nn )
{
	long	i, nn, dy_tot ;
	long	ii1, ii2 ;
	dy_tot = 0 ;

	nn = m_PeriodData[i1].ix-m_PeriodData[i1-pd_nn].ix ;
	ii1 = m_PeriodData[i1-pd_nn].ix ;
	ii2 = m_PeriodData[i2-pd_nn].ix ;
	for ( i=0; i<nn; i++ )
	{
		dy_tot += abs(m_pcm_data[i+ii1]-m_pcm_data[i+ii2] ) ;
	}
	dy_tot /= m_FlatVV ;
	dy_tot /= nn ;
	return ( dy_tot ) ;
}
//////////////////////////////////////////////////////////////////////
bool TFreqByPcm::like_SameData( long i1, long i2, long pd_nn )
{
	long	i ;
	long	in1, in2, dy_sum1, dy_sum2 ;
	long	in_tot, dy_sum_tot ;

	in1 = in2 = 0 ;
	dy_sum1 = dy_sum2 = 0 ;
	for ( i=0; i<pd_nn; i++ )
	{
		in1 += m_PeriodData[i+i1].in ;
		in2 += m_PeriodData[i+i2].in ;
		dy_sum1 += m_PeriodData[i+i1].dy_sum ;
		dy_sum2 += m_PeriodData[i+i2].dy_sum ;
	}
	in_tot = abs(in1-in2) ;
	dy_sum_tot = abs(dy_sum1-dy_sum2) ;
	if ( in_tot<=m_Max_In*pd_nn && dy_sum_tot<=m_Max_DySum*pd_nn )
		return ( true ) ;
	else
		return ( false ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::push_NextData( long i, long next_ii, long idx )
{
	PeriodData_Type	*pd ;
	pd = &m_PeriodData[i] ;
	pd->next_ii[idx] = next_ii ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::get_SameData( long i, long pd_nn )
{
	long	ii, dy_tot ;
	for ( ii=i+pd_nn; ii<m_PeriodData_m; ii++ )
	{
		if ( like_SameData( i, ii, pd_nn ) )
		{
			dy_tot = get_DyTot( i, ii, pd_nn ) ;
			if ( dy_tot<=m_Max_DyTot )
			{
//				push_SameData( i, ii, pd_nn, dy_tot ) ;
				push_NextData( i, ii, 0 ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_SameData()
{
	long	i, n, f, m ;

	m_Max_In = 3 ;
	m_Max_DySum = 5 ;
	m_Max_DyTot = 1 ;

	m_SameData_m = 0 ;
	n = 3 ;
	m = 0 ;
	for ( i=n; i<m_PeriodData_m; i+=n )
	{
		f = get_SameData( i, n ) ;
		if ( f<0 )
		{
			m ++ ;
			if ( m>3 )
				break ;
		}
		else
			m = 0 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
double TFreqByPcm::get_DyTot22( long i1, long i2, long pd_nn )
{
	long	i, nn ;
	double	dy_tot ;
	long	ii1, ii2 ;

	nn = m_PeriodData[i1].ix-m_PeriodData[i1-pd_nn].ix ;
	ii1 = m_PeriodData[i1-pd_nn].ix ;
	ii2 = m_PeriodData[i2-pd_nn].ix ;
	dy_tot = 0 ;
	for ( i=0; i<nn; i++ )
	{
		if ( abs(m_pcm_data[i+ii1]-m_pcm_data[i+ii2] )<=m_FlatVV*2 )
			dy_tot += 1 ;
	}
	dy_tot /= nn ;
	return ( dy_tot ) ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::get_DyTot2( long i1, long i2, long pd_nn )
{
	long	i, nn, dy_tot ;
	long	ii1, ii2 ;

	nn = m_PeriodData[i1].ix-m_PeriodData[i1-pd_nn].ix ;
	ii1 = m_PeriodData[i1-pd_nn].ix ;
	ii2 = m_PeriodData[i2-pd_nn].ix ;
	dy_tot = 0 ;
	for ( i=0; i<nn; i++ )
	{
		if ( abs(m_pcm_data[i+ii1]-m_pcm_data[i+ii2] )<=m_FlatVV*2 )
			dy_tot += 1 ;
	}
	if ( dy_tot>0 )
		return ( nn-dy_tot ) ;
	else
		return ( 10000 ) ;
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::get_SameData2( long i, long pd_nn )
{
	long	ii ;
	double	dy_tot ;
	for ( ii=i+pd_nn; ii<m_PeriodData_m; ii++ )
	{
		if ( like_SameData( i, ii, pd_nn ) )
		{
			dy_tot = get_DyTot22( i, ii, pd_nn ) ;
			if ( dy_tot>=m_Max_DyTot2 )
			{
				push_NextData( i, ii, 1 ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_SameData2()
{
	long	i, n, m, f ;

	m_Max_In = 3 ;
	m_Max_DySum = 5 ;
	m_Max_DyTot = 3 ;
	m_Max_DyTot2 = 0.818 ;

	m_SameData_m = 0 ;
	n = 2 ;
	m = 0 ;
	for ( i=n; i<m_PeriodData_m; i+=n )
	{
		f = get_SameData2( i, n ) ;
		if ( f<0 )
		{
			m ++ ;
			if ( m>3 )
				break ;
		}
		else
			m = 0 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_NextData( long idx, long y_off, long show_color, long line_mode )
{
	long	i, next_ii, ix, tot ;
	char	ss[50] ;

	m_LineMode = line_mode ;
	for ( i=0; i<m_PeriodData_m; i++ )
	{
		next_ii = m_PeriodData[i].next_ii[idx] ;
		if ( next_ii>0 )
		{
			ix = m_PeriodData[i].ix ;
			tot = m_PeriodData[next_ii].ix-ix ;
			sprintf( ss, "%ld-%ld-%ld", i, next_ii, tot ) ;
			if ( ( i % 2 )==0 )
				draw_VerticalLine( ix, 1+idx, -y_off, show_color, ss ) ;
			else
				draw_VerticalLine( ix, 1+idx, -(y_off+20), show_color, ss ) ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

