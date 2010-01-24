#include <iostream>
#include <cstring>
#include <csignal>

#include "cvout.h"
#include "cvin.h"

static CVOut cvout;
static CVIn cvin;

bool read_config(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		std::cerr << "Unable to open '" << filename << "' for reading" << std::endl;
		return false;
	}

	while (!feof(f)) {
		char buf[80];
		fgets(buf, sizeof buf, f);

		/* Ignore comments */
		if (buf[0] == '#') continue;

		char dir[80], name[80], type[80];
		int channel, ccmsb, cclsb, mrl, mru;
		float crl, cru;
		float latency;
		if (sscanf(buf, "%s %s %d %s %d %d %d %d %f %f %f", dir, name, &channel, type, &ccmsb, &cclsb, &mrl, &mru, &crl, &cru, &latency) == 11) {

			ControllerType itype;
			bool has_lsb = true;

			if (!strcmp(type, "pb")) itype = TYPE_PB;
			else if (!strcmp(type, "cc")) itype = TYPE_CC;
			else if (!strcmp(type, "nrpn")) itype = TYPE_NRPN;
			else if (!strcmp(type, "nrpn7")) { itype = TYPE_NRPN; has_lsb = false; }
			else if (!strcmp(type, "rpn")) itype = TYPE_RPN;
			else if (!strcmp(type, "rpn7")) { itype = TYPE_RPN; has_lsb = false; }
			else continue;

			if (itype == TYPE_CC && cclsb == -1) has_lsb = false;

			if (itype == TYPE_PB) {
				if (ccmsb != -1) continue;
				if (cclsb != -1) continue;
				if (mrl < 0 || mrl > 16383) continue;
				if (mru < 0 || mru > 16383) continue;
				has_lsb = false;
			} else {
				if (ccmsb < 0 || ccmsb > 127) continue;
				if (cclsb < -1 || cclsb > 127) continue;
				if (mrl < 0 || mrl > (has_lsb ? 16383 : 127)) continue;
				if (mru < 0 || mru > (has_lsb ? 16383 : 127)) continue;
			}
			if (mrl > mru) continue;
//			if (crl < -1.0f || crl > 1.0f) continue;
//			if (cru < -1.0f || cru > 1.0f) continue;
			if (crl > cru) continue;
			if (latency < 0.0f) continue;

			if (!strcmp(dir, "cvout")) {
				if (channel < -1 || channel > 15) continue;
				cvout.add_mapping(Mapping(name, channel, itype, ccmsb, cclsb, mrl, mru, crl, cru, latency, has_lsb));
			} else if (!strcmp(dir, "cvin")) {
				if (channel < 0 || channel > 15) continue;
				cvin.add_mapping(Mapping(name, channel, itype, ccmsb, cclsb, mrl, mru, crl, cru, latency, has_lsb));
			}
		}
	}

	return true;
}

static int _running = false;

static void sigint_handler(int)
{
	_running = false;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config>" << std::endl;
		return 0;
	}

	if (!read_config(argv[1])) return 0;

	cvout.start();
	cvin.start();

	_running = true;

	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	while (_running) {
		sleep(1);
	}

	cvin.stop();
	cvout.stop();
}
