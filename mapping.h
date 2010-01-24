#ifndef MAPPING_H
#define MAPPING_H

#include <string>
#include <list>

enum ControllerType {
	TYPE_PB,
	TYPE_CC,
	TYPE_NRPN,
	TYPE_RPN
};

template <typename T>
T clamp(T x, T min, T max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

class Mapping
{
public:
	std::string name;
	int   channel;
	ControllerType type;
	int   ccmsb, cclsb;
	int   mrl, mru;
	float crl, cru;
	float latency;

	bool has_lsb;
	float adj1;
	float adj2;
	tick_t latency_ticks;

	int   cur_mv;
	int   last_mv;
	sample_t cur_cv;
	sample_t last_cv;

	tick_t last_tick;
	tick_t since_last_tick;

	Mapping(const char *name,
	        int channel, ControllerType type, int ccmsb, int cclsb,
	        int mrl, int mru, float crl, float cru, float latency, bool has_lsb) :
	    name(name), channel(channel), type(type), ccmsb(ccmsb), cclsb(cclsb),
	    mrl(mrl), mru(mru), crl(crl), cru(cru), latency(latency),
	    has_lsb(has_lsb),
	    cur_mv(0), last_mv(0),
	    last_cv(0.0),
	    last_tick(0), since_last_tick(0)
	{
		/* Set up adjustment values for converting between
		 * MIDI values and CV values for this mapping. This
		 * results in scaling equations that are two ops. */
		adj1 = (cru - crl) / (mru - mrl);
		adj2 = (-crl / adj1) + mrl;
	}

	void interp_cvout(tick_t tick)
	{
		cur_cv = to_cv(cur_mv);
		last_tick = tick;
		last_cv = cur_cv;
	}

	void tick_cvout()
	{
		if (last_cv == cur_cv) return;
		since_last_tick++;
	}

	bool tick_cvin()
	{
		since_last_tick++;

		if (last_mv == cur_mv) return false;
		if (since_last_tick < latency_ticks) return false;

		since_last_tick = 0;
		last_mv = cur_mv;
		return true;
	}

	inline float to_cv(int mv) const
	{
		return clamp((mv - adj2) * adj1, crl, cru);
	}

	inline int to_mv(float cv) const
	{
		return clamp((int)(cv / adj1 + adj2), mrl, mru);
	}

	inline bool match(ControllerType _type, int _channel) const
	{
		return type == _type && (channel == _channel || channel == -1);
	}
};

typedef std::list<Mapping> MappingList;

#endif // MAPPING_H
