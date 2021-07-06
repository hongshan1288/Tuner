// TJDataProc.h: interface for the TJDataProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TJDATAPROC_H__8C28C176_E645_44DF_A77B_F75E29D54806__INCLUDED_)
#define AFX_TJDATAPROC_H__8C28C176_E645_44DF_A77B_F75E29D54806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "GetFreqType.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class TJDataProc  
{
public:

	TJDataProc();
	virtual ~TJDataProc();

	void	set_tj_name( long idx, char *aName ) ;
	void	clear_tj_data( long idx ) ;

	void	push_tj_data( long idx, double zq_dx, double zq_val ) ;

	void	print_tj_data( long idx, char print_name[] ) ;
	void	print_tj_tot( long idx ) ;

	void	sort_tj_data( long idx ) ;
	void	reverse_tj_data( long idx ) ;

	void	remove_some_tj_data( long idx ) ;
	void	remove_tj_data( long idx, long min_tot ) ;

	void	group_tj_data( long idx ) ;
	void	shrink_tj_data( long idx ) ;

	void	set_pcm_len( long idx, long pcm_si, long pcm_ei ) ;

private:

	double	m_zq_val, m_zq_dx ;
	TJ_Type		m_TJ[10] ;

	void	clear_tjData( long idx ) ;
	void	set_TJData_Len( long idx, long nn ) ;
	void	quickSort_TJData( TJData_Type *tj_data, long iLo, long iHi ) ;

	void	print_tjData( long idx, char print_name[] ) ;

};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_TJDATAPROC_H__8C28C176_E645_44DF_A77B_F75E29D54806__INCLUDED_)
