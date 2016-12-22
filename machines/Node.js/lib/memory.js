module.exports = function(print, debug){
	function Memory(){
		this.MAX_ADDR = (-1 >>> 0);
		this.SEGMENT_SIZE = 4096*100;
		this.SEGMENTS = Math.ceil(this.MAX_ADDR/this.SEGMENT_SIZE);
		this.INVALID_SEGMENT_CODE = this.SEGMENTS+512;

		this.memory_segments = new Uint32Array(this.SEGMENTS);
		this.segments = [];
		for(var k in this.memory_segments)
			this.memory_segments[k] = this.INVALID_SEGMENT_CODE;
	}

	Memory.prototype.write = function(addr, value32){
		var seg = (addr / this.SEGMENT_SIZE) >> 0;
		if(this.memory_segments[seg] == this.INVALID_SEGMENT_CODE){
			this.memory_segments[seg] = this.segments.length;
			this.segments.push(new Uint32Array(this.SEGMENT_SIZE));
		}
		this.segments[this.memory_segments[seg]][addr - seg*this.SEGMENT_SIZE] = value32;
		console.log('WRITE '+value32+' AT '+addr+' (SEG '+seg+')');
	}

	Memory.prototype.read = function(addr){
		var seg = (addr / this.SEGMENT_SIZE) >> 0;
		if(this.memory_segments[seg] == this.INVALID_SEGMENT_CODE) return 0;
		return this.segments[this.memory_segments[seg]][addr - seg*this.SEGMENT_SIZE];
	}

	Memory.prototype.writeUint32Array = function(addr, array){
		for(var i = 0; i < array.length; i += 1)
			this.write(addr+i, array[i]);
	}

	Memory.prototype.readUint32Array = function(addr, length){
		var arr = new Uint32Array(length);
		for(var i = 0; i < length; i += 1)
			arr[i] = this.read(addr+i);
		return arr;
	}

	return Memory;
}