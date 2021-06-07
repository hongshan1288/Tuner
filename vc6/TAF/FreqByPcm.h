// FreqByPcm.h: interface for the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
#define AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct	PcmModeData_Type
{
	long	idx_s, idx_e, idx_f ;
} ;

class TFreqByPcm  
{
private:

	short				*m_pcm_data ;
	long				m_pcm_len ;

	PcmModeData_Type	*m_PcmModeData ;
	long				m_PcmModeData_m, m_PcmModeData_n ;

	void				SetPcmModeDataLen( long nn ) ;
	void				PushPcmModeData( long idx, long idx_e, long idx_f ) ;

	void				show_PcmData( short *pcm_data, long pcm_len, long show_color ) ;

	
	long	m_FlatVV ;
	double	m_MinVV ;
	bool check_pcm_data_is_a_noise( short *pcm_data, long pcm_len ) ;

	void flat_pcm_data( short *pcm_data, long pcm_len ) ;
	void make_pmd_data( short *pcm_data, long pcm_len ) ;


public:

	double	mFreq ;

	TFreqByPcm();
	virtual ~TFreqByPcm();

	long GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc ) ;

};

#endif // !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
