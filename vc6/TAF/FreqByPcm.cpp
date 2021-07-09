// FreqByPcm.cpp: implementation of the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreqByPcm.h"
#include "ComUtils.h"
#include "DoDrawUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TFreqByPcm::TFreqByPcm()
{

	m_MinVV = 1000 ;
	m_FlatVV = 3200/8 ;

	m_LineMode = PS_SOLID ;

	m_DC_line_color = RGB( 100, 0, 0 ) ;

	m_Period_dn = 1 ;

	m_PeriodData_m = 0 ;
	m_PeriodData_n = 0 ;
	Set_PeriodData_Len( 1000 ) ;

	m_SameData_m = 0 ;
	m_SameData_n = 0 ;
	set_SameData_Len( 500 ) ;

//	m_pcm_data = NULL ;

	m_DCFlag = false ;
	m_MemFlag = true ;

/**
	m_SegYY_m = 0 ;
	m_SegYY_n = 0 ;
	set_SegYY_Len( 1000 ) ;

	m_SegData_m = 0 ;
	m_SegData_n = 0 ;
	set_SegData_Len( 100 ) ;
**/

	m_TJProc = new TJDataProc ;
	m_Make1 = new TMakeZQData_1 ;

}
//////////////////////////////////////////////////////////////////////
TFreqByPcm::~TFreqByPcm()
{
	delete m_TJProc ;
	delete m_Make1 ;
}
//////////////////////////////////////////////////////////////////////
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
long TFreqByPcm::make_WaveFormData( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data )
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

