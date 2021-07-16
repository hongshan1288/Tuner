// TJDataProc.cpp: implementation of the TJDataProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TJDataProc.h"
#include "ComUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TJDataProc::TJDataProc()
{
	ZeroMemory( m_TJ, sizeof(TJ_Type)*10 ) ;
	m_zq_val = 1.2 ;
	m_zq_dx = 1.2 ;
}
//////////////////////////////////////////////////////////////////////
TJDataProc::~TJDataProc()
{
	long	i ;
	for ( i=0; i<10; i++ )
		FreeBuf( m_TJ[i].tjData ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void TJDataProc::set_tj_name( long idx, char *aName )
{
	strcpy( m_TJ[idx].tj_name, aName ) ;
}
//////////////////////////////////////////////////////////////////////
void TJDataProc::set_TJData_Len( long idx, long nn )
{

	TJData_Type	*tj_data ;

	tj_data = m_TJ[idx].tjData ;

	tj_data = (TJData_Type*)GlobalAlloc( GPTR, (nn)*sizeof(TJData_Type) ) ;
	if ( m_TJ[idx].tjData_n>0 && m_TJ[idx].tjData != NULL ) 
	{
		CopyMemory( tj_data, m_TJ[idx].tjData, m_TJ[idx].tjData_n*sizeof(TJData_Type) ) ;
		FreeBuf( m_TJ[idx].tjData ) ;
	}
	m_TJ[idx].tjData = tj_data ;
	m_TJ[idx].tjData_n = nn ;

}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::clear_tjData( long idx )
{
	long	i ;
	for ( i=0; i<m_TJ[idx].tjData_m; i++ )
		m_TJ[idx].tjData->zq_tot = 0 ;
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::clear_tj_data( long idx )
{
	if ( m_TJ[idx].tjData_n==0 )
		set_TJData_Len( idx, 10 ) ;
	m_TJ[idx].tjData_m = 0 ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void	TJDataProc::push_tj_data( long idx, double zq_dx, double zq_val )
{
	TJData_Type	*tjd ;
	long	i ;

	for ( i=0; i<m_TJ[idx].tjData_m; i++ )
	{
		if ( fabs(zq_dx-m_TJ[idx].tjData[i].zq_dx)<=m_zq_dx && fabs(zq_val-m_TJ[idx].tjData[i].zq_val)<=m_zq_val )
		{
			m_TJ[idx].tjData[i].zq_sum += zq_val ;
			m_TJ[idx].tjData[i].zq_tot++ ;
			return ;
		}
	}
	i = m_TJ[idx].tjData_m++ ;
	if ( m_TJ[idx].tjData_m>=m_TJ[idx].tjData_n )
		set_TJData_Len( idx, m_TJ[idx].tjData_n+10 ) ;

	tjd = &m_TJ[idx].tjData[i] ;

	tjd->zq_dx = zq_dx ;
	tjd->zq_val = zq_val ;
	tjd->zq_tot = 1 ;

	tjd->zq_sum = zq_val ;
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::avg_tj_data( long idx )
{
	long	i ;
	for ( i=0; i<m_TJ[idx].tjData_m; i++ )
	{
		if ( m_TJ[idx].tjData[i].zq_tot>0 )
		{
			m_TJ[idx].tjData[i].zq_val = m_TJ[idx].tjData[i].zq_sum / m_TJ[idx].tjData[i].zq_tot ;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::set_pcm_len( long idx, long pcm_si, long pcm_ei ) 
{
	m_TJ[idx].pcm_si = pcm_si ;
	m_TJ[idx].pcm_ei = pcm_ei ;
	m_TJ[idx].pcm_len = pcm_ei-pcm_si ;
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::print_tjData( long idx, char print_name[] )
{
	log_prt( g_logFile, "print_tj_data--------------name=%s tot=%ld ---- %s\r\n", m_TJ[idx].tj_name, m_TJ[idx].tjData_m, print_name ) ;

	long	i ;
	double	zq_len ;
	TJData_Type *tj_data ;

	tj_data = m_TJ[idx].tjData ;
	zq_len = 0 ;
	for ( i=0; i<m_TJ[idx].tjData_m; i++ )
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
	m_TJ[idx].zq_len = zq_len ;
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::print_tj_data( long idx, char print_name[] )
{
	print_tjData( idx, print_name ) ;
}
//////////////////////////////////////////////////////////////////////
void	TJDataProc::print_tj_tot( long idx )
{
	log_prt( g_logFile, "print_tj_tot ****** name=%s tot=%ld pcm_si=%ld pcm_ei=%ld pcm_len=%ld zq_len=%1.1lf\r\n", 
												m_TJ[idx].tj_name, 
												m_TJ[idx].tjData_m,
												m_TJ[idx].pcm_si,
												m_TJ[idx].pcm_ei,
												m_TJ[idx].pcm_len,
												m_TJ[idx].zq_len
												) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void	TJDataProc::sort_tj_data( long idx )
{
	quickSort_TJData( m_TJ[idx].tjData, 0, m_TJ[idx].tjData_m-1 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::quickSort_TJData( TJData_Type *tj_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi ;
	long		Mid ;
	TJData_Type	a_data ;

	iSize = sizeof( TJData_Type ) ;

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
		quickSort_TJData( tj_data, iLo, Hi);
	if ( Lo < iHi )
		quickSort_TJData( tj_data, Lo, iHi);
 
}
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::reverse_tj_data( long idx )
{
	long	i, m, mm, iSize ;
	TJData_Type tj_data ;
	mm = m_TJ[idx].tjData_m ;
	if ( mm<2 )
		return ;
	m = mm/2 ;
	iSize = sizeof( TJData_Type ) ;
	for ( i=0; i<m; i++ )
	{
		memcpy( &tj_data, &m_TJ[idx].tjData[i], iSize ) ;
		memcpy( &m_TJ[idx].tjData[i], &m_TJ[idx].tjData[mm-i-1], iSize ) ;
		memcpy( &m_TJ[idx].tjData[mm-i-1], &tj_data, iSize ) ;
	}
}
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::remove_tj_data( long idx, long min_tot )
{
	long	i, mm, zq_tot ;

	mm = m_TJ[idx].tjData_m ;
	if ( mm<2 )
		return ;
	TJData_Type *tj_data ;

	tj_data = m_TJ[idx].tjData ;
	zq_tot = tj_data[0].zq_tot ;
	for ( i=1; i<mm; i++ )
	{
		if ( tj_data[i].zq_tot<=min_tot ) 
			break ;
	}
	m_TJ[idx].tjData_m = i ;
}
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::remove_some_tj_data( long idx )
{
	long	i, mm, zq_tot ;

	mm = m_TJ[idx].tjData_m ;
	if ( mm<2 )
		return ;
	TJData_Type *tj_data ;

	tj_data = m_TJ[idx].tjData ;
	zq_tot = tj_data[0].zq_tot ;
	for ( i=1; i<mm; i++ )
	{
		if ( tj_data[i].zq_tot<zq_tot/2 ) 
			break ;
	}
	m_TJ[idx].tjData_m = i ;
}
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::group_tj_data( long idx )
{
	long	i, k, mm ;
	double	zq_val ;
	TJData_Type *tj_data ;

	mm = m_TJ[idx].tjData_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJ[idx].tjData ;
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
				zq_val  = tj_data[i].zq_val ;
				tj_data[k].zq_tot = 0 ;
				tj_data[k].zq_sum = 0 ;
			}	
		}
	}
	avg_tj_data( idx ) ;
}
/////////////////////////////////////////////////////////////////////////////
void TJDataProc::shrink_tj_data( long idx )
{
	long	i, mm ;
	long	ii ;
	double	min_val, zq_val ;
	TJData_Type *tj_data ;

	mm = m_TJ[idx].tjData_m ;
	if ( mm<2 )
		return ;

	tj_data = m_TJ[idx].tjData ;
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
/////////////////////////////////////////////////////////////////////////////
double TJDataProc::get_max_zq_val( long idx )
{
	long	i, mm ;
	double	zq_val ;
	TJData_Type *tj_data ;

	tj_data = m_TJ[idx].tjData ;
	mm = m_TJ[idx].tjData_m ;
	zq_val = 0 ;
	for ( i=0; i<mm; i++ )
	{
		zq_val = tj_data[i].zq_val ;
		if ( zq_val<tj_data[i].zq_val )
			zq_val = tj_data[i].zq_val ;
	}
	return ( zq_val ) ;
}
//////////////////////////////////////////////////////////////////////
