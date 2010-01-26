#ifndef CVBASE_H
#define CVBASE_H

#include "client.h"
#include "mapping.h"

class CVBase : protected Client
{
protected:
	MappingList  m_mapping_list;
	bool         m_ready;
	tick_t       m_tick;
	std::string  m_config_name;

	sample_t **m_buffers;

	MappingList::iterator m_it_begin;
	MappingList::iterator m_it_end;

public:
	void add_mapping(Mapping m)
	{
		m_mapping_list.push_back(m);
	}

	bool find(const char *name) const;

	void stop()
	{
		close();
	}

	void set_name(const char *name)
	{
		m_config_name = name;
	}
};

#endif // CVBASE_H
