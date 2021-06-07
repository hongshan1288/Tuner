// AudioProc.cpp: implementation of the AudioProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioProc.h"
//#include "AllFuncs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
#define FreeBuf( a ) { \
	if ( a==INVALID_HANDLE_VALUE || a==NULL ) \
		; \
	else \
	{ \
		GlobalFree( a ) ; \
		a=NULL ; \
	} \
}
//////////////////////////////////////////////////////////////////////
AudioProc::AudioProc()
{
	mWaveInHandle = NULL ;
	mWavBuf = NULL ;
	mWaveHeader = NULL ;
	mWaveThread = NULL ;
	mBufs = 0 ;
	mAudioFlag = 0 ;
	m_WaveIn_Data_Proc = NULL ;
}
//////////////////////////////////////////////////////////////////////
AudioProc::~AudioProc()
{
	Audio_Close() ;
}
//////////////////////////////////////////////////////////////////////
long	AudioProc::Audio_Open( long OpenFlag, long Samples, long Bits, long Channels, long Bufs, long BufSize )
{
	long		i, size ;
	DWORD		err, dwID ;
	UINT		uDeviceID ; 

	Audio_Close() ;

	if ( OpenFlag==0 ) // Record
	{
		// Create a thread to deal with the waveform data to write the file
		mWaveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(_WaveIn_Thread), (LPVOID)this, 0, &dwID);
		if ( !mWaveThread )
		{
			sprintf( mErrMsg, "Can't create WAVE recording thread! -- %08X\n", GetLastError());
			return ( -2 ) ;
		}

		ZeroMemory(&mWaveFormat, sizeof(WAVEFORMATEX) ) ;

		mWaveFormat.wFormatTag = WAVE_FORMAT_PCM ;
		mWaveFormat.nSamplesPerSec = Samples ;		// 44100
		mWaveFormat.wBitsPerSample = (UINT)Bits ;	// 16
		mWaveFormat.nChannels = (UINT)Channels ;	// 2
		mWaveFormat.nBlockAlign = mWaveFormat.nChannels * (mWaveFormat.wBitsPerSample/8) ;
		mWaveFormat.nAvgBytesPerSec = BufSize; //mWaveFormat.nSamplesPerSec * mWaveFormat.nBlockAlign ;
		mWaveFormat.cbSize = 0 ;

		uDeviceID = WAVE_MAPPER ;
		// open the wave in device
		err = waveInOpen(&mWaveInHandle, uDeviceID, &mWaveFormat, (DWORD)dwID, 0, CALLBACK_THREAD);
		if (err)
		{
			sprintf( mErrMsg, "Can't open WAVE In Device!" ) ;
			return ( -3 ) ;
		}

		mBufs = Bufs ;
		size = mBufs ;
		size *= sizeof( WAVEHDR ) ;
		mWaveHeader = (WAVEHDR*)GlobalAlloc( GPTR, size ) ;

		mBufSize = BufSize ;

		size = mBufs ;
		size *= sizeof( LPSTR ) ;
		mWavBuf = (LPSTR*)GlobalAlloc( GPTR, size ) ;

		err = 0 ;
		size = mBufSize ;
		for ( i=0; i<mBufs; i++ )
		{
			mWavBuf[i] = (LPSTR)GlobalAlloc( GPTR, size ) ;
			mWaveHeader[i].dwBufferLength = mBufSize ;
			mWaveHeader[i].lpData = mWavBuf[i] ;

			// Prepare WAVEHDR
			if ((err = waveInPrepareHeader(mWaveInHandle, &mWaveHeader[i], sizeof(WAVEHDR))))
			{
				sprintf( mErrMsg, "Error preparing WAVEHDR 1! -- %08X\n", err);
				break ;
//				return ( -5 ) ;
			}

			// Queue WAVEHDR 
			if ((err = waveInAddBuffer(mWaveInHandle, &mWaveHeader[i], sizeof(WAVEHDR))))
			{
				sprintf( mErrMsg, "Error queueing WAVEHDR 1! -- %08X\n", err);
				break; 
//				return ( -6 ) ;
			}
		}

		if ( err !=0 )
		{
			for ( i=0; i<mBufs; i++ )
			{
				if ( mWavBuf[i] != NULL )
					FreeBuf( mWavBuf[i] ) ;
			}
			FreeBuf( mWavBuf ) ;
			FreeBuf( mWaveHeader ) ;
			return ( -111 ) ;
		}
	}
	else if ( OpenFlag==1 ) // Play
	{

	}
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void	AudioProc::Audio_Close()
{
	long	i ;
	DWORD	dwFlag ;

	if ( mAudioFlag > 0 )
	{
		mExitEvent = CreateEvent( NULL, FALSE, FALSE, NULL ) ; 
		ResetEvent(mExitEvent) ;

		mAudioFlag = 1000 ;
		waveInStop( mWaveInHandle ) ;
		for ( i=0; i<1000; i++)
		{
			dwFlag = WaitForSingleObject( mExitEvent, 10 ) ; 
			if ( mAudioFlag>=2000 ) 
				break ;
		}
		waveInReset(mWaveInHandle);

		if ( mBufs>0 )
		{
			for ( i=0; i<mBufs; i++ )
			{
				waveInUnprepareHeader(mWaveInHandle, &mWaveHeader[i], sizeof(WAVEHDR)) ;
				FreeBuf( mWavBuf[i] ) ;
			}
			FreeBuf( mWaveHeader ) ;
			mBufs = 0 ;
		}
		waveInClose(mWaveInHandle) ; 

		mWaveInHandle = NULL ;
		mAudioFlag = 0 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
UINT AudioProc::_WaveIn_Thread( LPVOID lpparam)
{
	UINT		uFlag ;
	AudioProc	*tThis ;
	tThis = (AudioProc*)lpparam ;

	uFlag = tThis->WaveIn_Thread() ;
	if ( uFlag>0 )
	{
		SetEvent( tThis->mExitEvent ) ;
	}
	return ( uFlag ) ;
}
//////////////////////////////////////////////////////////////////////
UINT AudioProc::WaveIn_Thread()
{
	MSG			msg ;
	int			iFlag ;
	WAVEHDR		*aWaveHeader ;

	while (GetMessage(&msg, 0, 0, 0) == 1)
	{
		iFlag = 0 ;
		switch (msg.message)
		{
			case MM_WIM_DATA:		//A buffer has been filled by the driver
			{
				aWaveHeader = ((WAVEHDR *)msg.lParam) ;
				if ( aWaveHeader != NULL )
				{
					if (aWaveHeader->dwBytesRecorded>0)
					{
						WaveIn_Data_Proc( aWaveHeader ) ;
						waveInAddBuffer(mWaveInHandle, aWaveHeader, sizeof(WAVEHDR));
					}
				}
				break ;
			}
			case MM_WIM_OPEN:		//main thread is opening the WAVE device
			{
                continue ;
			}

			case MM_WIM_CLOSE:		//main thread is closing the WAVE device
			{
				mAudioFlag = 2000 ;
				return ( 1000 ) ;
			}
		}
		if ( mAudioFlag>=1000 )
		{
			mAudioFlag = 2000 ;
			return ( 100 ) ;
		}
	}
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
long AudioProc::WaveIn_Data_Proc( WAVEHDR *aWaveHeader )
{
	if ( m_WaveIn_Data_Proc != NULL )
		return ( m_WaveIn_Data_Proc( aWaveHeader->lpData, aWaveHeader->dwBytesRecorded, m_Caller ) ) ;
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
long	AudioProc::Audio_Start()
{
	DWORD	err ;

	if ((err = waveInStart(mWaveInHandle)))
	{
		sprintf( mErrMsg, "Error starting record! -- %08X\n", err);
		return ( -9 ) ;
	}
	mAudioFlag = 10 ;
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void	AudioProc::Audio_Stop()
{
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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
