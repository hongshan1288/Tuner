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

	m_PeriodDa_mm = 0 ;
	m_PeriodDa_nn = 0 ;
	set_PeriodDa_Len( 100 ) ;

	m_PcmData_nn = 30 * 44100 ; // 30秒单声道的缓冲区
	m_PcmData = (short*)GlobalAlloc(GPTR, m_PcmData_nn*sizeof(short) ) ;
	m_PcmData_mm = 0 ;
	m_PcmData_si = 0 ;


	m_TJDa_n = 0 ;
	m_zq_val = 1.2 ;
	m_zq_dx = 1.2 ;


}
//////////////////////////////////////////////////////////////////////
TWavePeriod::~TWavePeriod()
{
	FreeBuf( m_PeriodYY ) ;
	FreeBuf( m_PeriodDa ) ;
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
	for ( i=m_PeriodDa_si; i<m_PeriodDa_mm-1; i++ )
	{
		push_PeriodYY( 0, 0 ) ; //原点肯定是0
		m_PeriodDa[i].PeriodYY_si = m_PeriodYY_m ;
		x0 = m_PeriodDa[i].xx ;
		xx_tot = m_PeriodDa[i+1].xx-x0 ;
		nn = 1 ;
		for ( xx=dx; xx<xx_tot; xx+=dx )
		{
			yy = get_yy( x0+xx, m_PcmData ) ;	
			push_PeriodYY( nn++, yy ) ;
		}
		m_PeriodDa[i].PeriodYY_nn = nn ;
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

	si = m_PeriodDa[idx].PeriodYY_si ;
	nn = m_PeriodDa[idx].PeriodYY_nn ;
	x0 = m_PeriodDa[idx].xx ;
	for ( i=0; i<nn; i++ )
	{
		xx = m_PeriodYY[i+si].xx ;
		yy = m_PeriodYY[i+si].yy ;
		draw_VLine( x0+xx, 0, yy, 0, 0, show_color, "" ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_PeriodDa_Len( long nn )
{
	PeriodDa_Type	*pd ;
	pd = (PeriodDa_Type*)GlobalAlloc( GPTR, (nn)*sizeof(PeriodDa_Type) ) ;
	if ( m_PeriodDa_nn>0 && m_PeriodDa != NULL ) 
	{
		CopyMemory( pd, m_PeriodDa, m_PeriodDa_nn*sizeof(PeriodDa_Type) ) ;
		FreeBuf( m_PeriodDa ) ;
	}
	m_PeriodDa = pd ;
	m_PeriodDa_nn = nn ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::push_PeriodDa( short ff, long ix, double xx )
{

	PeriodDa_Type	*pd ;

	pd = m_PeriodDa ;
	pd += m_PeriodDa_mm ;

	pd->xx = xx ;
	pd->ix = ix ;
	pd->ff = ff ;
	pd->next_ii = 0 ;
	pd->next_ff = 0 ;
	pd->next_nn = 0 ;

	if ( m_PeriodDa_mm==m_PeriodDa_si )
		pd->vv = 100 ;
	else
		pd->vv = 0 ;
	m_PeriodDa_mm++ ;

	if ( m_PeriodDa_mm>=m_PeriodDa_nn )
		set_PeriodDa_Len( m_PeriodDa_nn+100 ) ;

	return ( m_PeriodDa_mm-1 ) ;

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
		if ( m_PeriodDa_mm==m_PeriodDa_si )
		{
			if ( y0<0 && yy>=0 ) // 第一个点一定是从负值到正值（包括0）
			{
				xx = get_xx( i, y0, yy ) ;
				push_PeriodDa( 1001, i-1, xx ) ;
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
					push_PeriodDa( 1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
			else if ( ff<0 )
			{
				if ( yy>0 )
				{
					xx = get_xx( i, y0, yy ) ;
					push_PeriodDa( -1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
		}
		y0 = yy ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::remove_ShortPeriodData( double min_dx )
{
	long	i, ii, nn ;
	double	dx, dx_avg, dx_max ;

	// 1. 计算dx的平均值
	dx_avg = 0 ;
	dx_max = 0 ;
	for ( i=m_PeriodDa_si+1; i<m_PeriodDa_mm; i++ )
	{
		dx = m_PeriodDa[i].xx-m_PeriodDa[i-1].xx ;
		if ( dx_max<dx )
			dx_max = dx ;
		dx_avg += dx ;
	}
	dx_avg /= (m_PeriodDa_mm-m_PeriodDa_si) ;
	dx_avg *= min_dx ;

	// 2. 去除小于平均值百分比（1-0.618）或小于最大值一半的点
	ii = m_PeriodDa_si ;
	for ( i=m_PeriodDa_si+1; i<m_PeriodDa_mm;)
	{
		dx = m_PeriodDa[i].xx-m_PeriodDa[ii].xx ;
		if ( dx<dx_avg || dx<dx_max/2 )
		{
			nn = m_PeriodDa_mm-i ;
			CopyMemory( &m_PeriodDa[i], &m_PeriodDa[i+1], nn*sizeof(PeriodDa_Type) ) ;
			m_PeriodDa_mm -- ;
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
	for ( i=0; i<m_PeriodDa_mm; i++ )
	{
		if ( i==0 )
			dx = 0 ;
		else
			dx = m_PeriodDa[i].xx-m_PeriodDa[i-1].xx ;

		xx = m_PeriodDa[i].xx ;
		ix = m_PeriodDa[i].ix ;

		sprintf( ss, "%ld-%ld-%1.1lf", i, ix, dx ) ;
		if ( ( i % 2 )==0 )
			x = draw_VLine( xx, 0, -32000, di, -y_off, show_color, ss ) ;
		else
			x = draw_VLine( xx, 0, -32000, di, -(y_off+10), show_color, ss ) ;

		if ( m_PeriodDa[i].vv>0 )
		{
			sprintf( ss, "***" ) ;
			y = get_waveForm_y( 0, g_waveForm_bb / 2 ) ;
			do_DrawText( ss, x, y, RGB(100,10,10), g_waveForm_backColor, 10, -10 ) ;
		}

		if ( yy_color>0 )
		{
			if ( i<m_PeriodDa_mm )
				show_PeriodYY( i, yy_color ) ;
		}
		if ( next_color>0 )
		{
			if ( i<m_PeriodDa_mm-1 )
			{
				next_ii = m_PeriodDa[i].next_ii ;
				if ( next_ii>0 )
				{
					sprintf( ss, "%ld-%ld-%1.1lf", i, next_ii, m_PeriodDa[next_ii].xx-xx ) ;
					if ( ( i % 2 )==0 )
						x = draw_VLine( xx, 0, -32000, di, -(y_off+20), next_color, ss ) ;
					else
						x = draw_VLine( xx, 0, -32000, di, -(y_off+35), next_color, ss ) ;

					if ( m_PeriodDa[i].next_ff>1000 )
					{
						sprintf( ss, "%ld-%ld", m_PeriodDa[i].next_ff, m_PeriodDa[i].next_nn ) ;
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
long TWavePeriod::get_DyTotFromPeriodData( long i1, long i2, long comp_nn )
{
	long	i, nn ;
	long	ii1, ii2 ;
	double	tot_yy ;

	nn = m_PeriodDa[i1].PeriodYY_si-m_PeriodDa[i1-comp_nn].PeriodYY_si ;
	ii1 = m_PeriodDa[i1-comp_nn].PeriodYY_si ;
	ii2 = m_PeriodDa[i2-comp_nn].PeriodYY_si ;

	tot_yy = 0 ;
	for ( i=0; i<nn; i++ )
		tot_yy += fabs(m_PeriodYY[i+ii1].yy-m_PeriodYY[i+ii2].yy) ;

	tot_yy /= m_FlatVV ;
	tot_yy /= nn ;
	return ( (long)(tot_yy) ) ;
}
//////////////////////////////////////////////////////////////////////
double TWavePeriod::like_NextPeriodData( long i1, long i2, long comp_nn )
{
	long	i ;
	double	dx1, dx2, dx ;

	if ( m_PeriodDa[i1].ff != m_PeriodDa[i2].ff )
		return ( false ) ;

	dx1 = dx2 = 0 ;
	for ( i=0; i<comp_nn; i++ )
	{
		dx1 += m_PeriodDa[i+i1].xx-m_PeriodDa[i+i1-1].xx ;
		dx2 += m_PeriodDa[i+i2].xx-m_PeriodDa[i+i2-1].xx ;
	}
	dx = fabs(dx2-dx1)/comp_nn ;
	return ( dx ) ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::push_NextPeriodData( long i, long next_ii )
{
	PeriodDa_Type	*sd ;
	sd = &m_PeriodDa[i] ;
	sd->next_ii = next_ii ;
	sd->next_ff = 0 ;
	sd->next_nn = 0 ;
	if ( m_next_si<0 )
		m_next_si = i ;
	m_next_ei = i ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::set_NextPeriodData( long i, long comp_nn, double max_dx, long max_dyTot )
{
	long	ii, dy_tot ;
	double	dx ;
	for ( ii=i+comp_nn; ii<m_PeriodDa_mm-comp_nn; ii++ )
	{
		dx = like_NextPeriodData( i, ii, comp_nn ) ;
//log_prt( g_logFile, "set_NextSegData2 i=%-8ld ii=%-8ld dx=%1.2lf max_dx=%1.2lf\r\n", i, ii, dx, max_dx ) ;
		if ( dx<=max_dx )
		{
			dy_tot = get_DyTotFromPeriodData( i, ii, comp_nn ) ;
//log_prt( g_logFile, "set_NextSegData2 i=%-8ld ii=%-8ld dx=%1.2lf max_dx=%1.2lf dy_tot=%-8ld max_dyTot=%-8ld\r\n", i, ii, dx, max_dx, dy_tot, max_dyTot ) ;
			if ( dy_tot<=max_dyTot )
			{
log_prt( g_logFile, "push_NextSegData2 i=%-8ld ii=%-8ld di=%-5ld zq_xx=%-10.2lf dx=%-10.2lf max_dx=%-10.2lf dy_tot=%-8ld max_dyTot=%-8ld\r\n", 
																				i, ii, ii-i, 
																				m_PeriodDa[ii].xx-m_PeriodDa[i].xx,
																				dx, max_dx, dy_tot, max_dyTot ) ;
				push_NextPeriodData( i, ii ) ;
				return (ii) ;
			}
		}
		if ( ii>i+50 )
			break ;
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_next_data( long comp_nn, double max_dx, long max_dyTot )
{
	long	i, f, m ;

	m_next_si = -1 ;
	m = 0 ;
	for ( i=comp_nn; i<m_PeriodDa_mm-comp_nn; i++ )
	{
		f = set_NextPeriodData( i, comp_nn, max_dx, max_dyTot ) ;
		if ( f<0 )
		{
			m ++ ;
			if ( m>300 )
				break ;
		}
		else
			m = 0 ;
	}
	make_next_ff() ;
	set_all_next_ff() ;

}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::push_next_ff( long si, long next_ii )
{
	long	i ;
	double	dx_val, dx ;

	dx_val = m_PeriodDa[next_ii].xx-m_PeriodDa[si].xx ;
	for ( i=si; i<m_PeriodDa_mm; )
	{
		next_ii = m_PeriodDa[i].next_ii ;
		dx = m_PeriodDa[next_ii].xx-m_PeriodDa[i].xx ;
		if ( fabs(dx-dx_val)>dx_val/10 )
			break ;
		m_PeriodDa[i].next_ff = m_next_ff ;
		i = next_ii ;
	}

	m_next_ff ++ ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_next_ff()
{
	long	i, next_ii ;

	m_next_ff = 1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_PeriodDa[i].next_ff>0 )
			continue ;
		if ( i<m_PeriodDa_mm-1 )
		{
			next_ii = m_PeriodDa[i].next_ii ;
			if ( next_ii>0 )
			{
				push_next_ff( i, next_ii ) ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_all_next_ff_ex( long next_ff, long val )
{
	long	i, nn ;
	nn = 1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( next_ff == m_PeriodDa[i].next_ff )
		{
			m_PeriodDa[i].next_ff += val ;
			m_PeriodDa[i].next_nn = nn++ ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_all_next_ff()
{
	long	i, k, next_ff ;

	next_ff = 0 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_PeriodDa[i].next_ff<1000 )
		{
			if ( m_PeriodDa[i].next_ff>0 )
			{
				next_ff = m_PeriodDa[i].next_ff ;
				k = m_PeriodDa[i].next_ii ;
				for ( ; k<m_PeriodDa_mm; )
				{
					if ( m_PeriodDa[k].next_ff==0 )
					{
						set_all_next_ff_ex( next_ff, 1000 ) ;
						break ;
					}
					else if ( next_ff == m_PeriodDa[k].next_ff )
					{
						k = m_PeriodDa[k].next_ii ;
					}
					else
					{
						set_all_next_ff_ex( next_ff, 2000 ) ;
						break ;
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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_period_data()
{

	m_PcmData_si = 0 ;
	m_PcmData_mm = 0 ;

	m_PeriodYY_m = 0 ;

	m_PeriodDa_mm = 0 ;
	m_PeriodDa_si = 0 ;

}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::reset_si_vars()
{
	long	i, ix ;
	for ( i=m_PeriodDa_mm-1; i>=0; i-- )
	{
		if ( m_PeriodDa[i].next_ff<1000 )
			continue ;
		ix = m_PeriodDa[i].ix ;
		if ( m_PcmData[ix]<0 && m_PcmData[ix+1]>=0 )
			break ;
	}
	m_PcmData_si = ix ;
	m_PeriodDa_si = m_PeriodDa_mm = i ;
}
//////////////////////////////////////////////////////////////////////
double TWavePeriod::make_period_data(short *pcm_data, long pcm_len, long show_flag )
{

	push_PcmData( pcm_data, pcm_len ) ;
	if ( show_flag>0 )
		show_pcm_data( m_PcmData, m_PcmData_mm, RGB(10,50,10), RGB(20,80,20) ) ;

log_prt( g_logFile, "make_period_data========================================================================m_PcmData_si=%-8ld m_PcmData_mm=%-8ld m_PeriodDa_si=%-8ld  m_PeriodDa_mm=%-8ld\r\n", m_PcmData_si, m_PcmData_mm, m_PeriodDa_si, m_PeriodDa_mm ) ;

	make_PeriodData() ;
	if ( show_flag>0 )
		show_PeriodData( 0, 5, RGB(80,0,50), 0, 0 ) ;

	remove_ShortPeriodData( 1-0.618 ) ; // remove too short Period data

	make_PeriodYY() ;

	make_next_data(1, 1, 5) ;

	if ( show_flag>0 )
		show_PeriodData( 0, 5, RGB(20,50,20), 0, RGB(100,10,10) ) ;

	make_tj_data(show_flag) ;

	reset_si_vars() ;

	if ( m_TJDa_m==1 ) 
		return ( m_TJDa[0].zq_val ) ;
	else
		return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::show_period_data()
{
	show_pcm_data( m_PcmData, m_PcmData_mm, RGB(10,50,10), RGB(20,80,20) ) ;
	show_PeriodData( 0, 5, RGB(20,50,20), RGB(0,30,0), RGB(100,10,10) ) ;
	print_tj_data( "finished" ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_TJDa()
{
	long	i ;
	for ( i=0; i<m_TJDa_m; i++ )
		m_TJDa->zq_tot = 0 ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::set_TJDa_len( long nn )
{
	TJDa_Type	*tj_data ;
	tj_data = (TJDa_Type*)GlobalAlloc( GPTR, (nn)*sizeof(TJDa_Type) ) ;
	if ( m_TJDa_n>0 && m_TJDa != NULL ) 
	{
		CopyMemory( tj_data, m_TJDa, m_TJDa_n*sizeof(TJDa_Type) ) ;
		FreeBuf( m_TJDa ) ;
	}
	m_TJDa = tj_data ;
	m_TJDa_n = nn ;

}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::quick_sort_tj_data( TJDa_Type *tj_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi ;
	long		Mid ;
	TJDa_Type	a_data ;

	iSize = sizeof( TJDa_Type ) ;

	Lo = iLo;
	Hi = iHi;
  
	Mid = (tj_data[Lo].zq_tot+tj_data[Hi].zq_tot) / 2;

	while ( true )
	{
		while ( tj_data[Lo].zq_tot < Mid )
			Lo++ ;
		while ( tj_data[Hi].zq_tot > Mid )
			Hi-- ;

		if ( Lo <= Hi )
		{
			memcpy( &a_data, &tj_data[Lo], iSize ) ;
			memcpy( &tj_data[Lo], &tj_data[Hi], iSize ) ;
			memcpy( &tj_data[Hi], &a_data, iSize ) ;
			Lo++;
			Hi--;

		}
		if ( Lo>Hi )
			break ;
    }

	if ( Hi > iLo )
		quick_sort_tj_data( tj_data, iLo, Hi);
	if ( Lo < iHi )
		quick_sort_tj_data( tj_data, Lo, iHi);
 
}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::sort_tj_data()
{
	quick_sort_tj_data( m_TJDa, 0, m_TJDa_m-1 ) ;
}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::reverse_tj_data()
{
	long	i, m, mm, iSize ;

	TJDa_Type tj_data ;

	mm = m_TJDa_m ;
	if ( mm<2 )
		return ;
	m = mm/2 ;
	iSize = sizeof( TJDa_Type ) ;
	for ( i=0; i<m; i++ )
	{
		memcpy( &tj_data, &m_TJDa[i], iSize ) ;
		memcpy( &m_TJDa[i], &m_TJDa[mm-i-1], iSize ) ;
		memcpy( &m_TJDa[mm-i-1], &tj_data, iSize ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::push_tj_data( double zq_dx, double zq_val )
{
	TJDa_Type	*tjd ;
	long	i ;

	for ( i=0; i<m_TJDa_m; i++ )
	{
		if ( fabs(zq_dx-m_TJDa[i].zq_dx)<=m_zq_dx && fabs(zq_val-m_TJDa[i].zq_val)<=m_zq_val )
		{
			m_TJDa[i].zq_sum += zq_val ;
			m_TJDa[i].zq_tot++ ;
			return ;
		}
	}
	i = m_TJDa_m++ ;
	if ( m_TJDa_m>=m_TJDa_n )
		set_TJDa_len( m_TJDa_n+10 ) ;

	tjd = &m_TJDa[i] ;

	tjd->zq_dx = zq_dx ;
	tjd->zq_val = zq_val ;
	tjd->zq_tot = 1 ;
	tjd->zq_sum = zq_val ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::avg_tj_data()
{
	long	i ;
	for ( i=0; i<m_TJDa_m; i++ )
	{
		if ( m_TJDa[i].zq_tot>0 )
			m_TJDa[i].zq_val = m_TJDa[i].zq_sum / m_TJDa[i].zq_tot ;
	}
}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::remove_tj_data( long min_tot )
{
	long	i, mm ;
	TJDa_Type *tj_data ;

	mm = m_TJDa_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJDa ;
	for ( i=1; i<mm; i++ )
	{
		if ( tj_data[i].zq_tot<=min_tot ) 
			break ;
	}
	m_TJDa_m = i ;
}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::remove_some_tj_data()
{
	long	i, mm, zq_tot ;
	TJDa_Type *tj_data ;

	mm = m_TJDa_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJDa ;
	zq_tot = tj_data[0].zq_tot ;
	for ( i=1; i<mm; i++ )
	{
		if ( tj_data[i].zq_tot<zq_tot/2 ) 
			break ;
	}
	m_TJDa_m = i ;
}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::group_tj_data()
{
	long	i, k, mm ;
	TJDa_Type *tj_data ;

	double	zq_val ;

	mm = m_TJDa_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJDa ;
	for ( i=0; i<mm; i++ )
	{
		zq_val = tj_data[i].zq_val ;
		for ( k=i+1; k<mm; k++ ) 
		{
			if ( tj_data[k].zq_tot<=0 )
				continue ;
			if ( fabs( zq_val-tj_data[k].zq_val )<=3 )
			{
				tj_data[i].zq_sum += tj_data[k].zq_sum ;
				tj_data[i].zq_tot += tj_data[k].zq_tot ;
				tj_data[i].zq_val = tj_data[i].zq_sum/tj_data[i].zq_tot ;
				zq_val = tj_data[i].zq_val ;
				tj_data[k].zq_tot = 0 ;
				tj_data[k].zq_sum = 0 ;
			}	
		}
	}
	avg_tj_data() ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_next_nn( long next_ff )
{
	long	i ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( next_ff==m_PeriodDa[i].next_ff )
		{
			m_PeriodDa[i].next_nn = 0 ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::make_TJDataEx( long nn )
{

	long	i, next_ff, max_nn ;

	max_nn = 0 ;
	next_ff = -1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_PeriodDa[i].next_ii<=0 )
			continue ;
		if ( max_nn < m_PeriodDa[i].next_nn )
		{
			max_nn = m_PeriodDa[i].next_nn ;
			next_ff = m_PeriodDa[i].next_ff ;
		}
	}
//log_prt( g_logFile, "make_TJData2==================nn=%-8ld max_nn=%-8ld next_ff=%-8ld\r\n", nn, max_nn, next_ff ) ;
	if ( next_ff>=0 )
	{
		if ( nn>0 && max_nn<nn )
		{
			clear_next_nn( next_ff ) ;
			return ( 0 ) ;
		}
		double	zq_dx, zq_val ;
		long	next_ii ;

		for ( i=m_next_si; i<=m_next_ei; i++ )
		{
			if ( m_PeriodDa[i].next_ii<=0 )
				continue ;
			if ( next_ff==m_PeriodDa[i].next_ff )
			{
				next_ii = m_PeriodDa[i].next_ii ;
				zq_dx = m_PeriodDa[i+1].xx-m_PeriodDa[i].xx ;
				zq_val = m_PeriodDa[next_ii].xx-m_PeriodDa[i].xx ;
//log_prt( g_logFile, "make_TJData2----i=%-8ld next_ii=%-8ld di=%-5ld zq_dx=%-11.1lf zq_val=%-11.1lf\r\n", i, next_ii, next_ii-i, zq_dx, zq_val ) ;
				push_tj_data( zq_dx, zq_val ) ;
			}
		}
		clear_next_nn( next_ff ) ;
		return ( max_nn ) ;
	}
	else
		return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_tj_data()
{
	if ( m_TJDa_n==0 )
		set_TJDa_len( 10 ) ;
	m_TJDa_m = 0 ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_TJData()
{

	clear_tj_data() ;

	long	i, nn, ff ;

	nn = make_TJDataEx( 0 ) ;
	for ( i=0; i<5; i++ )
	{
		ff = make_TJDataEx( nn/2 ) ;
		if ( ff<0 )
			break ;
	}
	avg_tj_data() ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::make_tj_data( long show_flag )
{

	if ( show_flag>0 )
		log_prt( g_logFile, "make_tj_data========================================================================%s\r\n", g_wavFile ) ;

	make_TJData() ;

	if ( show_flag>0 )
		log_prt( g_logFile, "make_TJData2==================\r\n" ) ;

	sort_tj_data() ;
	reverse_tj_data() ;
	if ( show_flag>0 )
		print_tj_data( "original" ) ;

	group_tj_data() ;
	if ( show_flag>0 )
		print_tj_data("group-1" ) ;

	group_tj_data() ;
	if ( show_flag>0 )
		print_tj_data("group-2" ) ;

	remove_some_tj_data() ;
	sort_tj_data() ;

	reverse_tj_data() ;
	if ( show_flag>0 )
		print_tj_data( "reverse-1" ) ;

}
/////////////////////////////////////////////////////////////////////////////
void TWavePeriod::shrink_tj_data()
{
	long	i, mm ;
	long	ii ;
	double	min_val, zq_val ;

	TJDa_Type *tj_data ;

	mm = m_TJDa_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJDa ;
	ii = -1 ;
	min_val = 10000000 ;
	for ( i=0; i<mm; i++ )
	{
		if ( min_val>tj_data[i].zq_val )
		{
			ii = i ;
			min_val = tj_data[i].zq_val ;
		}
	}
	if ( ii>=0 )
	{
		for ( i=0; i<mm; i++ )
		{
			if ( i==ii )
				continue ;
			if ( tj_data[i].zq_tot<=0 )
				continue ;

			zq_val = tj_data[i].zq_val ;
			if ( fabs( min_val-zq_val/2 )<0.2*min_val )
			{
				tj_data[ii].zq_tot += 2*tj_data[i].zq_tot ;
				tj_data[i].zq_tot = 0 ;
			}
			else if ( fabs( min_val-zq_val/3 )<0.2*min_val )
			{
				tj_data[ii].zq_tot += 3*tj_data[i].zq_tot ;
				tj_data[i].zq_tot = 0 ;
			}
			else if ( fabs( min_val-zq_val/4 )<0.2*min_val )
			{
				tj_data[ii].zq_tot += 4*tj_data[i].zq_tot ;
				tj_data[i].zq_tot = 0 ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::print_tj_data( char print_name[] )
{


	long	i ;
	double	zq_len ;
	TJDa_Type *tj_data ;

	tj_data = m_TJDa ;

	log_prt( g_logFile, "print_tj_data--------------tot=%ld ---- %s\r\n", m_TJDa_m, print_name ) ;
	zq_len = 0 ;
	for ( i=0; i<m_TJDa_m; i++ )
	{
		log_prt( g_logFile, "i=%-8ld dx=%-11.3lf val=%-11.3lf tot=%-8ld sum=%-11.3lf avg=%-11.1lf\r\n",
						i, 
						tj_data[i].zq_dx,
						tj_data[i].zq_val,
						tj_data[i].zq_tot,
						tj_data[i].zq_sum
						) ;
		zq_len += tj_data[i].zq_val*tj_data[i].zq_tot ;
	}
	m_zq_len = zq_len ;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
double TWavePeriod::get_max_zq_val()
{
	long	i, mm ;
	double	zq_val ;

	TJDa_Type *tj_data ;

	tj_data = m_TJDa ;
	mm = m_TJDa_m ;
	zq_val = 0 ;
	for ( i=0; i<mm; i++ )
	{
		if ( zq_val<tj_data[i].zq_val )
			zq_val = tj_data[i].zq_val ;
	}
	return ( zq_val ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TWavePeriod::clear_next_data()
{
	long	i ;
	for ( i=0; i<m_PeriodDa_mm; i++ )
	{
		m_PeriodDa[i].next_ii = 0;
		m_PeriodDa[i].next_ff = 0;
		m_PeriodDa[i].next_nn = 0;
	}
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::get_DyTot_by_zq( long i0, long i1, long i2 )
{
	long	i, nn ;
	double	dy_tot ;
	long	ii0, ii1 ;

	dy_tot = 0 ;

	nn = m_PeriodDa[i1].PeriodYY_si-m_PeriodDa[i0].PeriodYY_si ;
	if ( nn>m_PeriodDa[i2].PeriodYY_si-m_PeriodDa[i1].PeriodYY_si )
		nn = m_PeriodDa[i2].PeriodYY_si-m_PeriodDa[i1].PeriodYY_si ;
	ii0 = m_PeriodDa[i0].PeriodYY_si ;
	ii1 = m_PeriodDa[i1].PeriodYY_si ;
	for ( i=0; i<nn; i++ )
	{
		dy_tot += fabs(m_PeriodYY[i+ii0].yy-m_PeriodYY[i+ii1].yy ) ;
	}
	dy_tot /= m_FlatVV ;
	dy_tot /= nn ;
	return ( (long)dy_tot ) ;
}
//////////////////////////////////////////////////////////////////////
void TWavePeriod::push_NextData_by_zq( long i, long next_ii )
{

	PeriodDa_Type	*sd ;
	sd = &m_PeriodDa[i] ;

	sd->next_ii = next_ii ;
	sd->next_ff = 1001 ;
	sd->next_nn = m_next_nn++ ;
	
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::get_Idx_by_zq( long i, double zq_val, double max_dx )
{
	long	ii ;
	double	xx, dx ;

	xx = m_PeriodDa[i].xx ;
	for ( ii=i+1; ii<m_PeriodDa_mm; ii++ )
	{
		dx = m_PeriodDa[ii].xx-xx ;
		if ( fabs(dx-zq_val)<max_dx )
			return ( ii ) ;
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::get_PreIdx_by_zq( long i0, double zq_len, double max_dx )
{
	long	i, i1 ;
	for ( i=i0+1; i<m_PeriodDa_mm; i++ )
	{
		i1 = get_Idx_by_zq( i, zq_len, max_dx ) ;
		if ( i1>i )
			break ;
	}
	return ( i ) ;
}
//////////////////////////////////////////////////////////////////////
long TWavePeriod::make_next_data_by_zq( double zq_len, double max_dx, long max_dyTot )
{
	long	ff, i0, i1, i2, dy_tot ;

	clear_next_data() ;

	m_next_nn = 1 ;

	i0 = get_PreIdx_by_zq( 0, zq_len, max_dx ) ;
	i1 = get_Idx_by_zq( i0, zq_len, max_dx ) ;

	ff = 0 ;
	while ( i1>0 )
	{
		i2 = get_Idx_by_zq( i1, zq_len, max_dx ) ;
		if ( i2<0 )
		{
log_prt( g_logFile, "break---------------------i0=%-8ld i1=%-8ld\r\n", i0, i1 ) ;
			i0 = get_PreIdx_by_zq( i0, zq_len, max_dx ) ;
			i1 = get_Idx_by_zq( i0, zq_len, max_dx ) ;
			m_next_nn = 1 ;
			continue ;
		}
		else 
		{
			dy_tot = get_DyTot_by_zq( i0, i1, i2 ) ;
log_prt( g_logFile, "make_next_data3 i0=%-8ld i1=%-8ld i2=%-8ld dy_tot=%-8ld max_dyTot=%-8ld\r\n", i0, i1, i2, dy_tot, max_dyTot ) ;
			if ( dy_tot<=max_dyTot )
			{
				if ( ff==0 )
				{
					push_NextData_by_zq( i0, i1 ) ;
					ff = 1 ;
				}
				push_NextData_by_zq( i1, i2 ) ;
			}
			i0 = i1 ;
			i1 = i2 ;
		}
		if ( i1+i1-i0>=m_PeriodDa_mm )
			break ;
	}
	if ( m_PeriodDa_mm-i1<=i1-i0 )
		return ( i1-i0 );
	else
		return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
