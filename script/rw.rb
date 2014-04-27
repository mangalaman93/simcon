(1..2).each { |index|
	system("./script/simcon gen -i input -l 120 -v 5")
	system("./script/simcon run -i input -o output")
	system("./script/simcon copy")
}
