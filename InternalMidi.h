/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _INTERNAL_SYNTH_H
#define _INTERNAL_SYNTH_H

#include <MidiSynth.h>
#include <MidiConsumer.h>
#include <OS.h>
#include <View.h>

class StatusView;

// -------------- InternalMidi ---------------------- 

class InternalMidi
	:	public BMidiLocalConsumer
{

public:

					InternalMidi();
					~InternalMidi();

void 				SetVolume(double volume) { fMidiSynth.SetVolume(volume); }
void				ProgramChange (int channel, int inst) { fMidiSynth.ProgramChange(channel, inst); }
void				NoteOff( uchar channel, uchar note, uchar velocity, bigtime_t time);
void				NoteOn( uchar channel, uchar note, uchar velocity, bigtime_t time);
void				KeyPressure( uchar channel, uchar note, uchar pressure, bigtime_t time);
void				ControlChange( uchar channel, uchar ontrolNumber, uchar ontrolValue, bigtime_t time);
void				ProgramChange( uchar channel, uchar programNumber, bigtime_t time);
void				ChannelPressure( uchar channel, uchar pressure, bigtime_t time );
void				PitchBend( uchar channel, uchar lsb, uchar msb, bigtime_t time);

bool				fIsLoaded;

BMidiSynth			fMidiSynth;
bool				LoadFromPrefs();
sem_id				sem;
};

// -------------- StatusView ---------------------- 
class StatusView : public BView {

public:
					StatusView( BRect frame );
void				Draw(BRect updateRect);
void				SetStatustext(const char *);
					~StatusView();			

private:

BBitmap				*bitmap;

};

#endif
