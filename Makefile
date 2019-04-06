all: bwtencode bwtsearch
	
bwtencode: bwtencode.c
	gcc bwtencode.c -o bwtencode
	
bwtsearch: bwtsearch.c
	gcc bwtsearch.c -o bwtsearch