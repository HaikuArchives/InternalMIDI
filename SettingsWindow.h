#ifndef _SETTINGS_WINDOW_H
#define _SETTINGS_WINDOW_H

#include <Window.h>

class SettingsWindow : public BWindow {

public:
		SettingsWindow();
void	MessageReceived(BMessage *message);
bool	QuitRequested();
void	Show();

};

#endif