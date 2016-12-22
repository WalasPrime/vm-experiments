// Node.js implementation
// Principles:
//	- Basic modules should not contain external dependencies (only internal)
//	- Avoid function usage in the CPU

var vm = require('./lib/all.js')(console.log, console.log);

var arg = require('minimist')(process.argv.slice(2));
if(!arg.program)
	throw new Error('No input program specified!');

var fs = require('fs');
var asm_file = fs.readFileSync(arg.program, 'utf8');
if(!asm_file)
	throw new Error('Input program is empty!');

var MEM = new vm.memory();
var CPU = new vm.cpu(MEM);
var ASM = new vm.assembler();

console.log('Memory, addressable: '+(MEM.MAX_ADDR*4)+' bytes');
console.log('Memory, segment size: '+MEM.SEGMENT_SIZE);
console.log('Memory, segments: '+MEM.SEGMENTS);

ASM.compile(asm_file, MEM);
CPU.execute(true);