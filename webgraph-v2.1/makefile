CPPFLAGS = -Wall -O3
COMPRESS = bitvector.o creategraph.o utils.o Compress.o graph.o HuffmanTree.o main-comp.o edmonds.o test.o
DECOMPRESS = bitvector.o utils.o Decompress.o graph.o DecodeTree.o main-decomp.o creategraph.o
CONVERT = utils.o creategraph.o convert.o graph.o bitvector.o
CREATE = utils.o creategraph.o main-create.o graph.o bitvector.o
all: compress decompress convert create
compress:$(COMPRESS)
	g++ $(CPPFALGS) -o comp-webgraph $(COMPRESS)
decompress:$(DECOMPRESS)
	g++ $(CPPFLAGS) -o decomp-webgraph $(DECOMPRESS)
convert:$(CONVERT)
	g++ $(CPPFLAGS) -o convert $(CONVERT)
create:$(CREATE)
	g++ $(CPPFLAGS) -o create $(CREATE)
clean:
	rm -rf *.o

