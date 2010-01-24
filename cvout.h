#include "client.h"
#include "mapping.h"

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

