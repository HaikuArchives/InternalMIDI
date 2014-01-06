#ifndef _APP_H
#define _APP_H

#include <Application.h>
#include <Path.h>

#define APP_SIGNATURE "application/x-vnd.pecora-internal_midi"

class InternalMidi;
class SettingsWindow;

class App : public BApplication {

public:
			App();
			~App();

void		ReadyToRun();
void		MessageReceived(BMessage *msg);
bool		QuitRequested();
void		AboutRequested();

private:

InternalMidi		*fInternalMidi;
SettingsWindow		*fSettingsWindow;
			
};

#endif