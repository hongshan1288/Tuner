// TWavePeriod2.h: interface for the TWavePeriod2 class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_TWAVEPERIOD2_H__7F3FC9FE_B3B0_40D6_981E_B1C6B460AAC1__INCLUDED_)
#define AFX_TWAVEPERIOD2_H__7F3FC9FE_B3B0_40D6_981E_B1C6B460AAC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WavePeriodDT.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
struct	PeriodDa_Type2
{
	long	ix ;
	short	ff, vv ;
	long	next_ii ;
	long	next_ff ;
	long	next_nn ;
} ;
//////////////////////////////////////////////////////////////////////
class TWavePeriod2
{

private:


	// pcm data
	double	m_MinVV ;
	long	m_FlatVV ;
	long	m_DC_line_color ;

	short	*m_PcmData ;
	long	m_PcmData_si, m_PcmData_mm, m_PcmData_nn ;
	void make_flat_data( short *pcm_data, long pcm_len ) ;
	void push_PcmData(short *pcm_data, long pcm_len) ;

	long draw_VLine( double xx, long y1, long y2, long xoff, long fx, long show_color, char *sa ) ;
	void draw_WaveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dot_color ) ;
	long make_WaveFormData( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data ) ;



	// PeriodDa
	PeriodDa_Type2	*m_PeriodDa ;
	long		m_PeriodDa_si, m_PeriodDa_mm, m_PeriodDa_nn ;

	void	set_PeriodDa_Len( long nn ) ;
	long	push_PeriodDa( short ff, long ix ) ;

	void clear_next_data() ;
	
	long get_sign( long yy ) ;

	void make_PeriodData() ;
	void remove_ShortPeriodData( double min_dx ) ;
	void show_PeriodData( long di, long y_off, long show_color, long yy_color, long next_color ) ;



	// PeriodYY
	void show_PeriodYY( long i, long show_color ) ;


	// nextData
	long	m_next_si, m_next_ei, m_next_ff ;
	void set_next_data( long comp_nn, double max_dx, long max_dyTot ) ;
	void make_next_data( long comp_nn, double max_dx, long max_dyTot ) ;
	long set_NextPeriodData( long i, long comp_nn, double max_dx, long max_dyTot ) ;
	void push_NextPeriodData( long i, long next_ii ) ;
	double like_NextPeriodData( long i1, long i2, long comp_nn ) ;
	long get_DyTotFromPeriodData( long i1, long i2, long comp_nn ) ;

	void push_next_ff( long si, long next_ii ) ;
	void make_next_ff() ;
	void set_all_next_ff() ;
	void set_all_next_ff_ex( long next_ff, long val ) ;

	void reset_si_vars() ;


	// TJData
	double	m_zq_val, m_zq_dx, m_zq_len ;

	long			m_TJDa_m, m_TJDa_n ;
	TJDa_Type		*m_TJDa ;

	void clear_TJDa() ;
	void set_TJDa_len( long nn ) ;

	void quick_sort_tj_data( TJDa_Type *tj_data, long iLo, long iHi ) ;
	void reverse_tj_data() ;

	void push_tj_data( double zq_dx, double zq_val ) ;
	void avg_tj_data() ;

	void remove_tj_data( long min_tot ) ;
	void remove_some_tj_data() ;
	void group_tj_data() ;
	void shrink_tj_data() ;
	double get_max_zq_val() ;

	void sort_tj_data() ;

	void clear_next_nn( long next_ff ) ;
	long make_TJDataEx( long nn ) ;
	void make_TJData() ;

	void clear_tj_data() ;
	void make_tj_data( long show_flag ) ;
	void print_tj_data( char print_name[] ) ;

	// make_next_data_by_zq
	long	m_next_nn ;
	long make_next_data_by_zq( double zq_len, double max_dx, long max_dyTot ) ;


	long get_DyTot_by_zq( long i0, long i1, long i2 ) ;
	void push_NextData_by_zq( long i, long next_ii ) ;
	long get_Idx_by_zq( long i, double zq_val, double max_dx ) ;
	long get_PreIdx_by_zq( long i0, double zq_len, double max_dx ) ;

	void show_pcm_data( short *pcm_data, long pcm_len, long show_color, long dot_color ) ;

	void set_BeginTime() ;
	void set_EndTime() ;

	// FreqZQ 
	long	m_FreqZQ_mm, m_FreqZQ_nn ;
	double	*m_FreqZQ ;

	LONGLONG	m_BeginTime, m_EndTime;
	void set_FreqZQ_Len( long nn );
	long push_FreqZQ( double zq_val ) ;

	long	m_Samples ;
	void	set_Freq_vars() ;

public:

	char	m_WaveFile[2048] ;
	double	m_ZQ, m_Freq ;
	double	m_TimeLen ;	

public:

	TWavePeriod2();
	virtual ~TWavePeriod2();

	void clear_period_data() ;
	double make_period_data(short *pcm_data, long pcm_len, long show_flag, long print_flag, long nCount ) ;

	void show_period_data() ;
	double	get_FreqZQ() ;

	void	set_Samples( long samples ) ;

	double make_last_FreqZQ(long show_flag, long print_flag ) ;

};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TWAVEPERIOD2_H__7F3FC9FE_B3B0_40D6_981E_B1C6B460AAC1__INCLUDED_)
