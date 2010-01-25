#include <cstring>
#include "cvbase.h"

bool CVBase::find(const char *name) const
{
	MappingList::const_iterator it;
	for (it = m_mapping_list.begin(); it != m_mapping_list.end(); ++it) {
		const Mapping *m = &(*it);
		if (!strcmp(m->name.c_str(), name)) return true;
	}
	return false;
}
