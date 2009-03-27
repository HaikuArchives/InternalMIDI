/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Button.h>
#include <CheckBox.h>
#include <Directory.h>
#include <Entry.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Roster.h>
#include <Synth.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "Prefs.h"
#include "RealtimeSlider.h"
#include "ScopeView.h"
#include "SettingsMainView.h"
#include "SignalView.h"

SettingsMainView::SettingsMainView( BRect bounds )
:	BBox( bounds, NULL, B_FOLLOW_ALL ),
	fIcon( 0 )
{
}

void SettingsMainView::AttachedToWindow() {

	// Fontsensitivität...	
	BFont	font(be_plain_font);
	float divider = font.StringWidth("Synthesizer patches") + 8;

	// Icon
	entry_ref ref;
	be_roster->FindApp( APP_SIGNATURE, &ref);

	BFile			file(&ref, B_READ_WRITE);
	BAppFileInfo	appFileInfo(&file);
	
	fIcon = new BBitmap(BRect(0,0,31,31), B_CMAP8 );
	appFileInfo.GetIcon(fIcon, B_LARGE_ICON);

	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
	SetLowColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
	SetDrawingMode( B_OP_OVER );

	// Pause
	BCheckBox	*checkbox = new BCheckBox( BRect( 0, 0, font.StringWidth("Pause") + 24, 12 ).OffsetByCopy( 50, 24 ),
		"pause", "Pause", new BMessage('Paus') );
	if (prefs.fPause) checkbox->SetValue( 1 );
	AddChild( checkbox );
								
	BRect bounds;

	// Signal
	bounds = BRect( 0, 0, 9, 39 );
	bounds.OffsetTo( Bounds().right - bounds.Width() - 63, Bounds().top + 10 );

	SignalView	*signalView = new SignalView( bounds, B_FOLLOW_RIGHT );
	AddChild( signalView );
	
	// Scope
	bounds = BRect( 0, 0, 39, 39 );
	bounds.OffsetTo( Bounds().right - bounds.Width() - 10, Bounds().top + 10 );

	ScopeView	*scopeView = new ScopeView( bounds, B_FOLLOW_RIGHT );
	AddChild( scopeView );

	BBox			*box;
	BPopUpMenu		*menu;
	BMenuItem		*menuItem;
	BMenuField		*menuField;
	BRect			frame;
	RealtimeSlider	*slider;

	// ----------- 1. Box ------------------------------
	bounds = BRect( Bounds().left + 10, 54, Bounds().right - 8, 146 );
	box = new BBox( bounds, NULL, B_FOLLOW_ALL);
	box->SetLabel( "Quality" );
	AddChild( box );
	
	// patches
	frame = BRect( 10, 13, bounds.right - 12, 20 );
	menu = new BPopUpMenu("please select");

	BDirectory	directory( prefs.fSynthDirectory.Path() );
	
	while ( directory.GetNextRef( &ref ) == B_OK ) {
		BMessage	*message = new BMessage('Patc');
		message->AddRef("patches", &ref);
		menuItem = new BMenuItem( ref.name, message );
		if (strcmp(ref.name, prefs.fSynthFile.String())==0) menuItem->SetMarked( true );
		menu->AddItem( menuItem );
	}

	menuField = new BMenuField( frame, "patches", "Synthesizer patches", menu, B_FOLLOW_ALL);
	menuField->SetDivider( divider );
	box->AddChild( menuField );

	// sampling rate
	int	rate_array[] = { 11025, 22050, 44100 };
	
	frame.OffsetBy(0, 25 );
	menu = new BPopUpMenu("please select");
	
	for (int i=0; i < sizeof(rate_array) / sizeof(int32); ++i ) {
		BMessage	*message = new BMessage('Rate');
		message->AddInt32("rate", rate_array[i]);
		BString menu_title;
		menu_title << rate_array[i];
		menuItem = new BMenuItem( menu_title.String(), message );
		if ( rate_array[i] == prefs.fSamplingRate) menuItem->SetMarked( true );
		menu->AddItem( menuItem );
	}

	menuField = new BMenuField( frame, "rates", "Sampling rate", menu, B_FOLLOW_ALL );
	menuField->SetDivider( divider );
	box->AddChild( menuField );

	// interpolation
	const char *interpolations[] = { "Drop sample", "2 point interpolation", "Linear interpolation" };
	
	frame.OffsetBy(0, 25 );
	menu = new BPopUpMenu("please select");
	
	for (int i=0; i < sizeof(interpolations) / sizeof(int32); ++i ) {
		BMessage	*message = new BMessage('Inte');
		message->AddInt32("interpolation", i);
		BString menu_title;
		menu_title << interpolations[i];
		menuItem = new BMenuItem( menu_title.String(), message );
		if ( i == prefs.fInterpolation)	menuItem->SetMarked( true );
		menu->AddItem( menuItem );
	}

	menuField = new BMenuField( frame, "interpolation", "Interpolation", menu, B_FOLLOW_ALL );
	menuField->SetDivider( divider );
	box->AddChild( menuField );
	
	// ----------- 2. Box ------------------------------
	bounds.top = bounds.bottom + 10; bounds.bottom = bounds.top + 74;
	box = new BBox( bounds, NULL, B_FOLLOW_ALL);
	box->SetLabel( "Volume" );
	AddChild( box );

	rgb_color dark_blue = { 0, 51, 102 };

	frame = BRect( 10, 18, bounds.right - 21, 22 );

	slider = new RealtimeSlider(frame, "gain", "Gain", new BMessage('Gain'), 0, 500, B_TRIANGLE_THUMB, B_FOLLOW_LEFT_RIGHT );
	slider->SetBarColor( dark_blue ); 
	slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	slider->SetHashMarkCount(6);
	slider->SetLimitLabels("0%", "500%");
	slider->SetValue(prefs.fGain);
	slider->SetDefaultValue(100);
	slider->SetFontSize( 10 );
	box->AddChild( slider );

	// ----------- 3. Box ------------------------------
	bounds.top = bounds.bottom + 10; bounds.bottom = bounds.top + 132;
	box = new BBox( bounds, NULL, B_FOLLOW_ALL);
	box->SetLabel( "Special" );
	AddChild( box );

	frame = BRect( 10, 18, bounds.right - 21, 22 );

	slider = new RealtimeSlider(frame, "reverb", "Reverb", new BMessage('Reve'), (int)B_REVERB_NONE, (int)B_REVERB_DUNGEON, B_TRIANGLE_THUMB, B_FOLLOW_LEFT_RIGHT );
	slider->SetBarColor( dark_blue ); 
	slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	slider->SetHashMarkCount((int)B_REVERB_DUNGEON - (int)B_REVERB_NONE + 1);
	slider->SetLimitLabels("None", "Dungeon");
	slider->SetValue((int)prefs.fReverbMode);
	slider->SetDefaultValue((int)B_REVERB_BALLROOM);
	slider->SetFontSize( 10 );
	box->AddChild( slider );

	frame.OffsetBy ( 0, 58 );
	slider = new RealtimeSlider(frame, "transposition", "Transposition", new BMessage('Tran'), -12, +12, B_TRIANGLE_THUMB, B_FOLLOW_LEFT_RIGHT );
	slider->SetBarColor( dark_blue ); 
	slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	slider->SetHashMarkCount(13);
	slider->SetLimitLabels("-12", "+12");
	slider->SetValue((int)prefs.fTransposition);
	slider->SetDefaultValue(0);
	slider->SetFontSize( 10 );
	box->AddChild( slider );
	
	// Quit, Show Replicant
	bounds.left += 12;
	bounds.top = bounds.bottom + 10; bounds.bottom = bounds.top + 16;
	checkbox = new BCheckBox( bounds, "showreplicant", "Show replicant in Deskbar", new BMessage('Repl') );
	checkbox->SetValue( prefs.fShowReplicant );
	AddChild( checkbox );
	
	// Für nachfolgendes Draw()
	font.SetFamilyAndStyle("Swis721 BT", "Roman");
	font.SetSize(9);
	font.SetRotation(90.0);
	font.SetSpacing(B_CHAR_SPACING);
	SetFont(&font);

	SetLowColor(255, 255, 255);
	
}

SettingsMainView::~SettingsMainView()
{
	delete fIcon;
}

void SettingsMainView::MouseDown( BPoint where )
{
	Window()->Activate();
	BView::MouseDown( where );
}

void SettingsMainView::Draw( BRect updateRect ) {

	DrawBitmap( fIcon, BPoint( 10, 16 ) );
	
	MovePenTo( Bounds().right - 75, Bounds().top + 50 );
	DrawString("Signal");
	
	MovePenTo( Bounds().right - 52, Bounds().top + 50 );
	DrawString("Output");
}
