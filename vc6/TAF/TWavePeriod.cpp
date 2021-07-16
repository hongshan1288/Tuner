// TWavePeriod.cpp: implementation of the TWavePeriod class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TWavePeriod.h"
#include "ComUtils.h"
#include "DoDrawUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWavePeriod::TWavePeriod()
{

	m_MinVV = 1000 ;
	m_FlatVV = 3200/8 ; // 平滑值
	m_DC_line_color = RGB( 80, 10, 10 ) ;

	m_PeriodYY_m = 0 ;
	m_PeriodYY_n = 0 ;
	set_PeriodYY_Len( 1000 ) ;

	m_PeriodData_m = 0 ;
	m_PeriodData_n = 0 ;
	set_PeriodData_Len( 100 ) ;

	m_PcmData_nn = 30 * 44100 ; // 30秒单声道的缓冲区
	m_PcmData = (short*)GlobalAlloc(GPTR, m_PcmData_nn*sizeof(short) ) ;
	m_PcmData_mm = 0 ;
	m_PcmData_si = 0 ;

}
//////////////////////////////////////////////////////////////////////
TWavePeriod::~TWavePeriod()
{
	FreeBuf( m_PeriodYY ) ;
	FreeBuf( m_PeriodData ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_PeriodYY_Len( long nn )
{
	PeriodYY_Type	*pyd ;

	pyd = (PeriodYY_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PeriodYY_Type) ) ;
	if ( m_PeriodYY_n>0 && m_PeriodYY != NULL ) 
	{
		CopyMemory( pyd, m_PeriodYY, m_PeriodYY_n*sizeof(PeriodYY_Type) ) ;
		FreeBuf( m_PeriodYY ) ;
	}
	m_PeriodYY = pyd ;
	m_PeriodYY_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::push_PeriodYY( long xx, short yy )
{

	PeriodYY_Type	*pyd ;

	pyd = m_PeriodYY ;
	pyd += m_PeriodYY_m++ ;

	pyd->xx = xx ;
	pyd->yy = yy ;

	if ( m_PeriodYY_m>=m_PeriodYY_n )
		set_PeriodYY_Len( m_PeriodYY_n+1000 ) ;

	return ( m_PeriodYY_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_PeriodYY()
{
	long	i, nn ;
	short	yy ;
	double	xx_tot, xx, x0, dx ;

	dx = 1 ;
	for ( i=0; i<m_PeriodData_m-1; i++ )
	{
		push_PeriodYY( 0, 0 ) ; //原点肯定是0
		m_PeriodData[i].PeriodYY_si = m_PeriodYY_m ;
		x0 = m_PeriodData[i].xx ;
		xx_tot = m_PeriodData[i+1].xx-x0 ;
		nn = 1 ;
		for ( xx=dx; xx<xx_tot; xx+=dx )
		{
			yy = get_yy( x0+xx, m_PcmData ) ;	
			push_PeriodYY( nn++, yy ) ;
		}
		m_PeriodData[i].PeriodYY_nn = nn ;
//		make_SegDY( i ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::show_PeriodYY( long idx, long show_color )
{
	long	i, nn, si ;
	double	x0, xx ;
	short	yy ;

	m_LineMode = PS_DASH ;

	si = m_PeriodData[idx].PeriodYY_si ;
	nn = m_PeriodData[idx].PeriodYY_nn ;
	x0 = m_PeriodData[idx].xx ;
	for ( i=0; i<nn; i++ )
	{
		xx = m_PeriodYY[i+si].xx ;
		yy = m_PeriodYY[i+si].yy ;
		draw_VLine( x0+xx, 0, yy, 0, 0, show_color, "" ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_PeriodData_Len( long nn )
{
	PeriodDa_Type	*pd ;
	pd = (PeriodDa_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PeriodDa_Type) ) ;
	if ( m_PeriodData_n>0 && m_PeriodData != NULL ) 
	{
		CopyMemory( pd, m_PeriodData, m_PeriodData_n*sizeof(PeriodDa_Type) ) ;
		FreeBuf( m_PeriodData ) ;
	}
	m_PeriodData = pd ;
	m_PeriodData_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::push_PeriodData( short ff, long ix, double xx )
{

	PeriodDa_Type	*pd ;

	pd = m_PeriodData ;
	pd += m_PeriodData_m++ ;

	pd->xx = xx ;
	pd->ix = ix ;
	pd->ff = ff ;
	pd->next_ii[0] = pd->next_ii[1] = 0 ;

	if ( m_PeriodData_m>=m_PeriodData_n )
		set_PeriodData_Len( m_PeriodData_n+100 ) ;

	return ( m_PeriodData_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_flat_data( short *pcm_data, long pcm_len )
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
void TWavePeriod::push_PcmData(short *pcm_data, long pcm_len)
{
	if ( m_PcmData_mm+pcm_len>m_PcmData_nn )
		return ;

	make_flat_data( pcm_data, pcm_len ) ;

	short	*pp ;
	pp = m_PcmData ;
	pp += m_PcmData_mm ;
	CopyMemory( pp, pcm_data, pcm_len*sizeof(short) ) ;
	m_PcmData_mm += pcm_len ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
inline long TWavePeriod::get_sign( long yy )
{
	if ( yy>0 )
		return ( 1 ) ;
	else if ( yy<0 )
		return ( -1 ) ;
	else
		return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
double TWavePeriod::get_xx( long i, long y0, long yy )
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
short TWavePeriod::get_yy( double xx, short *pcm_data )
{
	double	kk, yy, x1, x2, y1, y2 ;
	long	ix ;

	ix = (long)xx ;
	x1 = ix ;
	x2 = ix+1 ;

	y1 = pcm_data[ix] ;
	y2 = pcm_data[ix+1] ;

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
void TWavePeriod::make_PeriodData()
{
	long	i, ff, yy, y0 ;
	double	xx ;

	y0 = m_PcmData[m_PcmData_si];
	ff = get_sign( y0 ) ;
	for ( i=m_PcmData_si; i<m_PcmData_mm; i++ )
	{
		yy = m_PcmData[i];
		if ( m_PeriodData_m==0 )
		{
			if ( y0<0 && yy>=0 ) // 第一个点一定是从负值到正值（包括0）
			{
				xx = get_xx( i, y0, yy ) ;
				push_PeriodData( 1001, i-1, xx ) ;
				ff = 1 ;
			}
		}
		else
		{
			if ( ff>0 )
			{
				if ( yy<0 )
				{
					xx = get_xx( i, y0, yy ) ;
					push_PeriodData( 1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
			else if ( ff<0 )
			{
				if ( yy>0 )
				{
					xx = get_xx( i, y0, yy ) ;
					push_PeriodData( -1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
		}
		y0 = yy ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::remove_TooShort_PeriodData( double min_dx )
{
	long	i, ii, nn ;
	double	dx, dx_avg, dx_max ;

	// 1. 计算dx的平均值
	dx_avg = 0 ;
	dx_max = 0 ;
	for ( i=m_PeriodData_si; i<m_PeriodData_m; i++ )
	{
		dx = m_PeriodData[i].xx-m_PeriodData[i-1].xx ;
		if ( dx_max<dx )
			dx_max = dx ;
		dx_avg += dx ;
	}
	dx_avg /= (m_PeriodData_m-m_PeriodData_si) ;
	dx_avg *= min_dx ;

	// 2. 去除小于平均值百分比（0.2）的点
	ii = m_PeriodData_si ;
	for ( i=m_PeriodData_si+1; i<m_PeriodData_m;)
	{
		dx = m_PeriodData[i].xx-m_PeriodData[ii].xx ;
		if ( dx<dx_avg || dx<dx_max/2 )
		{
			nn = m_PeriodData_m-i ;
			CopyMemory( &m_PeriodData[i], &m_PeriodData[i+1], nn*sizeof(PeriodDa_Type) ) ;
			m_PeriodData_m -- ;
		}
		else
		{
			ii = i ;
			i ++ ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::show_PeriodData( long di, long y_off, long show_color, long yy_color, long next_color )
{
	long	i, ix, next_ii ;
	long	x, y ;
	double	xx, dx ;
	char	ss[50] ;

	m_LineMode = PS_SOLID ;
	for ( i=0; i<m_PeriodData_m; i++ )
	{
		if ( i==0 )
			dx = 0 ;
		else
			dx = m_PeriodData[i].xx-m_PeriodData[i-1].xx ;
		xx = m_PeriodData[i].xx ;
		ix = m_PeriodData[i].ix ;
		sprintf( ss, "%ld-%ld-%1.1lf", i, ix, dx ) ;
		if ( ( i % 2 )==0 )
			draw_VLine( xx, 0, -32000, di, -y_off, show_color, ss ) ;
		else
			draw_VLine( xx, 0, -32000, di, -(y_off+10), show_color, ss ) ;

		if ( yy_color>0 )
		{
			if ( i<m_PeriodData_m )
				show_PeriodYY( i, yy_color ) ;
		}
		if ( next_color>0 )
		{
			if ( i<m_PeriodData_m-1 )
			{
				next_ii = m_PeriodData[i].next_ii[0] ;
				if ( next_ii>0 )
				{
					sprintf( ss, "%ld-%ld-%1.1lf", i, next_ii, m_PeriodData[next_ii].xx-xx ) ;
					if ( ( i % 2 )==0 )
						x = draw_VLine( xx, 0, -32000, di, -(y_off+20), next_color, ss ) ;
					else
						x = draw_VLine( xx, 0, -32000, di, -(y_off+35), next_color, ss ) ;

					if ( m_PeriodData[i].next_ff[0]>1000 )
					{
						sprintf( ss, "%ld-%ld", m_PeriodData[i].next_ff[0], m_PeriodData[i].next_nn[0] ) ;
						y = get_waveForm_y( -32000, g_waveForm_bb / 2 ) ;
						do_DrawText( ss, x, y, next_color, g_waveForm_backColor, 10, -60 ) ;
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long TWavePeriod::draw_VLine( double xx, long y1, long y2, long xoff, long fx, long show_color, char *sa )
{
	long	x, y00 ;
	double	dx ;

	y00 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= m_PcmData_mm ;

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
void TWavePeriod::draw_WaveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dot_color )
{
	do_Polyline( xy_data, xy_nn, line_width, waveForm_color, PS_SOLID, R2_COPYPEN ) ;
	do_DrawLine( 0, g_waveForm_bb / 2, g_waveForm_aa-1, g_waveForm_bb / 2, 1, m_DC_line_color, PS_SOLID, R2_COPYPEN ) ;

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
long TWavePeriod::make_WaveFormData( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data )
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

	xx = 0 ;
	x0 = 0 ;
	x = 0 ;
	ii = 0 ;

	xy_data[ii].x = 0 ;

	for ( i=0; i<pcm_len; i++ )
	{
		y = pcm_data[i] ;

		xx = i*dx ;
		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			x = (long)floor(xx) ;

			if ( x>x0 )
			{
				if ( min_y>0 && max_y>0 )
				{
					xy_data[ii].x = x + xxx ;
					xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
					ii++ ;
				}
				else if ( min_y<0 && max_y<0 )
				{
					xy_data[ii].x = x + xxx ;
					xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
					ii++ ;
				}
				else
				{
					xy_data[ii].x = x + xxx ;
					xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
					ii++ ;

					xy_data[ii].x = x + xxx ;
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
			xx = i*dx ;
			xy_data[ii].x = (long)xx + xxx ;
			xy_data[ii].y = get_waveForm_y( y, y0 ) ;
			x0 = (long)xx ;
			ii ++ ;
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
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::show_pcm_data( short *pcm_data, long pcm_len, long show_color, long dot_color )
{
	long	xy_nn, xy_size ;
	POINT	*waveForm_xy ;
		
	xy_size = g_waveForm_aa*2+100 ;
	waveForm_xy = (POINT*)GlobalAlloc( GPTR, xy_size*sizeof(POINT) ) ;
	__try
	{
		xy_nn = make_WaveFormData( pcm_data, pcm_len, 0, g_waveForm_aa, waveForm_xy ) ;
		draw_WaveForm_data( 1, waveForm_xy, xy_nn, 1, show_color, dot_color ) ; 
	}
	__finally
	{
		FreeBuf( waveForm_xy ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_period_data()
{

	m_PcmData_si = 0 ;
	m_PcmData_mm = 0 ;

	m_PeriodYY_m = 0 ;

	m_PeriodData_m = 0 ;
	m_PeriodData_si = 0 ;

}
//////////////////////////////////////////////////////////////////////
double TWavePeriod::make_period_data(short *pcm_data, long pcm_len)
{

	push_PcmData( pcm_data, pcm_len ) ;
	show_pcm_data( m_PcmData, m_PcmData_mm, RGB(10,50,10), RGB(20,80,20) ) ;

	make_PeriodData() ;
	show_PeriodData( 0, 5, RGB(0,0,50), 0, 0 ) ;

	remove_TooShort_PeriodData( 1-0.618 ) ;

	make_PeriodYY() ;
	show_PeriodData( 0, 5, RGB(20,50,20), RGB(0,30,0), RGB(10,50,50) ) ;

	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
