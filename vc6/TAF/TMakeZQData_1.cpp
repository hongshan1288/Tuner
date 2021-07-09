// TMakeZQData_1.cpp: implementation of the TMakeZQData_1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TMakeZQData_1.h"
#include "ComUtils.h"
#include "DoDrawUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TMakeZQData_1::TMakeZQData_1()
{
	m_SegYY_m = 0 ;
	m_SegYY_n = 0 ;
	set_SegYY_Len( 1000 ) ;

	m_SegData_m = 0 ;
	m_SegData_n = 0 ;
	set_SegData_Len( 100 ) ;
}
//////////////////////////////////////////////////////////////////////
TMakeZQData_1::~TMakeZQData_1()
{
	FreeBuf( m_SegYY ) ;
	FreeBuf( m_SegData ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::set_SegData_Len( long nn )
{
	SegData_Type	*sd ;
	sd = (SegData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(SegData_Type) ) ;
	if ( m_SegData_n>0 && m_SegData != NULL ) 
	{
		CopyMemory( sd, m_SegData, m_SegData_n*sizeof(SegData_Type) ) ;
		FreeBuf( m_SegData ) ;
	}
	m_SegData = sd ;
	m_SegData_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::push_SegData( short ff, long ix, double xx )
{

	SegData_Type	*sd ;

	sd = m_SegData ;
	sd += m_SegData_m++ ;

	sd->xx = xx ;
	sd->ix = ix ;
	sd->ff = ff ;
	sd->next_ii[0] = sd->next_ii[1] = 0 ;

	if ( m_SegData_m>=m_SegData_n )
		set_SegData_Len( m_SegData_n+100 ) ;

	return ( m_SegData_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
inline long TMakeZQData_1::get_sign( long yy )
{
	if ( yy>0 )
		return ( 1 ) ;
	else if ( yy<0 )
		return ( -1 ) ;
	else
		return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
double TMakeZQData_1::get_xx( long i, long y0, long yy )
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
short TMakeZQData_1::get_yy( double xx )
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
void TMakeZQData_1::make_SegData( short *pcm_data, long pcm_len )
{
	long	i, ff, yy, y0 ;
	double	xx ;

	m_SegData_m = 0 ;

	y0 = pcm_data[0];
	ff = get_sign( y0 ) ;
	for ( i=1; i<pcm_len; i++ )
	{
		yy = pcm_data[i];
		if ( m_SegData_m==0 )
		{
			if ( y0<0 && yy>=0 ) // 第一个点一定是从负值到正值（包括0）
			{
				xx = get_xx( i, y0, yy ) ;
				push_SegData( 1001, i-1, xx ) ;
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
					push_SegData( 1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
			else if ( ff<0 )
			{
				if ( yy>0 )
				{
					xx = get_xx( i, y0, yy ) ;
					push_SegData( -1, i-1, xx ) ;
					ff = get_sign( yy ) ;
				}
			}
		}
		y0 = yy ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::remove_TooShortDxSegData( double min_dx )
{
	long	i, ii, nn ;
	double	dx, dx_avg, dx_max ;

	// 1. 计算dx的平均值
	dx_avg = 0 ;
	dx_max = 0 ;
	for ( i=1; i<m_SegData_m; i++ )
	{
		dx = m_SegData[i].xx-m_SegData[i-1].xx ;
		if ( dx_max<dx )
			dx_max = dx ;
		dx_avg += dx ;
	}
	dx_avg /= m_SegData_m-1 ;
	dx_avg *= min_dx ;

	// 2. 去除小于平均值百分比（0.2）的点
	ii = 0 ;
	for ( i=1; i<m_SegData_m; )
	{
		dx = m_SegData[i].xx-m_SegData[ii].xx ;
		if ( dx<dx_avg || dx<dx_max/3 )
		{
			nn = m_SegData_m-i ;
			CopyMemory( &m_SegData[i], &m_SegData[i+1], nn*sizeof(SegData_Type) ) ;
			m_SegData_m -- ;
		}
		else
		{
			ii = i ;
			i ++ ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::set_SegYY_Len( long nn )
{
	SegYY_Type	*sd ;
	sd = (SegYY_Type*)GlobalAlloc( GPTR, (nn)*sizeof(SegYY_Type) ) ;
	if ( m_SegYY_n>0 && m_SegYY != NULL ) 
	{
		CopyMemory( sd, m_SegYY, m_SegYY_n*sizeof(SegYY_Type) ) ;
		FreeBuf( m_SegYY ) ;
	}
	m_SegYY = sd ;
	m_SegYY_n = nn ;
}
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::push_SegYY( long xx, short yy )
{

	SegYY_Type	*sd ;

	sd = m_SegYY ;
	sd += m_SegYY_m++ ;

	sd->xx = xx ;
	sd->yy = yy ;
	sd->dy = 0 ;

	if ( m_SegYY_m>=m_SegYY_n )
		set_SegYY_Len( m_SegYY_n+1000 ) ;

	return ( m_SegYY_m-1 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_SegDY( long ii )
{
	long	i, si, nn ;

	si = m_SegData[ii].SegYY_si ;
	nn = m_SegData[ii].SegYY_nn ;
	for ( i=0; i<nn; i++ )
		m_SegYY[i+si].dy = (short)((m_SegYY[i+si].yy-m_SegYY[i+si-1].yy)/m_FlatVV) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_SegYY()
{
	long	i, nn ;
	short	yy ;
	double	xx_tot, xx, x0, dx ;

	m_SegYY_m = 0 ;
	dx = 1 ;
	for ( i=0; i<m_SegData_m-1; i++ )
	{
		push_SegYY( 0, 0 ) ; //原点肯定是0
		m_SegData[i].SegYY_si = m_SegYY_m ;
		x0 = m_SegData[i].xx ;
		xx_tot = m_SegData[i+1].xx-x0 ;
		nn = 1 ;
		for ( xx=dx; xx<xx_tot; xx+=dx )
		{
			yy = get_yy( x0+xx ) ;	
			push_SegYY( nn++, yy ) ;
		}
		m_SegData[i].SegYY_nn = nn ;
		make_SegDY( i ) ;
	}
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::show_SegYY( long idx, long show_color )
{
	long	i, nn, si, dy, x, y ;
	double	x0, xx ;
	short	yy ;
	char	ss[50] ;

	m_LineMode = PS_DASH ;

	si = m_SegData[idx].SegYY_si ;
	nn = m_SegData[idx].SegYY_nn ;
	x0 = m_SegData[idx].xx ;
	for ( i=0; i<nn; i++ )
	{
		xx = m_SegYY[i+si].xx ;
		yy = m_SegYY[i+si].yy ;
		x = draw_VLine( x0+xx, 0, yy, 0, dy, show_color, "" ) ;

		if ( yy>0 )
			dy = -5 ;
		else
			dy = 5 ;

//		sprintf( ss, "%d(%d)", m_SegYY[i+si].yy, m_SegYY[i+si].dy ) ;
		sprintf( ss, "%d", m_SegYY[i+si].dy ) ;

		y = get_waveForm_y( yy, g_waveForm_bb / 2 ) ;
		do_DrawText( ss, x, y, show_color, g_waveForm_backColor, 0, dy ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::get_DyTotFromSegData( long i1, long i2, long comp_nn )
{
	long	i, nn ;
	double	dy_tot ;
	long	ii1, ii2 ;

	dy_tot = 0 ;

	nn = m_SegData[i1].SegYY_si-m_SegData[i1-comp_nn].SegYY_si ;
	ii1 = m_SegData[i1-comp_nn].SegYY_si ;
	ii2 = m_SegData[i2-comp_nn].SegYY_si ;
	for ( i=0; i<nn; i++ )
	{
		dy_tot += fabs(m_SegYY[i+ii1].yy-m_SegYY[i+ii2].yy ) ;
	}
	dy_tot /= m_FlatVV ;
	dy_tot /= nn ;
	return ( (long)dy_tot ) ;
}
//////////////////////////////////////////////////////////////////////
bool TMakeZQData_1::like_NextSegData( long i1, long i2, long comp_nn, double max_dx )
{
	long	i ;
	double	dx1, dx2 ;

	dx1 = dx2 = 0 ;
	for ( i=0; i<comp_nn; i++ )
	{
		dx1 += m_SegData[i+i1].xx-m_SegData[i+i1-1].xx ;
		dx2 += m_SegData[i+i2].xx-m_SegData[i+i2-1].xx ;
	}
	if ( fabs(dx2-dx1)/comp_nn<=max_dx )
		return ( true ) ;
	else
		return ( false ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::push_NextSegData( long i, long next_ii )
{
	SegData_Type	*sd ;
	sd = &m_SegData[i] ;
	sd->next_ii[0] = next_ii ;
}
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::set_NextSegData( long i, long comp_nn, double max_dx, long max_dyTot )
{
	long	ii, dy_tot ;
	for ( ii=i+comp_nn; ii<m_SegData_m; ii++ )
	{
		if ( like_NextSegData( i, ii, comp_nn, max_dx ) )
		{
			dy_tot = get_DyTotFromSegData( i, ii, comp_nn ) ;
			if ( dy_tot<=max_dyTot )
			{
				push_NextSegData( i, ii ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_next_data( long comp_nn, double max_dx, long max_dyTot )
{
	long	i, f, m ;

	m = 0 ;
	for ( i=comp_nn; i<m_SegData_m; i++ )
	{
		f = set_NextSegData( i, comp_nn, max_dx, max_dyTot ) ;
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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::get_DyTotFromSegData2( long i1, long i2, long comp_nn )
{
	long	i, nn, tot ;
	long	ii1, ii2 ;

	tot = 0 ;
	nn = m_SegData[i1].SegYY_si-m_SegData[i1-comp_nn].SegYY_si ;
	ii1 = m_SegData[i1-comp_nn].SegYY_si ;
	ii2 = m_SegData[i2-comp_nn].SegYY_si ;
	for ( i=0; i<nn; i++ )
	{
		if ( fabs(m_SegYY[i+ii1].dy-m_SegYY[i+ii2].dy )<=2 )
			tot ++ ;
	}
	if ( tot>0 )
		return ( nn/tot ) ;
	else
		return ( 10000 ) ;
}
//////////////////////////////////////////////////////////////////////
bool TMakeZQData_1::like_NextSegData2( long i1, long i2, long comp_nn, double max_dx )
{
	long	i ;
	double	dx1, dx2, dx ;

	if ( m_SegData[i1].ff != m_SegData[i2].ff )
		return ( false ) ;

	dx1 = dx2 = 0 ;
	for ( i=0; i<comp_nn; i++ )
	{
		dx1 += m_SegData[i+i1].xx-m_SegData[i+i1-1].xx ;
		dx2 += m_SegData[i+i2].xx-m_SegData[i+i2-1].xx ;
	}
	dx = fabs(dx2-dx1)/comp_nn ;
	if ( dx<=max_dx )
		return ( true ) ;
	else
		return ( false ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::push_NextSegData2( long i, long next_ii )
{
	SegData_Type	*sd ;
	sd = &m_SegData[i] ;
	sd->next_ii[0] = next_ii ;
	sd->next_ff[0] = 0 ;
	sd->next_nn[0] = 0 ;
	if ( m_next_si<0 )
		m_next_si = i ;
	m_next_ei = i ;
}
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::set_NextSegData2( long i, long comp_nn, double max_dx, long max_dyTot )
{
	long	ii, dy_tot ;
	for ( ii=i+comp_nn; ii<m_SegData_m-comp_nn; ii++ )
	{
		if ( like_NextSegData2( i, ii, comp_nn, max_dx ) )
		{
			dy_tot = get_DyTotFromSegData2( i, ii, comp_nn ) ;
			if ( dy_tot<=max_dyTot )
			{
				push_NextSegData2( i, ii ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_next_data2( long comp_nn, double max_dx, long max_dyTot )
{
	long	i, f, m ;

	m_next_si = -1 ;
	m = 0 ;
	for ( i=comp_nn; i<m_SegData_m-comp_nn; i++ )
	{
		f = set_NextSegData2( i, comp_nn, max_dx, max_dyTot ) ;
		if ( f<0 )
		{
			m ++ ;
			if ( m>3 )
				break ;
		}
		else
			m = 0 ;
	}
	make_next_ff() ;
	set_all_next_ff() ;

}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::push_next_ff( long si, long next_ii )
{
	long	i ;
	double	dx_val, dx ;

	dx_val = m_SegData[next_ii].xx-m_SegData[si].xx ;
	for ( i=si; i<m_SegData_m; )
	{
		next_ii = m_SegData[i].next_ii[0] ;
		dx = m_SegData[next_ii].xx-m_SegData[i].xx ;
		if ( fabs(dx-dx_val)>dx_val/10 )
			break ;
		m_SegData[i].next_ff[0] = m_next_ff ;
		i = next_ii ;
	}

	m_next_ff ++ ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_next_ff()
{
	long	i, next_ii ;

	m_next_ff = 1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_SegData[i].next_ff[0]>0 )
			continue ;
		if ( i<m_SegData_m-1 )
		{
			next_ii = m_SegData[i].next_ii[0] ;
			if ( next_ii>0 )
			{
				push_next_ff( i, next_ii ) ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::set_all_next_ff_ex( long next_ff, long val )
{
	long	i, nn ;
	nn = 1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( next_ff == m_SegData[i].next_ff[0] )
		{
			m_SegData[i].next_ff[0] += val ;
			m_SegData[i].next_nn[0] = nn++ ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::set_all_next_ff()
{
	long	i, k, next_ff ;

	next_ff = 0 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_SegData[i].next_ff[0]<1000 )
		{
			if ( m_SegData[i].next_ff[0]>0 )
			{
				next_ff = m_SegData[i].next_ff[0] ;
				k = m_SegData[i].next_ii[0] ;
				for ( ; k<m_SegData_m; )
				{
					if ( m_SegData[k].next_ff[0]==0 )
					{
						set_all_next_ff_ex( next_ff, 1000 ) ;
						break ;
					}
					else if ( next_ff == m_SegData[k].next_ff[0] )
					{
						k = m_SegData[k].next_ii[0] ;
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
void TMakeZQData_1::show_seg_data( long di, long y_off, long show_color, long show_yy )
{
	long	i, ix, next_ii, next_color ;
	long	x, y ;
	double	xx, dx ;
	char	ss[50] ;

	m_LineMode = PS_SOLID ;
	next_color = RGB(30,30,30) ;
	for ( i=0; i<m_SegData_m; i++ )
	{
		if ( i==0 )
			dx = 0 ;
		else
			dx = m_SegData[i].xx-m_SegData[i-1].xx ;
		xx = m_SegData[i].xx ;
		ix = m_SegData[i].ix ;
		sprintf( ss, "%ld-%ld-%1.1lf", i, ix, dx ) ;
		if ( ( i % 2 )==0 )
			draw_VLine( xx, 0, -32000, di, -y_off, show_color, ss ) ;
		else
			draw_VLine( xx, 0, -32000, di, -(y_off+10), show_color, ss ) ;

		if ( show_yy>0 )
		{
			if ( i<m_SegData_m )
				show_SegYY( i, RGB(20,80,20) ) ;
		}
		if ( i<m_SegData_m-1 )
		{
			next_ii = m_SegData[i].next_ii[0] ;
			if ( next_ii>0 )
			{
				sprintf( ss, "%ld-%ld-%1.1lf", i, next_ii, m_SegData[next_ii].xx-xx ) ;
				if ( ( i % 2 )==0 )
					x = draw_VLine( xx, 0, -32000, di, -(y_off+20), show_color+next_color, ss ) ;
				else
					x = draw_VLine( xx, 0, -32000, di, -(y_off+35), show_color+next_color, ss ) ;

				if ( m_SegData[i].next_ff[0]>1000 )
				{
					sprintf( ss, "%ld-%ld", m_SegData[i].next_ff[0], m_SegData[i].next_nn[0] ) ;
					y = get_waveForm_y( -32000, g_waveForm_bb / 2 ) ;
					do_DrawText( ss, x, y, RGB(80,10,10), g_waveForm_backColor, 10, -60 ) ;
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_seg_data()
{

	make_SegData( m_pcm_data, m_pcm_len ) ;
	remove_TooShortDxSegData( 1-0.618 ) ;
	make_SegYY() ;

}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_TJData( TJDataProc *aTJProc, long idx, char *tjName )
{

	aTJProc->set_tj_name( idx, tjName ) ;
	aTJProc->clear_tj_data( idx ) ;

	double	zq_dx, zq_val ;
	long	i, next_ii ;

	for ( i=0; i<m_SegData_m-1; i++ )
	{
		next_ii = m_SegData[i].next_ii[0] ;
		if ( next_ii>0 )
		{
			zq_dx = m_SegData[i+1].xx-m_SegData[i].xx ;
			zq_val = m_SegData[next_ii].xx-m_SegData[i].xx ;
			aTJProc->push_tj_data( idx, zq_dx, zq_val ) ;
		}
	}

	long	pcm_si, pcm_ei ;
	pcm_si = m_SegData[0].ix ;
	pcm_ei = m_SegData[m_SegData_m-1].ix ;

	aTJProc->set_pcm_len( idx, pcm_si, pcm_ei ) ;

}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_tj_data( TJDataProc *aTJProc, long idx, char *tjName )
{

	log_prt( g_logFile, "make_tj_data========================================================================%s\r\n", g_wavFile ) ;
/**
	make_TJData( aTJProc, idx, tjName ) ;
	aTJProc->print_tj_data( 0, "original" ) ;

	aTJProc->sort_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "sort-1" ) ;

	aTJProc->reverse_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "reverse-1" ) ;

	aTJProc->group_tj_data( 0 ) ;

	aTJProc->sort_tj_data( 0 ) ;
	aTJProc->reverse_tj_data( 0 ) ;

	aTJProc->print_tj_data( 0, "group-sort-reversed" ) ;

	aTJProc->remove_some_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "remove_some" ) ;

	aTJProc->shrink_tj_data( 0 ) ;
	aTJProc->sort_tj_data( 0 ) ;
	aTJProc->reverse_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "shrink-sort-reverse" ) ;

	aTJProc->remove_some_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "remove_some" ) ;

**/

	make_TJData2( aTJProc, 0, tjName ) ;
	log_prt( g_logFile, "make_TJData2==================\r\n" ) ;

	aTJProc->print_tj_data( 0, "original" ) ;

	aTJProc->group_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "group" ) ;

	aTJProc->remove_some_tj_data( 0 ) ;

	aTJProc->sort_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "sort-1" ) ;

	aTJProc->reverse_tj_data( 0 ) ;
	aTJProc->print_tj_data( 0, "reverse-1" ) ;

//	aTJProc->sort_tj_data( 0 ) ;
//	aTJProc->reverse_tj_data( 0 ) ;
//	aTJProc->print_tj_data( 0, "group-sort-reversed" ) ;

//	aTJProc->print_tj_data( 0, "remove_some" ) ;


}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::clear_next_nn( long next_ff )
{
	long	i ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( next_ff==m_SegData[i].next_ff[0] )
		{
			m_SegData[i].next_nn[0] = 0 ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
long TMakeZQData_1::make_TJData2Ex( TJDataProc *aTJProc, long idx, long nn )
{

	long	i, next_ff, max_nn ;

	max_nn = 0 ;
	next_ff = -1 ;
	for ( i=m_next_si; i<=m_next_ei; i++ )
	{
		if ( m_SegData[i].next_ii[0]<=0 )
			continue ;
		if ( max_nn < m_SegData[i].next_nn[0] )
		{
			max_nn = m_SegData[i].next_nn[0] ;
			next_ff = m_SegData[i].next_ff[0] ;
		}
	}
log_prt( g_logFile, "make_TJData2==================nn=%ld max_nn=%ld next_ff=%ld\r\n", nn, max_nn, next_ff ) ;
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
			if ( m_SegData[i].next_ii[0]<=0 )
				continue ;
			if ( next_ff==m_SegData[i].next_ff[0] )
			{
				next_ii = m_SegData[i].next_ii[0] ;
				zq_dx = m_SegData[i+1].xx-m_SegData[i].xx ;
				zq_val = m_SegData[next_ii].xx-m_SegData[i].xx ;
log_prt( g_logFile, "i=%ld next_ii=%ld zq_dx=%1.1lf zq_val=%1.1lf\r\n", i, next_ii, zq_dx, zq_val ) ;
				aTJProc->push_tj_data( idx, zq_dx, zq_val ) ;
			}
		}
		clear_next_nn( next_ff ) ;
		return ( max_nn ) ;
	}
	else
		return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void TMakeZQData_1::make_TJData2( TJDataProc *aTJProc, long idx, char *tjName )
{
	aTJProc->set_tj_name( idx, tjName ) ;
	aTJProc->clear_tj_data( idx ) ;

	long	i, nn, ff ;

	nn = make_TJData2Ex( aTJProc, idx, 0 ) ;
	for ( i=0; i<5; i++ )
	{
		ff = make_TJData2Ex( aTJProc, idx, nn/2 ) ;
		if ( ff<0 )
			break ;
	}
	aTJProc->avg_tj_data( idx ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
