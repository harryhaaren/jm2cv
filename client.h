#ifndef CLIENT_H
#define CLIENT_H

#include <jack/jack.h>

typedef jack_default_audio_sample_t sample_t;
typedef jack_nframes_t              tick_t;

class Client
{
private:
	jack_client_t   *m_client;
protected:
	jack_port_t    **m_ports;
	const char      *m_name;
	jack_nframes_t   m_sample_rate;

private:
	static void shutdown(void *arg);
	static int process(jack_nframes_t nframes, void *arg);

	virtual void shutdown() = 0;
	virtual int process(jack_nframes_t) = 0;

public:
	Client() : m_client(NULL), m_ports(NULL), m_name(NULL), m_sample_rate(0)
	{
	}

	~Client()
	{
	}

	bool open(const char *name);
	void close();

	jack_port_t *port_register(const char *port_name, const char *port_type, unsigned long flags, unsigned long buffer_size);
};

#endif // CLIENT_H
