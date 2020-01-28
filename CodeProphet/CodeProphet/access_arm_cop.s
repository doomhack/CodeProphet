; Code to read and write the arm cop15 and cop15 registers.

	area |.text|, code, readonly

	export ReadCOPReg
	export WriteCOPReg
	
ReadCOPReg proc ;r0 = reg to read. 0 = cpuid, 2 = intel pmu ccnt.

	stmfd	sp!, {r1}
	
	cmp r0, #2
	bhi skip					;not a valid reg to read. Must be 0-2.

	mrs r1, cpsr				; check if we run in non-user mode.
	and r1, r1, #15
	cmp r1, #15
	bne skip

	cmp r0, #0 ;0 = REG_CPUID
	beq rdcpuid
	
	cmp r0, #1 ;1 = REG_INTEL_PMU_CCNT
	beq rdccnt

	cmp r0, #2 ;2 = REG_INTEL_PMU_CTRL
	beq rdpmuctrl
	
rdcpuid
	mrc p15, 0, r0, c0, c0, 0	; read cpuid cp15 into register 0
	b return
	
rdccnt
	mrc p14, 0, r0, c1, c0, 0
	b return
	
rdpmuctrl
	mrc p14, 0, r0, c0, c0, 0 
	b return
	
skip
	mov r0, #0
	
return
	ldmfd	sp!, {r1}  
	mov	pc, lr 
	
	

WriteCOPReg proc ;r0 = reg to write. r1 = value to write.

	stmfd	sp!, {r1,r2}

	cmp r0, #2
	bne wtreturn					;reg to write must be 2. intel pmu ctrl.

	mrs r2, cpsr				; check if we run in non-user mode.
	and r2, r2, #15
	cmp r2, #15
	bne wtreturn

wtpmuctrl
	mcr p14, 0, r1, c2, c0, 0

wtreturn
	ldmfd	sp!, {r1,r2}
	mov	pc, lr

end