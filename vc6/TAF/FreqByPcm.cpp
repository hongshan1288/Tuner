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
	m_FlatVV = 2000 ;

	// init mode data
	m_PcmModeData_m = 0 ;
	m_PcmModeData_n = 0 ;
	SetPcmModeDataLen( 1000 ) ;

//	m_pcm_data = NULL ;

	m_pcm_data = g_int_data ;

}
//////////////////////////////////////////////////////////////////////
TFreqByPcm::~TFreqByPcm()
{

}
//////////////////////////////////////////////////////////////////////
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
void TFreqByPcm::flat_pcm_data( short *pcm_data, long pcm_len )
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
void TFreqByPcm::show_PcmData( short *pcm_data, long pcm_len, long show_color )
{
	g_waveForm_xy_nn = make_WaveForm_Data( pcm_data, pcm_len, 0, g_waveForm_aa, g_waveForm_xy ) ;
	show_waveForm_data( 1, g_waveForm_xy, g_waveForm_xy_nn, 1, show_color, RGB(120,0,0) ) ; 
}
//////////////////////////////////////////////////////////////////////
long TFreqByPcm::GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc )
{
/*
	if ( check_pcm_data_is_a_noise( pcm_data, pcm_len ) )
		return ( -1 ) ;
*/

	show_PcmData( pcm_data, pcm_len, RGB(0,120,0) ) ;

//	FreeBuf( m_pcm_data ) ;

//	m_pcm_data = (short*)GlobalAlloc( GPTR, (pcm_len)*sizeof(short) ) ;

	CopyMemory( m_pcm_data, pcm_data, pcm_len*sizeof(short) ) ;
	m_pcm_len = pcm_len ;

	flat_pcm_data( m_pcm_data, m_pcm_len ) ;

	show_PcmData( m_pcm_data, m_pcm_len, RGB(120,120,0) ) ;

	show_PcmData( pcm_data, pcm_len, RGB(0,120,0) ) ;

//	make_pmd_data( m_pcm_data, m_pcm_len ) ;

//	FreeBuf( m_pcm_data ) ;

	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
