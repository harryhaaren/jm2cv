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

		char type[80], name[80];
		int channel, ccmsb, cclsb, mrl, mru;
		float crl, cru;
		float latency;
		if (sscanf(buf, "%s %s %d %d %d %d %d %f %f %f", type, name, &channel, &ccmsb, &cclsb, &mrl, &mru, &crl, &cru, &latency) == 10) {

			if (ccmsb < 0 || ccmsb > 127) continue;
			if (cclsb < -1 || cclsb > 127) continue;
			if (mrl < 0 || mrl > (cclsb == -1 ? 127 : 16383)) continue;
			if (mru < 0 || mru > (cclsb == -1 ? 127 : 16383)) continue;
			if (mrl > mru) continue;
//			if (crl < -1.0f || crl > 1.0f) continue;
//			if (cru < -1.0f || cru > 1.0f) continue;
			if (crl > cru) continue;
			if (latency < 0.0f) continue;

			if (!strcmp(type, "cvout")) {
				if (channel < -1 || channel > 15) continue;
				cvout.add_mapping(Mapping(name, channel, ccmsb, cclsb, mrl, mru, crl, cru, latency));
			} else if (!strcmp(type, "cvin")) {
				if (channel < 0 || channel > 15) continue;
				cvin.add_mapping(Mapping(name, channel, ccmsb, cclsb, mrl, mru, crl, cru, latency));
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
