#include <iostream>
#include "client.h"

bool Client::open(const char *name)
{
	jack_status_t status;
	m_client = jack_client_open(name, JackNoStartServer, &status);
	if (m_client == NULL) {
		if (status & JackServerFailed) {
			std::cerr << "JACK server not running" << std::endl;
		} else {
			std::cerr << "jack_client_open() failed, status = " << status << std::endl;
		}

		return false;
	}

	m_name        = jack_get_client_name(m_client);
	m_sample_rate = jack_get_sample_rate(m_client);

	jack_on_shutdown(m_client, &shutdown, this);
	jack_set_process_callback(m_client, &process, this);

	jack_activate(m_client);

	return true;
}

void Client::close()
{
	if (m_client == NULL) return;

	jack_deactivate(m_client);
	jack_client_close(m_client);
}

void Client::shutdown(void *arg)
{
	((Client *)arg)->shutdown();
}

int Client::process(jack_nframes_t nframes, void *arg)
{
	return ((Client *)arg)->process(nframes);
}

jack_port_t *Client::port_register(const char *port_name, const char *port_type, unsigned long flags, unsigned long buffer_size)
{
	return jack_port_register(m_client, port_name, port_type, flags, buffer_size);
}
