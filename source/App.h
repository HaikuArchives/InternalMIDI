/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _APP_H
#define _APP_H

#include <Application.h>
#include <Path.h>

#define APP_SIGNATURE "application/x-vnd.haiku-internal_midi"

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
