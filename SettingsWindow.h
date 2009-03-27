/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

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
