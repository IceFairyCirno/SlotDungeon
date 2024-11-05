CPPFLAGS=-pedantic-errors -std=c++11 -g


GAME_OBJ_FILES := bin/main.o bin/menu.o bin/logger.o bin/game.o bin/blackMarket.o bin/terminalUtils.o bin/colorTextUtils.o bin/fileUtils.o bin/settings.o bin/ansi.o bin/gameData.o bin/playermove.o bin/battle.o bin/slotMachine.o

all: launcher game

# Targets to compile individual object files
launcher.o: launcher.cpp
	g++ $(CPPFLAGS) -c launcher.cpp -o launcher.o

bin/menu.o: bin/menu.cpp bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/menu.cpp -o bin/menu.o

bin/logger.o: bin/logger.cpp bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/logger.cpp -o bin/logger.o

bin/game.o: bin/game.cpp bin/game.h bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/game.cpp -o bin/game.o

bin/blackMarket.o: bin/blackMarket.cpp bin/blackMarket.h bin/commonUtils.h bin/gameData.h bin/ansi.h
	g++ $(CPPFLAGS) -c bin/blackMarket.cpp -o bin/blackMarket.o

bin/battle.o: bin/battle.cpp bin/battle.h bin/commonUtils.h bin/fileUtils.h  bin/gameData.h bin/ansi.h
	g++ $(CPPFLAGS) -c bin/battle.cpp -o bin/battle.o

bin/slotMachine.o: bin/slotMachine.cpp bin/slotMachine.h bin/commonUtils.h bin/fileUtils.h  bin/gameData.h bin/ansi.h
	g++ $(CPPFLAGS) -c bin/slotMachine.cpp -o bin/slotMachine.o

# Utils
bin/playermove.o: bin/playermove.cpp bin/game.h
	g++ $(CPPFLAGS) -c bin/playermove.cpp -o bin/playermove.o

bin/gameData.o: bin/gameData.cpp bin/gameData.h
	g++ $(CPPFLAGS) -c bin/gameData.cpp -o bin/gameData.o

bin/ansi.o: bin/ansi.cpp bin/ansi.h bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/ansi.cpp -o bin/ansi.o

bin/terminalUtils.o: bin/terminalUtils.cpp bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/terminalUtils.cpp -o bin/terminalUtils.o

bin/colorTextUtils.o: bin/colorTextUtils.cpp bin/commonUtils.h
	g++ $(CPPFLAGS) -c bin/colorTextUtils.cpp -o bin/colorTextUtils.o

bin/fileUtils.o: bin/fileUtils.cpp bin/fileUtils.h
	g++ $(CPPFLAGS) -c bin/fileUtils.cpp -o bin/fileUtils.o

bin/settings.o: bin/settings.cpp bin/settings.h
	g++ $(CPPFLAGS) -c bin/settings.cpp -o bin/settings.o

# main
bin/main.o: bin/main.cpp bin/menu.h bin/commonUtils.h bin/settings.h bin/fileUtils.h
	g++ $(CPPFLAGS) -c bin/main.cpp -o bin/main.o


# Executable targets
launcher: launcher.o
	g++ launcher.o -o launcher

game: $(GAME_OBJ_FILES)
	g++ $^ -o bin/game

# Clean command to remove object files and executables
clean:
	rm -f *.o bin/*.o launcher game
