#include "Main.hpp"

Engine engine(80,49);

int main() {
	bool status = true;
    while ( status && !TCODConsole::isWindowClosed() ) {
    	status = engine.update();
    	engine.render();
		TCODConsole::flush();    
    }
    return 0;
}
