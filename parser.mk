PCOMP = g++
PCOMPFLAGS = -std=c++11 -g -MMD -I"../src/expressions" -I"../src/statements" -I"../src" -I"./"

y.tab.c y.tab.h: ../src/grammar.y
	bison -vyd ../src/grammar.y -o y.tab.c

y.tab.o: y.tab.c y.tab.h
	$(PCOMP) $(PCOMPFLAGS) -w -c y.tab.c

lex.yy.c: ../src/lexer.l
	flex ../src/lexer.l

lex.yy.o: lex.yy.c y.tab.h 
	$(PCOMP) $(PCOMPFLAGS) -c lex.yy.c
