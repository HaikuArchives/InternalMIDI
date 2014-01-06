#ifndef _SETTINGS_MAIN_VIEW_H
#define _SETTINGS_MAIN_VIEW_H

#include <Box.h>

class BBitmap;

class SettingsMainView : public BBox {

public:

		SettingsMainView( BRect frame );
		~SettingsMainView();
		
void	AttachedToWindow();

void	MouseDown( BPoint where );
void	Draw( BRect updateRect );
		
private:

BBitmap	*fIcon;

};

#endif