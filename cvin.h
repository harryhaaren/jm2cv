#ifndef CVIN_H
#define CVIN_H

#include "cvbase.h"

class CVIn : public CVBase
{
private:
	jack_port_t *m_midi_out;

	void shutdown();
	int process(jack_nframes_t nframes);

public:
	void start();
	void stop();
};

#endif // CVIN_H
