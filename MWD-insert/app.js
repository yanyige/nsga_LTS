var fs = require('fs');
var readline = require('readline');
var os = require('os');
var xlsx = require('node-xlsx');

var rand = 1;
var gen = 10;
var pop = 100;
var originName = 'MWD';

// var fileName = 'TMNR-1-10-100.txt';

var fReadName = 'MWD-1-10-100-insert.txt';

var data = [];
var arr = [];
var title = [];
var str = "";
// var fWriteName = '.'

var fRead = fs.createReadStream(fReadName);

var objReadLine = readline.createInterface({
	input: fRead,
	terminal: true,
});

var index = 0;

title.push('instance name','algorithm','population size','generations','num of tasks','num of machines','cycles','seed','num of non-dominated solutions','pareto front');



var coor = [];

for(var rand = 1 ; rand < 5 ; rand ++) {
	for(gen = 10 ; gen < 101 ; gen += 10) {
		for(pop = 100 ; pop < 1001 ; pop += 100) {
			if(pop == 100  || pop == 300 || pop == 500 || pop == 700 || pop == 900 || pop == 1000) {


				fileName = originName + '-' + rand + '-' + gen + '-' + pop+ '-insert';

				(function(fileName, gen, pop, rand) {
					console.log(fileName);
					fReadName = fileName+'.txt';
					var fRead = fs.createReadStream(fReadName);
					// console.log(fRead);
					var objReadLine = readline.createInterface({
						input: fRead,
						terminal: true,
					});

					coor = [];

					objReadLine.on('line', (line) => {

						if(line[0] == '[') {
							str += line;
						}
						if(line[0] == 't') {
							var tot = line.split("=")[1];
							coor.push(str);
							arr[index] = [];
							arr[index].push(fileName);
							arr[index].push('MOHA');
							arr[index].push(pop);
							arr[index].push(gen);
							arr[index].push(14);
							arr[index].push(3);
							arr[index].push(3);
							arr[index].push(rand);
							arr[index].push(tot);
							arr[index].push(coor);
							coor = [];
							str = "";
							index ++;
							console.log('done' + index);
						}
						// console.log(coor);
						// console.log(index);
						// data.push(coor);
					});

				})(fileName, gen, pop, rand);

				// var foo = function(fileName) {
				// 	return function(fileName) {

				// 		fReadName = fileName+'.txt';
				// 		console.log(fReadName);
				// 		fRead = fs.createReadStream(fReadName);
				// 		// console.log(fRead);
				// 		var objReadLine = readline.createInterface({
				// 			input: fRead,
				// 			terminal: true,
				// 		});

				// 		coor = [];
				// 		objReadLine.on('line', (line) => {
				// 			if(line[0] == '[') {
				// 				coor += line;
				// 			}
				// 			index ++;
				// 			// console.log(coor);
				// 			// console.log(index);
				// 		});
				// 		console.log(coor);
				// 		objReadLine.on('close', () => {

				// 			console.log("asdsad");
				// 		});
				// 	}
				// }

				// foo(fileName)(fileName);

			}
		}
	}
}



objReadLine.on('close', () => {
	// console.log(arr);
	data.push(title);
	for(var i = 0 ; i < index ; i ++) {
		data.push(arr[i]);
	}
	var buffer = xlsx.build([
		{
			name: 'sheet1',
			data: data,
		}
	]);
	fs.writeFileSync('test1.xlsx', buffer, {'flag': 'w'});
});



