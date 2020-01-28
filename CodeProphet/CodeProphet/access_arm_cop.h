#pragma once

extern "C" u32 ReadCOPReg(u32 reg);
extern "C" void WriteCOPReg(u32 reg, u32 val);

#define REG_CPUID			0
#define REG_INTEL_PMU_CCNT	1
#define REG_INTEL_PMU_CTRL	2