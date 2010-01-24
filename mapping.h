#ifndef MAPPING_H
#define MAPPING_H

#include <string>
#include <list>

class Mapping
{
public:
	std::string name;
	int   channel;
	int   ccmsb, cclsb;
	int   mrl, mru;
	float crl, cru;
	float latency;

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
	        int channel, int ccmsb, int cclsb,
	        int mrl, int mru, float crl, float cru, float latency) :
	    name(name), channel(channel), ccmsb(ccmsb), cclsb(cclsb),
	    mrl(mrl), mru(mru), crl(crl), cru(cru), latency(latency),
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

	void handle_cvout(int c, int cc, int val, tick_t tick)
	{
		if (c != channel && channel != -1) return;
		if (cc == ccmsb) {
			if (cclsb == -1) {
				cur_mv = val;
				interp_cvout(tick);
			} else {
				cur_mv = val << 7;
			}
		} else if (cc == cclsb) {
			cur_mv |= val;
			interp_cvout(tick);
		}
	}

	inline float to_cv(int mv)
	{
		return (mv - adj2) * adj1;
	}

	inline int to_mv(float cv)
	{
		return cv / adj1 + adj2;
	}
};

typedef std::list<Mapping> MappingList;

#endif // MAPPING_H
