#pragma once



/*

This header includes function prototypes and definitions for programatically directing
the CodeProphet Instrumented profiler.

*/



/*

*/

#ifdef __cplusplus 
extern "C" { 
#endif 


/*

*/
DWORD StartProfiling(void);

/*

*/
DWORD StopProfiling(void);

/*

*/
DWORD IsProfilerRunning(void);

/*

*/
DWORD SuspendProfiling(void);

/*

*/
DWORD ResumeProfiling(void);

/*

*/
DWORD SaveCallGraphData(void);

/*

*/
DWORD SetThreadName(TCHAR* thread_name);

#ifdef __cplusplus
}
#endif