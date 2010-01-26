#ifndef CVOUT_H
#define CVOUT_H

#include "cvbase.h"

#define MAX_MIDI_CHANNELS 16

class CVOut : public CVBase
{
private:
	jack_port_t *m_midi_in;

	char m_nrpn_msb[MAX_MIDI_CHANNELS];
	char m_nrpn_lsb[MAX_MIDI_CHANNELS];
	bool m_nrpn[MAX_MIDI_CHANNELS];

	void shutdown();
	int process(jack_nframes_t nframes);

public:
	void start();
	void stop();
};

#endif // CVOUT_H
