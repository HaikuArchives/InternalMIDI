/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "InternalMidi.h"

#include <Alert.h>
#include <AppFileInfo.h>
#include <Application.h>
#include <Bitmap.h>
#include <Message.h>
#include <Midi.h>
#include <MidiSynth.h>
#include <Path.h>
#include <Roster.h>
#include <scheduler.h>
#include <Screen.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "Prefs.h"

/* -------------- InternalMidi ------------------------------- */

InternalMidi::InternalMidi()
:	BMidiLocalConsumer("InternalMIDI"),
	fIsLoaded( false )
{

	set_thread_priority(find_thread(NULL), 50);
	
	sem = create_sem(1, "blocker");

	// Find Icon...
	entry_ref	ref;
	be_roster->FindApp( APP_SIGNATURE, &ref);

	BFile			file(&ref, B_READ_WRITE);
	BAppFileInfo	appFileInfo(&file);
	
	BMessage props;
	if (GetProperties(&props) == B_OK) {

		BBitmap			*bitmap;
		bitmap = new BBitmap(BRect(0,0,31,31), B_CMAP8);
		appFileInfo.GetIcon(bitmap, B_LARGE_ICON);
		props.AddData("be:large_icon", 'ICON', bitmap->Bits(), 1024);
		delete bitmap;
	
		bitmap = new BBitmap(BRect(0,0,15,15), B_CMAP8);
		appFileInfo.GetIcon(bitmap, B_MINI_ICON);
		props.AddData("be:small_icon", 'MICN', bitmap->Bits(), 256);
		delete bitmap;

		SetProperties( &props );
		
	}

	// ...and register
	if (Register() == B_OK) fIsLoaded = true;

	// Load the samples
	if (!LoadFromPrefs()) return;

}

InternalMidi::~InternalMidi() {

	acquire_sem(sem);
	
	if (fIsLoaded) Unregister();

	release_sem(sem);
	delete_sem(sem);

}

bool InternalMidi::LoadFromPrefs() {

	if (acquire_sem(sem) != B_OK) return false;

	// Open Status window
	BRect	screenFrame(BScreen().Frame());
	BRect	winFrame(0.0, 0.0, 280.0, 35.0);

	winFrame.OffsetTo((screenFrame.Width() - winFrame.Width())/2, (screenFrame.Height() - winFrame.Height())/3);

	BWindow 	*statuswindow = new BWindow(winFrame, NULL, B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_MOVABLE|B_NOT_RESIZABLE|B_AVOID_FRONT|B_AVOID_FOCUS);
	StatusView	*statusview	= new StatusView(winFrame);
	statusview->MoveTo( 0.0, 0.0 );
	statuswindow->AddChild( statusview );
	statuswindow->Show();

	// Synthesizer available?
	BPath	path	= prefs.fSynthDirectory;
	path.SetTo( path.Path(), prefs.fSynthFile.String() );
	
	entry_ref	ref;
	get_ref_for_path(path.Path(), &ref);
	
	// Load sounds and incorporate the remaining settings
	be_synth->Pause();
	bool loaded = be_synth->LoadSynthData(&ref) == B_OK && be_synth->IsLoaded();

	if (loaded) {

		fMidiSynth.EnableInput(true, true);
		be_synth->SetSamplingRate( prefs.fSamplingRate);
		be_synth->SetInterpolation( prefs.fInterpolation);

		fMidiSynth.SetVolume( (double)prefs.fGain / 100 );

		fMidiSynth.SetTransposition( prefs.fTransposition );
		be_synth->SetReverb( prefs.fReverbMode );

		if (!prefs.fPause) be_synth->Resume();
	}
	
	// Close Statuswindow
	statuswindow->Lock();
	statuswindow->Quit();

	release_sem(sem);
	
	return loaded;

}

void InternalMidi::NoteOff( uchar channel, uchar note, uchar velocity, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.NoteOff(channel + 1, note, velocity, time / 1000);
	release_sem( sem );
}

void InternalMidi::NoteOn( uchar channel, uchar note, uchar velocity, bigtime_t time) {

	if (acquire_sem(sem) != B_OK) return;

	fMidiSynth.NoteOn(channel + 1, note, velocity, time / 1000);

	BWindow *win = be_app->WindowAt(0);

	if (win) {
		win->Lock();
		BView *view = win->FindView("signalview");
		win->Unlock();

		BMessage msg('Ping');
		msg.AddInt8("velocity", velocity);
		if (view) win->PostMessage(&msg, view);
	}

	release_sem( sem );

}

void  InternalMidi::KeyPressure( uchar channel, uchar note, uchar pressure, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.KeyPressure(channel + 1, note, pressure, time / 1000);
	release_sem( sem );
}

void InternalMidi::ControlChange( uchar channel, uchar controlNumber, uchar controlValue, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.ControlChange(channel + 1, controlNumber, controlValue, time / 1000);
	release_sem( sem );
}

void  InternalMidi::ProgramChange( uchar channel, uchar programNumber, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.ProgramChange(channel + 1, programNumber, time / 1000);
	release_sem( sem );
}

void InternalMidi::ChannelPressure( uchar channel, uchar pressure, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.ChannelPressure(channel + 1, pressure, time / 1000);
	release_sem( sem );
}

void InternalMidi::PitchBend( uchar channel, uchar lsb, uchar msb, bigtime_t time) {
	if (acquire_sem(sem) != B_OK) return;
	fMidiSynth.PitchBend(channel + 1, lsb, msb, time / 1000);
	release_sem( sem );
}

/* ---------- Status View --------------------------------------- */

StatusView::StatusView( BRect frame ) : BView( frame, NULL, 0, B_WILL_DRAW ) {

	frame.bottom++; // must increase the size by one pixel
	BScreen().GetBitmap(&bitmap, false, &frame);

	BFont font;
	font.SetFamilyAndStyle("Swis721 BT", "Roman");
	font.SetSize(20);
	SetFont(&font);
	
};

void StatusView::Draw(BRect updateRect) {
	DrawBitmap(bitmap, BPoint(0.0, 0.0));

	SetDrawingMode(B_OP_ALPHA);
	SetHighColor( 255, 255, 255, 128 );
	FillRoundRect( Bounds(), 7.0, 7.0);
	SetHighColor( 0, 0, 0, 128 );
	StrokeRoundRect( Bounds(), 7.0, 7.0);
	SetLowColor( 255, 255, 255, 128 );
	SetHighColor( 0, 0, 0, 255 );

	DrawString("Loading patches...", BPoint( 70.0, 25.0));
};

StatusView::~StatusView() {
	delete bitmap;
};

