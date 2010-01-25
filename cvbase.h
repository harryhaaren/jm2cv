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

	sample_t **m_buffers;

	MappingList::iterator m_it_begin;
	MappingList::iterator m_it_end;

public:
	void add_mapping(Mapping m)
	{
		m_mapping_list.push_back(m);
	}

	void stop()
	{
		close();
	}
};

#endif // CVBASE_H