objects = expression.o

parse : $(objects)
	g++ -o parse $(objects)

expression.o : expression.cc node.h token.h parser.h
	g++ -c expression.cc

clean:
	rm -rf *.o parse
