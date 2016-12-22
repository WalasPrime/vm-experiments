var ARCH = require(__dirname+'/arch.js');

function parseError(msg, linenum, linestr){
	return new Error('Parsing error!\n'+msg+(linenum ? '\nOn line '+linenum : '')+(linestr ? '\n'+linestr : ''));
}

module.exports = function(print, debug){
	function AssemblerLoader(){
		this.op_lib = ARCH.OP;
	}

	AssemblerLoader.prototype.compile = function(code, memory){
		var raw_instructions = [];
		var labels = {};
		var lines = code.split("\n");
		debug('Code has '+lines.length+' lines');

		for(var k in lines)
			lines[k] = (lines[k].split('#')[0]).trim();
		
		for(var k in lines){
			var line = lines[k].toUpperCase();
			if(!line)continue;
			var words = line.split(' ');

			// Check if a label
			if(words[0] && words[0].substr(-1) == ':'){
				if(words.length > 1)
					throw parseError('No words expected after a label');
				var label = words[0].substr(0,words[0].length-1);
				debug('Label '+label+' defined for instruction '+raw_instructions.length);
				labels[label] = raw_instructions.length;
				continue;
			}
			
			// Check if a known instruction
			if(!this.op_lib[words[0]] && this.op_lib[words[0]] !== 0){
				throw parseError('Unknown instruction '+words[0], k, line);
			}
			raw_instructions.push({
				line: line,
				linenum: k,
				words: words,
				opcode: this.op_lib[words[0]],
				size: 2,
				location: -1
			});
		}

		debug('Code has '+raw_instructions.length+' instructions');
		var ptr = 0;
		for(var i = 0; i < raw_instructions.length; i+=1){
			raw_instructions[i].location = ptr;
			debug(raw_instructions[i].line+' @ '+ptr);
			ptr += raw_instructions[i].size;
		}

		for(var k in raw_instructions){
			var i = raw_instructions[k];
			var FMT = ARCH.FORMAT[i.words[0]];
			if(!FMT)
				throw parseError('No format defined for instruction '+i.words[0]);
			
			if(FMT == ARCH.FORMATS.NO_ARG && i.words.length > 1)
				throw parseError('Too many arguments, expected 0', i.linenum, i.line);
			
			if((FMT == ARCH.FORMATS.VARIANT || FMT == ARCH.FORMATS.REG) && i.words.length > 2)
				throw parseError('Too many arguments, expected 1', i.linenum, i.line);
			
			if((FMT == ARCH.FORMATS.VARIANT || FMT == ARCH.FORMATS.REG) && i.words.length < 2)
				throw parseError('Not enough arguments, expected 1', i.linenum, i.line);
			
			if((FMT == ARCH.FORMATS.REG2 || FMT == ARCH.FORMATS.REG_VARIANT) && i.words.length > 3)
				throw parseError('Too many arguments, expected 2', i.linenum, i.line);
			
			if((FMT == ARCH.FORMATS.REG2 || FMT == ARCH.FORMATS.REG_VARIANT) && i.words.length < 3)
				throw parseError('Not enough arguments, expected 2', i.linenum, i.line);

			var OP = new Uint32Array(2);
			OP[0] = i.opcode;
			if(FMT != ARCH.FORMATS.NO_ARG){
				var w1_is_reg = (ARCH.REGS[i.words[1]] ? true : false);
				var w2_is_reg = i.words.length > 2 ? (ARCH.REGS[i.words[2]] ? true : false) : false;

				if(!w1_is_reg && FMT != ARCH.FORMATS.VARIANT)
					throw parseError('First argument must be a register', i.linenum, i.line);
				
				if(!w2_is_reg && FMT == ARCH.FORMATS.REG2)
					throw parseError('Second argument must be a register', i.linenum, i.line);
				
				var w1_is_ref = (i.words[1].substr(0,1) == '@'); var w1_ref = 0;
				var w2_is_ref = (i.words.length > 2 && i.words[2].substr(0,1) == '@'); var w2_ref = 0;
				if(w1_is_ref) w1_ref = labels[i.words[1].substr(1)];
				if(w2_is_ref) w2_ref = labels[i.words[2].substr(1)];
				if(w1_is_ref && !w1_ref || w2_is_ref && !w2_ref)
					throw parseError('Unknown label '+i.words[1]+' referenced', i.linenum, i.line);
				if(w1_ref) i.words[1] = raw_instructions[w1_ref].location;
				if(w2_ref) i.words[2] = raw_instructions[w2_ref].location;

				// Uint32 hack: http://stackoverflow.com/questions/22335853/hack-to-convert-javascript-number-to-uint32
				var w1_uint = i.words[1] >>> 0;
				var w2_uint = (i.words.length > 2 ? i.words[2] >>> 0 : null);

				
				OP[0] = OP[0] | ((!w1_is_reg || !w2_is_reg ? 1 : 0) << 8) | ((w1_is_reg ? ARCH.REGS[i.words[1]] : 0) << 16) | ((w2_is_reg ? ARCH.REGS[i.words[2]] : 0) << 24);
				OP[1] = (!w1_is_reg ? w1_uint : w2_uint) || 0;
				debug('From instruction '+i.line+' code : '+OP[0]+' '+OP[1]);
			}
			memory.writeUint32Array(i.location, OP);
		}
	}

	return AssemblerLoader;
}