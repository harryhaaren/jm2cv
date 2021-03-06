#include <jack/jack.h>
#include <jack/midiport.h>
#include "cvin.h"

void CVIn::shutdown()
{
}

int CVIn::process(jack_nframes_t nframes)
{
	if (!m_ready) return 0;

	void *midi_out = jack_port_get_buffer(m_midi_out, nframes);
	jack_midi_clear_buffer(midi_out);

	for (unsigned i = 0; i < m_mapping_list.size(); i++) {
		m_buffers[i] = static_cast<sample_t *>(jack_port_get_buffer(m_ports[i], nframes));
	}

	for (jack_nframes_t f = 0; f < nframes; f++) {
		int port = 0;
		MappingList::iterator it;
		for (it = m_it_begin; it != m_it_end; ++it, port++) {
			Mapping *m = &(*it);
			jack_midi_data_t buf[3];

			m->cur_mv = m->to_mv(*m_buffers[port]++);

			if (m->tick_cvin()) {
				switch (m->type) {
				case TYPE_PB:
					buf[0] = 0xE0 | m->channel;
					buf[1] = m->last_mv >> 7;
					buf[2] = m->last_mv & 0x7F;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					break;

				case TYPE_CC:
					buf[0] = 0xB0 | m->channel;
					buf[1] = m->ccmsb;
					if (m->has_lsb) {
						buf[2] = m->last_mv >> 7;
						jack_midi_event_write(midi_out, f, buf, sizeof buf);
						buf[1] = m->cclsb;
					}
					buf[2] = m->last_mv & 0x7F;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					break;

				case TYPE_NRPN:
					buf[0] = 0xB0 | m->channel;
					buf[1] = 99;
					buf[2] = m->ccmsb;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					buf[1] = 98;
					buf[2] = m->cclsb;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					buf[1] = 6;
					if (m->has_lsb) {
						buf[2] = m->last_mv >> 7;
						jack_midi_event_write(midi_out, f, buf, sizeof buf);
						buf[1] = 38;
					}
					buf[2] = m->last_mv & 0x7F;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					break;

				case TYPE_RPN:
					buf[0] = 0xB0 | m->channel;
					buf[1] = 101;
					buf[2] = m->ccmsb;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					buf[1] = 100;
					buf[2] = m->cclsb;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					buf[1] = 6;
					if (m->has_lsb) {
						buf[2] = m->last_mv >> 7;
						jack_midi_event_write(midi_out, f, buf, sizeof buf);
						buf[1] = 38;
					}
					buf[2] = m->last_mv & 0x7F;
					jack_midi_event_write(midi_out, f, buf, sizeof buf);
					break;
				}
			}
		}
	}
	return 0;
}

void CVIn::start()
{
	m_ready = false;
	m_tick  = 0;

	if (m_mapping_list.size() == 0) return;

	m_ports = new jack_port_t *[m_mapping_list.size()];
	m_buffers = new sample_t *[m_mapping_list.size()];

	if (!open(m_config_name.empty() ? "m2cv_in" : m_config_name.c_str())) exit(EXIT_FAILURE);

	m_midi_out = port_register("midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

	MappingList::iterator it_begin = m_mapping_list.begin();
	MappingList::iterator it_end   = m_mapping_list.end();

	int port = 0;
	MappingList::iterator it;
	for (it = it_begin; it != it_end; ++it, port++) {
		Mapping *m = &(*it);
		m_ports[port] = port_register(m->name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

		m->latency_ticks = m_sample_rate * m->latency / 1000.0f;
	}

	m_it_end   = it_end;
	m_it_begin = it_begin;
	m_ready    = true;
}

void CVIn::stop()
{
	if (m_mapping_list.size() == 0) return;

	close();

	delete [] m_ports;
	delete [] m_buffers;
}
