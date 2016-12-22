// Node.js implementation
// Principles:
//	- Basic modules should not contain external dependencies (only internal)
//	- Avoid function usage in the CPU
var arg = require('minimist')(process.argv.slice(2));
if(!arg.program)
	throw new Error('No input program specified!');
var DEBUG = arg.d;

var vm = require('./lib/all.js')(console.log, DEBUG ? console.log : function(){});

var fs = require('fs');
var asm_file = fs.readFileSync(arg.program, 'utf8');
if(!asm_file)
	throw new Error('Input program is empty!');

var MEM = new vm.memory();
var CPU = new vm.cpu(MEM);
var ASM = new vm.assembler();

if(DEBUG){
	console.log('DEBUG MODE');
	console.log('Memory, addressable: '+(MEM.MAX_ADDR*4)+' bytes');
	console.log('Memory, segment size: '+MEM.SEGMENT_SIZE);
	console.log('Memory, segments: '+MEM.SEGMENTS);
}

ASM.compile(asm_file, MEM);
CPU.execute(DEBUG);

if(arg.memdump){
	console.log('A Memory dump is being created...');
	var header = new Uint32Array(2);
	header[0] = arg.dumpaddr;
	header[1] = arg.dumplength;
	fs.writeFileSync(arg.memdump, new Buffer(header.buffer));
	fs.writeFileSync(arg.memdump, new Buffer(MEM.readUint32Array(arg.dumpaddr, arg.dumplength).buffer), {flag: 'a'});
}