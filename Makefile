WEBIDLPATH=$(EMSCRIPTEN)/tools
WEBIDL=python $(WEBIDLPATH)/webidl_binder.py
SRC=src
_CFILES=dx7_voice.bc dx7_voice_data.bc dx7_voice_patches.bc dx7_voice_render.bc dx7_voice_tables.bc hexter_synth.bc
_GCCFILES=dx7_voice.o dx7_voice_data.o dx7_voice_patches.o dx7_voice_render.o dx7_voice_tables.o hexter_synth.o
CFILES=$(patsubst %,$(SRC)/%,$(_CFILES))
GCCFILES=$(patsubst %,$(SRC)/%,$(_GCCFILES))
CC=emcc
CXX=em++
CFLAGS=-O2 -g0
CXXFLAGS=$(CFLAGS) -std=c++11


%.bc: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) -c $^

ihexter:$(CFILES) $(SRC)/glue.cpp
	$(CXX) $(CXXFLAGS) $(CFILES) $(SRC)/i_hexter.cpp $(SRC)/hexter_js.cpp $(SRC)/gluewrapper.cpp -o hexter.js --post-js $(SRC)/glue.js

test: $(CFILES)
	$(CXX) $(CXXFLAGS) $^ $(SRC)/i_hexter.cpp $(SRC)/test.cpp -o run_test.js

cli: $(GCCFILES)
	mv *.o src
	gcc  $(SRC)/wave_io.c -c
	g++ $(CXXFLAGS) $^ wave_io.o $(SRC)/i_hexter.cpp $(SRC)/hexter-cli.cpp -o hexter-cli.js 

$(SRC)/glue.cpp : $(SRC)/ihexter.idl
	$(WEBIDL) $^ $(SRC)/glue

.PHONY: clean

clean:
	rm -f $(SRC)/*.o $(SRC)/*.bc $(SRC)/*.out $(SCR)/*.pkl $(SRC)/glue.cpp $(SRC)/glue.js
	rm -f *.js *.js.mem *.bc *.out *.pkl *.o
	rm -f hexter-cli
