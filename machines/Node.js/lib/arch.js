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
		JMP: 3,
		BREAK: 4
	},
	FORMATS: OP_FORMATS,
	FORMAT: {
		MOV: OP_FORMATS.REG_VARIANT,
		ADD: OP_FORMATS.REG_VARIANT,
		JMP: OP_FORMATS.VARIANT,
		BREAK: OP_FORMATS.NO_ARG
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
	}
}