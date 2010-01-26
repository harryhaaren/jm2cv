#include <jack/jack.h>
#include <jack/midiport.h>
#include "cvout.h"

void CVOut::shutdown()
{
}

int CVOut::process(jack_nframes_t nframes)
{
	if (!m_ready) return 0;

	void *midi_in = jack_port_get_buffer(m_midi_in, nframes);

	for (unsigned i = 0; i < m_mapping_list.size(); i++) {
		m_buffers[i] = static_cast<sample_t *>(jack_port_get_buffer(m_ports[i], nframes));
	}

	jack_nframes_t event_count = jack_midi_get_event_count(midi_in);
	jack_nframes_t event_index = 0;
	jack_midi_event_t ev;

	if (0 < event_count) jack_midi_event_get(&ev, midi_in, event_index);

	for (jack_nframes_t f = 0; f < nframes; f++) {
		while (event_index < event_count && ev.time == f) {
			int channel = ev.buffer[0] & 0x0F;
			MappingList::iterator it;

			switch (ev.buffer[0] & 0xF0) {
			case 0xB0:
				if (ev.buffer[1] == 99) {
					m_nrpn_msb[channel] = ev.buffer[2];
					m_nrpn[channel] = true;
				} else if (ev.buffer[1] == 98) {
					m_nrpn_lsb[channel] = ev.buffer[2];
					m_nrpn[channel] = true;
				} else if (ev.buffer[1] == 101) {
					m_nrpn_msb[channel] = ev.buffer[2];
					m_nrpn[channel] = false;
				} else if (ev.buffer[1] == 100) {
					m_nrpn_lsb[channel] = ev.buffer[2];
					m_nrpn[channel] = false;
				} else {
					if (ev.buffer[1] == 6 || ev.buffer[1] == 38) {
						ControllerType type = m_nrpn[channel] ? TYPE_NRPN : TYPE_RPN;
						bool msb = ev.buffer[1] == 6;

						for (it = m_it_begin; it != m_it_end; ++it) {
							Mapping *m = &(*it);
							if (!m->match(type, channel)) continue;
							if (m_nrpn_msb[channel] != m->ccmsb) continue;
							if (m_nrpn_lsb[channel] != m->cclsb) continue;

							if (msb) {
								if (m->has_lsb) {
									m->cur_mv = ev.buffer[2] << 7;
								} else {
									m->cur_mv = ev.buffer[2];
									m->interp_cvout(m_tick);
								}
							} else {
								m->cur_mv &= ~0x7F;
								m->cur_mv |= ev.buffer[2];
								m->interp_cvout(m_tick);
							}
						}
					} else {
						for (it = m_it_begin; it != m_it_end; ++it) {
							Mapping *m = &(*it);
							if (!m->match(TYPE_CC, channel)) continue;

							if (ev.buffer[1] == m->ccmsb) {
								if (m->has_lsb) {
									m->cur_mv = ev.buffer[2] << 7;
								} else {
									m->cur_mv = ev.buffer[2];
									m->interp_cvout(m_tick);
								}
							} else if (ev.buffer[1] == m->cclsb) {
								m->cur_mv &= ~0x7F;
								m->cur_mv |= ev.buffer[2];
								m->interp_cvout(m_tick);
							}
						}
					}
				}
				break;

			case 0xE0:
				for (it = m_it_begin; it != m_it_end; ++it) {
					Mapping *m = &(*it);
					if (!m->match(TYPE_PB, channel)) continue;

					m->cur_mv = ev.buffer[1] | (ev.buffer[2] << 7);
					m->interp_cvout(m_tick);
				}
				break;
			}

			event_index++;
			if (event_index < event_count) jack_midi_event_get(&ev, midi_in, event_index);
		}

		int port = 0;
		MappingList::iterator it;
		for (it = m_it_begin; it != m_it_end; ++it, port++) {
			const Mapping *m = &(*it);

			*m_buffers[port]++ = m->last_cv;
		}

		m_tick++;
	}
	return 0;
}

void CVOut::start()
{
	m_ready = false;
	m_tick  = 0;

	if (m_mapping_list.size() == 0) return;

	m_ports = new jack_port_t *[m_mapping_list.size()];
	m_buffers = new sample_t *[m_mapping_list.size()];

	if (!open(m_config_name.empty() ? "m2cv_out" : m_config_name.c_str())) exit(EXIT_FAILURE);

	m_midi_in = port_register("midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

	MappingList::iterator it_begin = m_mapping_list.begin();
	MappingList::iterator it_end   = m_mapping_list.end();

	int port = 0;
	MappingList::iterator it;
	for (it = it_begin; it != it_end; ++it, port++) {
		Mapping *m = &(*it);
		m_ports[port] = port_register(m->name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

		m->latency_ticks = m_sample_rate * m->latency / 1000.0f;
	}

	m_it_end   = it_end;
	m_it_begin = it_begin;
	m_ready    = true;
}

void CVOut::stop()
{
	if (m_mapping_list.size() == 0) return;

	close();

	delete [] m_ports;
	delete [] m_buffers;
}
