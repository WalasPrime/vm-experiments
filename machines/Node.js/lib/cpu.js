function dumbMaxPropertyVal(obj){
	var max = null;
	for(var k in obj)if(max == null || obj[k] > max)max = obj[k];
	return max;
}

var ARCH = require(__dirname+'/arch.js');

module.exports = function(print, debug){
	function cpu(memory){
		this.mem = memory;

		this.state = {
			registers: new Uint32Array(dumbMaxPropertyVal(ARCH.REGS)+1)
		}
	}

	cpu.prototype.execute = function(doDebug){
		debug('CPU execution starting');
		var F_CHECK_Z = false; var F_SET_C = false; var F_UNSET_C = false; var F_SET_L = false; var F_UNSET_L = false;
		var F_SET_Z = false; var F_UNSET_Z = false;
		var F_CHECK_REG = 0; var O2; var BIT32_H = 1 << 31;
		fetch: while(true){
			var OP = this.mem.readUint32Array(this.state.registers[ARCH.REGS.PC], 2);

			var OPCODE = OP[0] & 255;
			var OPTS = (OP[0] >> 8) & 255;
			var REG1 = (OP[0] >> 16) & 255;
			var REG2 = (OP[0] >> 24) & 255;
			var VAL = OP[1];

			if(doDebug){
				var OP_NAME;	
				for(var k in ARCH.OP){
					if(ARCH.OP[k] === OPCODE){
						OP_NAME = k;
						break;
					}
				}
				debug('Fetched OP '+OP_NAME+'('+(OPTS != 1 ? 'BY REG' : 'BY VAL')+')');
				var FMT = ARCH.FORMAT[OP_NAME];

				if(FMT != ARCH.FORMATS.NO_ARG){
					if(OPTS != 1){
						// REG1 must be a valid registry
						if(REG1 >= this.state.registers.length)
							throw new Error('Invalid register code for operation '+OP_NAME+' at '+this.state.registers[ARCH.REGS.PC]);
					}
					if(FMT == ARCH.FORMATS.REG2 || FMT == ARCH.FORMATS.REG_VARIANT && OPTS != 1){
						// REG2 must be a valid registry
						if(REG2 >= this.state.registers.length)
							throw new Error('Invalid register code for operand 2 operation '+OP_NAME+' at '+this.state.registers[ARCH.REGS.PC]);
					}
				}
			}

			exec: while(true){
				switch(OPCODE){
					case ARCH.OP.MOV:
						this.state.registers[REG1] = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						F_CHECK_REG = REG1;
						F_CHECK_Z = true; 
					break;

					case ARCH.OP.ADD:
						O2 = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						this.state.registers[ARCH.REGS.ACC] = this.state.registers[REG1] + O2;
						if(this.state.registers[REG1] & BIT32_H || O2 & BIT32_H){
							F_SET_C = true;
						}else{
							F_UNSET_C = true;
						}
					break;
					case ARCH.OP.ADC:
						if(OPTS != 1){
							OPTS = 1;
							VAL = this.state.registers[REG2];
						}
						if(this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.C)
							VAL += 1;
						OPCODE = ARCH.OP.ADD;
					continue exec;
					case ARCH.OP.INC:
						this.state.registers[REG1] += 1;
						F_CHECK_REG = REG1;
						F_CHECK_Z = true;
					break;

					// TODO: Add L flag modification
					case ARCH.OP.SUB:
						O2 = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						this.state.registers[ARCH.REGS.ACC] = this.state.registers[REG1] - O2;
						if(O2 > this.state.registers[REG1]){
							F_SET_C = true;
						}else{
							F_UNSET_C = true;
						}
					break;
					case ARCH.OP.SBC:
						if(OPTS == 1){
							OPTS = 1;
							VAL = this.state.registers[REG2];
						}
						if(this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.C)
							VAL -= 1;
						OPCODE = ARCH.OP.SUB;
					continue exec;
					case ARCH.OP.DEC:
						this.state.registers[REG1] -= 1;
						F_CHECK_REG = REG1;
						F_CHECK_Z = true;
					break;

					case ARCH.OP.CLF:
						this.state.registers[ARCH.REGS.FLAGS] = 0;
					break;

					case ARCH.OP.LOAD:
						O2 = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						this.state.registers[REG1] = this.mem.read(this.state.registers[ARCH.REGS.DS]+O2);
					break;

					case ARCH.OP.SAVE:
						O2 = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						this.mem.write(this.state.registers[ARCH.REGS.DS]+O2, this.state.registers[REG1]);
					break;

					case ARCH.OP.CMP:
						O2 = (OPTS != 1 ? this.state.registers[REG2] : VAL);
						if(this.state.registers[REG1] < O2){
							F_SET_L = true;
							debug('\tSET_LESS');
						}else{
							F_UNSET_L = true;
						}
						if(this.state.registers[REG1] == O2){
							F_SET_Z = true;
							debug('\tSET_ZERO');
						}else{
							F_UNSET_Z = true;
						}
					break;

					case ARCH.OP.JMP:
						this.state.registers[ARCH.REGS.PC] = (OPTS != 1 ? this.state.registers[REG1] : VAL);
						debug('\tJumped to '+this.state.registers[ARCH.REGS.PC]);
					continue fetch;

					case ARCH.OP.JE:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.Z) != 0)
							continue exec;
					break;
					case ARCH.OP.JNE:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.Z) == 0)
							continue exec;
					break;
					case ARCH.OP.JL:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.L) != 0)
							continue exec;
					break;
					case ARCH.OP.JLE:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & (ARCH.FLAGS.L | ARCH.FLAGS.Z)) != 0)
							continue exec;
					break;
					case ARCH.OP.JG:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & (ARCH.FLAGS.L | ARCH.FLAGS.Z)) == 0)
							continue exec;
					break;
					case ARCH.OP.JGE:
						OPCODE = ARCH.OP.JMP;
						if((this.state.registers[ARCH.REGS.FLAGS] & ARCH.FLAGS.L) == 0)
							continue exec;
					break;

					case ARCH.OP.PUSH:
					case ARCH.OP.CALL:
						if(OPCODE == ARCH.OP.CALL)
							REG1 = ARCH.REGS.PC;
						this.mem.write(this.state.registers[ARCH.REGS.SS] + this.state.registers[ARCH.REGS.SP], this.state.registers[REG1]);
						this.state.registers[ARCH.REGS.SP] += 1;
						if(OPCODE == ARCH.OP.CALL){
							OPCODE = ARCH.OP.JMP;
							continue exec;
						}
					break;
					case ARCH.OP.POP:
					case ARCH.OP.RET:
						if(this.state.registers[ARCH.REGS.SP] == 0)
							throw new Error('Tried to pop an empty stack');

						if(OPCODE == ARCH.OP.RET)
							REG1 = ARCH.REGS.PC;
						this.state.registers[ARCH.REGS.SP] -= 1;
						this.state.registers[REG1] = this.mem.read(this.state.registers[ARCH.REGS.SS] + this.state.registers[ARCH.REGS.SP]);
					break;

					case ARCH.OP.BREAK:
						debug('BREAK encountered at '+this.state.registers[ARCH.REGS.PC]);
					break fetch;

					case ARCH.OP.FAIL:
						throw new Error('FAIL encountered at '+this.state.registers[ARCH.REGS.PC]);

					default:
						debug('Unknown OP '+OPCODE);
						break fetch;
				}
				break exec;
			}

			if(F_CHECK_Z){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] & ~ARCH.FLAGS.Z | (this.state.registers[F_CHECK_REG] === 0 ? ARCH.FLAGS.Z : 0);
				F_CHECK_Z = false;
			}
			if(F_SET_Z){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] | ARCH.FLAGS.Z;
				F_SET_Z = false;
			}
			if(F_UNSET_Z){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] & ~ARCH.FLAGS.Z;
				F_UNSET_Z = false;
			}
			if(F_SET_C){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] | ARCH.FLAGS.C;
				F_SET_C = false;
			}
			if(F_UNSET_C){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] & ~ARCH.FLAGS.C;
				F_UNSET_C = false;
			}
			if(F_SET_L){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] | ARCH.FLAGS.L;
				F_SET_L = false;
			}
			if(F_UNSET_L){
				this.state.registers[ARCH.REGS.FLAGS] = this.state.registers[ARCH.REGS.FLAGS] & ~ARCH.FLAGS.L;
				F_UNSET_L = false;
			}

			this.state.registers[ARCH.REGS.PC] += 2;
		}
	}

	return cpu;
}