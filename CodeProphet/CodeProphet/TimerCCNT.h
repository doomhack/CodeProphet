#pragma once

/* performance monitoring  
 
REGISTERS (Co-processor 14) 
--------------------------- 
PMNC (CP14 : Reg 0) - Performance Monitor Control  
CCNT (CP14 : Reg 1) - Clock Counter 
PMN0 (CP14 : Reg 2) - Performance Counter 0 
PMN1 (CP14 : Reg 3) - Performance Counter 1 

*/
								 
#define PMNC_BIT_RESERVED_ALL 0xF0000880 
 
#define PMNC_BIT_RESERVED_MASK (~PMNC_BIT_RESERVED_ALL) 
 
// clear bits that require write-as-zero as per spec 
#define PMNC_BIT_RESERVED_CLEAR(dw) (dw &= PMNC_BIT_RESERVED_MASK) 

// 
// *** PMNC Register *** 
// 
 
// Enable (E) 
#define PMNC_BIT_E          (0x1) 
#define PMNC_BIT_E_SET(dw) (dw |= PMNC_BIT_E) 
#define PMNC_BIT_E_DIS(dw) (dw &= ~PMNC_BIT_E) 
 
// Clock Counter Reset (C) 
#define PMNC_BIT_C          (0x1 << 2) 
#define PMNC_BIT_C_SET(dw) (dw |= PMNC_BIT_C) 
#define PMNC_BIT_C_DIS(dw) (dw &= ~PMNC_BIT_C) 
 
// Clock Counter Divider (D) 
#define PMNC_BIT_D          (0x1 << 3) 
#define PMNC_BIT_D_SET(dw) (dw |= PMNC_BIT_D) 
#define PMNC_BIT_D_DIS(dw) (dw &= ~PMNC_BIT_D) 
 
// clock overflow flag 
#define PMNC_BIT_CCNT_OVERFLOW (0x1 << 10) 
#define PMNC_BIT_CCNT_OVERFLOW_SET(dw) (dw |= PMNC_BIT_CCNT_OVERFLOW) 
#define PMNC_BIT_CCNT_OVERFLOW_DIS(dw) (dw &= ~PMNC_BIT_CCNT_OVERFLOW) 
 
// Nomenclature for PMU CP register access (matches spec) 
#define PMUREG_PMNC 0        // PMU Control Register 
#define PMUREG_CCNT 1        // PMU Clock Counter (CCNT) 


class TimerCCNT : public ITimer
{
	public:
	TimerCCNT(void);
	~TimerCCNT(void);

	virtual u32 Init(void);
	virtual u32 Shutdown(void);
	virtual u64 GetTime(void);

	private:
	volatile u32 wrap_count;

	HANDLE semaphore;
	BOOL oldkmode;
};
