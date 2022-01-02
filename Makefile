SRC = main.c
BIN = build/example.exe
# replace MSDXSDK with your own path to Microsoft DirectX SDK
INC = -Ideps/SDL2/include/ -IE:/MSDXSDK/Include
LLIB = -Ldeps/SDL2/lib/x64 -LE:/MSDXSDK/Lib/x64/
LIB = -lSDL2main -lSDL2 -lShell32 -Xlinker /subsystem:console

$(BIN) : $(SRC) 
	clang -std=c99 $(SRC) $(INC) $(LLIB) $(LIB) -o $(BIN)

clean:
	rm -f $(BIN)