//	xy_data[ii].x = x0 + xxx ;
//	xy_data[ii].y = get_waveForm_y( 0, y0 ) ; ;
//	ii++ ;

	return ( ii ) ;

}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_xy_buf( long aa )
{
	m_waveForm_xy_size = aa*2+100 ;
	m_waveForm_xy = (POINT*)GlobalAlloc( GPTR, m_waveForm_xy_size*sizeof(POINT) ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::show_PcmData( short *pcm_data, long pcm_len, long show_color, long dot_color )
{
	long xy_nn = make_WaveFormData( pcm_data, pcm_len, 0, g_waveForm_aa, m_waveForm_xy ) ;
	draw_WaveForm_data( 1, m_waveForm_xy, xy_nn, 1, show_color, dot_color ) ; 
}
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
		sprintf( ss, "%ld-%ld", i, ix ) ;
		if ( ( i % 2 )==0 )
			draw_VerticalLine( ix, 0, -5, RGB( 50, 50, 10 ), ss ) ;
		else
			draw_VerticalLine( ix, 0, -20, RGB( 50, 50, 10 ), ss ) ;
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
//	if ( in_tot<=m_Max_In*pd_nn && dy_sum_tot<=m_Max_DySum*pd_nn )
	if ( in_tot<=m_Max_In && dy_sum_tot<=m_Max_DySum*pd_nn )
		return ( true ) ;
	else
		return ( false ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::push_SameNextData( long i, long next_ii, long idx )
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
				push_SameNextData( i, ii, 0 ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_SameData( long n )
{
	long	i, f, m ;

	m_Max_In = 3 ;
	m_Max_DySum = 5 ;
	m_Max_DyTot = 2 ;

	m_SameData_m = 0 ;
	m = 0 ;
	for ( i=n; i<m_PeriodData_m; i++ )
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
				push_SameNextData( i, ii, 1 ) ;
				return (ii) ;
			}
		}
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
void TFreqByPcm::make_SameData2(long n)
{
	long	i, m, f ;

	m_Max_In = 3 ;
	m_Max_DySum = 5 ;
	m_Max_DyTot = 3 ;
	m_Max_DyTot2 = 0.618 ;

	m_SameData_m = 0 ;
	m = 0 ;
	for ( i=n; i<m_PeriodData_m; i++ )
	{
		f = get_SameData2( i, n ) ;
		if ( f<0 )
		{
			m ++ ;
			if ( m>3 )
			{
				if ( m_SameData_m>3 )
					break ;
			}
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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc )
{

	if ( check_pcm_data_is_a_noise( pcm_data, pcm_len ) )
		return ( -1 ) ;

	m_dx = g_waveForm_aa ;
	m_dx /= pcm_len ;

	m_pcm_data = (short*)GlobalAlloc( GPTR, (pcm_len)*sizeof(short) ) ;
//	g_dy_data = (char*)GlobalAlloc( GPTR, (pcm_len)*sizeof(char) ) ;

	__try
	{
/**
		CopyMemory( m_pcm_data, pcm_data, pcm_len*sizeof(short) ) ;
		m_pcm_len = pcm_len ;
		show_PcmData( m_pcm_data, m_pcm_len, RGB(0,20,0), RGB(0,50,0) ) ;

		make_flat_data( m_pcm_data, m_pcm_len ) ;

		make_PeriodData( m_pcm_data, m_pcm_len ) ;

		make_dy_data( m_pcm_data, m_pcm_len ) ;
		make_PeriodData_dySum() ;

//		show_dy_data( RGB(40,40,50) ) ;

		make_SameData(1) ;
		make_SameData2(1) ;

		show_PcmData( m_pcm_data, m_pcm_len, RGB(50,50,50), RGB(100,100,100) ) ;
		show_PeriodData( RGB(80,20,20) ) ;
		show_NextData( 0, 20, RGB(80,20,120), PS_SOLID ) ;
		show_NextData( 1, 40, RGB(20,80,80), PS_DASH ) ;
**/

		CopyMemory( m_pcm_data, pcm_data, pcm_len*sizeof(short) ) ;
		m_pcm_len = pcm_len ;

		make_flat_data( m_pcm_data, m_pcm_len ) ;
		show_PcmData( m_pcm_data, m_pcm_len, RGB(50,50,50), RGB(100,100,100) ) ;

		MakeZQData_proc1() ;
/**
		make_SegData( m_pcm_data, m_pcm_len ) ;

//		show_SegData( 0, RGB(50,20,20), 0 ) ;

		remove_SomeSegData( 1-0.618 ) ;
//		set_MaxDxSegData( 0.85 ) ;

		make_SegYY( 1 ) ;
		make_NextSegData( 0, 3, 3.0, 5 ) ;

		show_SegData( 1, RGB(50,50,20), 1 ) ;
		show_NextSegData( 0, 20, RGB(80,80,50), PS_DASH ) ;
**/
	}
	__finally
	{
		FreeBuf( m_pcm_data ) ;
//		FreeBuf( g_dy_data ) ;
	}

	return ( 0 ) ;

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
void TFreqByPcm::MakeZQData_proc1()
{
	m_Make1->make_seg_data() ;
/**
	// 2-3-5
	m_Make1->make_next_data( 2, 1, 5 ) ;
	m_Make1->show_seg_data( 0, 5, RGB(50,50,20), 1 ) ;
	m_Make1->make_tj_data( m_TJProc, 0, "SegDatTJ-2-3-5" ) ;

	m_TJProc->print_tj_data( 0, 0 ) ;
	m_TJProc->sort_tj_data( 0 ) ;
	m_TJProc->print_tj_data( 0, 0 ) ;
**/

	// make_next_data2 by dy
	m_Make1->make_next_data2( 1, 2, 1 ) ;
//	m_Make1->show_seg_data( 0, 5, RGB(50,50,20), 1 ) ;
	m_Make1->make_tj_data( m_TJProc, 0, "SegDatTJ-2-3-5" ) ;

	// to do
	// 按照最大周期再找SegData中进行一次计算
	double max_zq_val ;
	max_zq_val = m_TJProc->get_max_zq_val( 0 ) ;
	m_Make1->make_next_data3( max_zq_val, 5, 10 ) ;
	m_Make1->show_seg_data( 0, 5, RGB(50,50,20), 1 ) ;
	m_Make1->make_tj_data( m_TJProc, 0, "last" ) ;

//	m_TJProc->print_tj_data( 0, 0 ) ;

//	m_TJProc->sort_tj_data( 0 ) ;
//	m_TJProc->print_tj_data( 0, 0 ) ;

//	m_TJProc->reverse_tj_data( 0 ) ;
//	m_TJProc->print_tj_data( 0, 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

