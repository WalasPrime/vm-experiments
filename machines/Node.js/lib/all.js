module.exports = function(print, debug){
	return {
		assembler: require(__dirname+'/assembler_loader.js')(print, debug),
		cpu: require(__dirname+'/cpu.js')(print, debug),
		memory: require(__dirname+'/memory.js')(print, debug)
	}
};