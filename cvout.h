#include "client.h"
#include "mapping.h"

#define MAX_MIDI_CHANNELS 16

class CVOut : Client
{
private:
	MappingList  m_mapping_list;
	jack_port_t *m_midi_in;
	bool         m_ready;
	tick_t       m_tick;

	sample_t **m_buffers;

	MappingList::iterator m_it_begin;
	MappingList::iterator m_it_end;

	char m_nrpn_msb[MAX_MIDI_CHANNELS];
	char m_nrpn_lsb[MAX_MIDI_CHANNELS];
	bool m_nrpn[MAX_MIDI_CHANNELS];

	void shutdown();
	int process(jack_nframes_t nframes);

public:
	void add_mapping(Mapping m)
	{
		m_mapping_list.push_back(m);
	}

	void start();
	void stop()
	{
		close();
	}
};

