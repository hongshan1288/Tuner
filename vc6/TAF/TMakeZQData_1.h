// TMakeZQData_1.h: interface for the TMakeZQData_1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMAKEZQDATA_1_H__F5D495C1_E6D6_4512_AD07_48FD0578FDD6__INCLUDED_)
#define AFX_TMAKEZQDATA_1_H__F5D495C1_E6D6_4512_AD07_48FD0578FDD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "GetFreqType.h"
#include "TJDataProc.h"
//////////////////////////////////////////////////////////////////////
class TMakeZQData_1  
{
public:

	TMakeZQData_1();
	virtual ~TMakeZQData_1();

	void	show_seg_data( long di, long y_off, long show_color, long show_yy ) ;
	void	make_seg_data() ;

	void	make_next_data( long n, double max_dx, long max_dyTot ) ;
	void	make_next_data2( long n, double max_dx, long max_dyTot ) ;
	long	make_next_data3( double zq_len, double max_dx, long max_dyTot ) ;

	void	make_tj_data( TJDataProc *aTJProc, long idx, char *tjName ) ;



private:

	double	m_MaxDiff_Dx ;
	long	m_MaxDiff_Dy ;

	SegYY_Type	*m_SegYY ;
	long		m_SegYY_m, m_SegYY_n ;
	void	set_SegYY_Len( long nn ) ;

	long push_SegYY( long xx, short yy ) ;
	void make_SegDY( long i ) ;
	void make_SegYY() ;
	void show_SegYY( long i, long show_color ) ;

	// seg data
	SegData_Type	*m_SegData ;
	long		m_SegData_m, m_SegData_n ;

	void	set_SegData_Len( long nn ) ;
	long	push_SegData( short ff, long ix, double xx ) ;

	long get_sign( long yy ) ;
	double get_xx( long i, long y0, long yy ) ;
	short get_yy( double xx ) ;

	void make_SegData( short *pcm_data, long pcm_len ) ;
	void remove_TooShortDxSegData( double min_dx ) ;

	long get_DyTotFromSegData( long i1, long i2, long comp_nn ) ;
	bool like_NextSegData( long i1, long i2, long comp_nn, double max_dx ) ;
	void push_NextSegData( long i, long next_ii ) ;
	long set_NextSegData( long i, long comp_nn, double max_dx, long max_dyTot ) ;

	long get_DyTotFromSegData2( long i1, long i2, long comp_nn ) ;
	bool like_NextSegData2( long i1, long i2, long comp_nn, double max_dx ) ;
	void push_NextSegData2( long i, long next_ii ) ;
	long set_NextSegData2( long i, long comp_nn, double max_dx, long max_dyTot ) ;

	void make_TJData( TJDataProc *aTJProc, long idx, char *tjName ) ;


	long	m_next_si, m_next_ei ;

	long	m_next_ff ;
	void push_next_ff( long si, long next_ii ) ;
	void make_next_ff() ;
	void set_all_next_ff() ;
	void set_all_next_ff_ex( long next_ff, long val ) ;

	void clear_next_nn( long next_ff ) ;
	long make_TJData2Ex( TJDataProc *aTJProc, long idx, long nn ) ;
	void make_TJData2( TJDataProc *aTJProc, long idx, char *tjName ) ;


	long	m_next_nn ;
	void clear_next_data() ;
	void push_NextSegData3( long i, long next_ii ) ;
	long get_DyTotFromSegData3( long i0, long i1, long i2 ) ;
	long get_Index_NextSegData3( long i, double zq_val, double max_dx ) ;
	long set_NextSegData3( long i0, long i, double zq_val, long max_dyTot ) ;

};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TMAKEZQDATA_1_H__F5D495C1_E6D6_4512_AD07_48FD0578FDD6__INCLUDED_)
