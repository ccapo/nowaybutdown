#include <stdio.h>
#include <stdarg.h>
#include "Main.hpp"

static const int PANEL_HEIGHT=9;
static const int BAR_WIDTH=20;
static const int MSG_X=BAR_WIDTH+2;
static const int MSG_HEIGHT=PANEL_HEIGHT-1;

Gui::Gui() {
  con = new TCODConsole(engine.windowWidth,PANEL_HEIGHT);
}

Gui::~Gui() {
  delete con;
  log.clearAndDelete();
}

void Gui::render() {
  // clear the GUI console
  con->setDefaultBackground(TCODColor::black);
  con->clear();

  // draw the health bar
  renderBar(1,1,BAR_WIDTH,"HP",engine.player->entity->hp,
    engine.player->entity->hpMax,
    TCODColor::lightRed,TCODColor::darkerRed);

  // draw the message log
  int y=1;
  float colorCoef=0.4f;
  for (Message **it=log.begin(); it != log.end(); it++) {
    Message *message=*it;
    con->setDefaultForeground(message->col * colorCoef);
    con->print(MSG_X,y,message->text);
    y++;
    if ( colorCoef < 1.0f ) {
      colorCoef+=0.3f;
    }
  }

  // Player stats
  y = 3;
  con->setDefaultForeground(TCODColor::white);
  con->print(1,y++,"ATK: %d",engine.player->entity->atk);
  con->print(1,y++,"DEF: %d",engine.player->entity->def);
  //con->print(1,y++,"SPD: %d",engine.player->entity->spd);
  con->print(1,y++,"LVL: %d",engine.level);

  // blit the GUI console on the root console
  TCODConsole::blit(con,0,0,engine.windowWidth,PANEL_HEIGHT,
    TCODConsole::root,0,engine.windowHeight-PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, const char *name,
  float value, float maxValue, const TCODColor &barColor,
  const TCODColor &backColor) {
  // fill the background
  con->setDefaultBackground(backColor);
  con->rect(x,y,width,1,false,TCOD_BKGND_SET);

  int barWidth = (int)(value / maxValue * width);
  if ( barWidth > 0 ) {
    // draw the bar
    con->setDefaultBackground(barColor);
    con->rect(x,y,barWidth,1,false,TCOD_BKGND_SET);
  }
  // print text on top of the bar
  con->setDefaultForeground(TCODColor::white);
  con->printEx(x+width/2,y,TCOD_BKGND_NONE,TCOD_CENTER,
    "%s : %g/%g", name, value, maxValue);
}

Gui::Message::Message(const char *text, const TCODColor &col) :
  text(strdup(text)),col(col) { 
}

Gui::Message::~Message() {
  free(text);
}

void Gui::message(const TCODColor &col, const char *text, ...) {
  // build the text
  va_list ap;
  char buf[128];
  va_start(ap,text);
  vsprintf(buf,text,ap);
  va_end(ap);

  char *lineBegin=buf;
  char *lineEnd;
  do {
    // make room for the new message
    if ( log.size() == MSG_HEIGHT ) {
      Message *toRemove=log.get(0);
      log.remove(toRemove);
      delete toRemove;
    }

    // detect end of the line
    lineEnd=strchr(lineBegin,'\n');
    if ( lineEnd ) {
      *lineEnd='\0';
    }

    // add a new message to the log
    Message *msg=new Message(lineBegin, col);
    log.push(msg);

    // go to next line
    lineBegin=lineEnd+1;
  } while ( lineEnd );
}
