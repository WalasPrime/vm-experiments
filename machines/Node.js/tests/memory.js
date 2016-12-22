var vm = require(__dirname+'/../lib/all.js')(function(){}, function(){});
var test = require('blue-tape');

var MEM;
test('Memory controller constructable', function(t){
	MEM = new vm.memory();
	t.end();
});

test('Segment 0 is not initialized', function(t){
	t.equal(MEM.memory_segments[0], MEM.INVALID_SEGMENT_CODE)
	t.end();
});

test('Reading non existing data returns 0', function(t){
	t.equal(MEM.read(0), 0, 'Reading 0');
	t.end();
});

test('Writing creates a segment', function(t){
	MEM.write(0, 1024);
	t.notEqual(MEM.memory_segments[0], MEM.INVALID_SEGMENT_CODE, 'Segment initialized');
	t.end();
});

test('Can read after writing', function(t){
	t.equal(MEM.read(0), 1024, 'Read proper value');
	t.end();
});

test('Can read after writing an array', function(t){
	var numbers = [123, 123, 563, 9874, 45864123, 8652123];
	MEM.writeUint32Array(10,numbers);
	var loaded = MEM.readUint32Array(10,numbers.length);
	for(var k in loaded){
		t.equals(loaded[k], numbers[k]);
	}
	t.end();
});