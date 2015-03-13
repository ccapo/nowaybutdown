#include "Main.hpp"

Engine engine(80,49);

int main() {
	bool status = true;
    while ( status && !TCODConsole::isWindowClosed() ) {
    	status = engine.update();
    	engine.render();
		TCODConsole::flush();
    }

	// Fade out
	if( !status ) {
		for(int i = 1; i <= 24; i++) {
			int fade = 255*(24 - i)/(24 - 1);
			TCODConsole::setFade(fade, TCODColor::black);
			TCODConsole::root->flush();
		}
	}

    return 0;
}
