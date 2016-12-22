var OP_FORMATS = {
	NO_ARG: 1,
	VARIANT: 2,
	REG: 3,
	REG2: 4,
	REG_VARIANT: 5
}
module.exports = {
	OP: {
		MOV: 1,
		ADD: 2,
		ADC: 3,
		INC: 4,
		SUB: 5,
		SBC: 6,
		DEC: 7,
		CLF: 8,
		LOAD: 9,
		SAVE: 10,
		CMP: 11,
		JMP: 12,
		JE: 13,
		JNE: 14,
		JL: 15,
		JLE: 16,
		JG: 17,
		JGE: 18,
		PUSH: 19,
		POP: 20,
		CALL: 21,
		RET: 22,
		BREAK: 23,
		FAIL: 24
	},
	FORMATS: OP_FORMATS,
	FORMAT: {
		MOV: OP_FORMATS.REG_VARIANT,
		ADD: OP_FORMATS.REG_VARIANT,
		ADC: OP_FORMATS.REG_VARIANT,
		INC: OP_FORMATS.REG,
		SUB: OP_FORMATS.REG_VARIANT,
		SBC: OP_FORMATS.REG_VARIANT,
		DEC: OP_FORMATS.REG,
		CLF: OP_FORMATS.NO_ARG,
		LOAD: OP_FORMATS.REG_VARIANT,
		SAVE: OP_FORMATS.REG_VARIANT,
		CMP: OP_FORMATS.REG_VARIANT,
		JMP: OP_FORMATS.VARIANT,
		JE: OP_FORMATS.VARIANT,
		JNE: OP_FORMATS.VARIANT,
		JL: OP_FORMATS.VARIANT,
		JLE: OP_FORMATS.VARIANT,
		JG: OP_FORMATS.VARIANT,
		JGE: OP_FORMATS.VARIANT,
		PUSH: OP_FORMATS.REG,
		POP: OP_FORMATS.REG,
		CALL: OP_FORMATS.VARIANT,
		RET: OP_FORMATS.NO_ARG,
		BREAK: OP_FORMATS.NO_ARG,
		FAIL: OP_FORMATS.NO_ARG
	},
	REGS: {
		PC: 1,
		FLAGS: 2,
		ACC: 3,
		DS: 4,
		SS: 5,
		SP: 6,
		R0: 7,
		R1: 8,
		R2: 9,
		R3: 10,
		R4: 11,
		R5: 12,
		R6: 13,
		R7: 14,
		R8: 15,
		R9: 16,
		R10: 17,
		R11: 18
	},
	FLAGS: {
		C: 0,
		Z: 1,
		L: 2
	}
}