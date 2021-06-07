
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TAF_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TAF_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TAF_EXPORTS
#define TAF_API __declspec(dllexport)
#else
#define TAF_API __declspec(dllimport)
#endif

// This class is exported from the TAF.dll
class TAF_API CTAF {
public:
	CTAF(void);
	// TODO: add your methods here.
};

extern TAF_API int nTAF;

TAF_API int fnTAF(void);

