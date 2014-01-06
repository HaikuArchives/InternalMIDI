#ifndef _PREFS_H
#define _PREFS_H

#include <Path.h>
#include <String.h>
#include <Synth.h>

#define	PREFS_FILENAME	"InternalMIDI_settings"

class Prefs {

public:
					Prefs();
					~Prefs();

bool				fPause;
BString				fSynthFile;
int32				fSamplingRate;
interpolation_mode	fInterpolation;
int32				fGain;
reverb_mode			fReverbMode;
int32				fTransposition;
bool				fShowReplicant;

BPath				fSynthDirectory;

private:

BPath				fSavePath;

};

extern Prefs prefs;
	
#endif