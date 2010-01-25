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

	int line = 0;
	bool good = true;

	while (!feof(f)) {
		line++;
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
			else {
				std::clog << line << ": Unknown type '" << type << "'." << std::endl;
				good = false;
			}

			if (itype == TYPE_CC && cclsb == -1) has_lsb = false;

			if (itype == TYPE_PB) {
				if (ccmsb != -1) continue;
				if (cclsb != -1) continue;
				if (mrl < 0 || mrl > 16383) continue;
				if (mru < 0 || mru > 16383) continue;
				has_lsb = false;
			} else {
				if (ccmsb < 0 || ccmsb > 127) {
					std::clog << line << ": CC MSB " << ccmsb << " must be between 0 and 127." << std::endl;
					good = false;
				}
				if (cclsb < -1 || cclsb > 127) {
					std::clog << line << ": CC LSB " << cclsb << " must be between -1 and 127." << std::endl;
					good = false;
				}
				if (mrl < 0 || mrl > (has_lsb ? 16383 : 127)) {
					std::clog << line << ": MIDI range lower bound " << mrl << " must be between 0 and " << (has_lsb ? 16383 : 127) << "." << std::endl;
					good = false;
				}
				if (mru < 0 || mru > (has_lsb ? 16383 : 127)) {
					std::clog << line << ": MIDI range upper bound " << mru << " must be between 0 and " << (has_lsb ? 16383 : 127) << "." << std::endl;
					good = false;
				}
			}
			if (mrl > mru) {
				std::clog << line << ": MIDI range lower bound must be below upper bound." << std::endl;
				good = false;
			}
//			if (crl < -1.0f || crl > 1.0f) continue;
//			if (cru < -1.0f || cru > 1.0f) continue;
//			if (crl > cru) continue;
			if (latency < 0.0f) {
				std::clog << line << ": Latency " << latency << " must be greater than zero." << std::endl;
				good = false;
			}

			if (!strcmp(dir, "cvout")) {
				if (channel < -1 || channel > 15) {
					std::clog << line << ": MIDI channel " << channel << " must be between -1 and 15." << std::endl;
					good = false;
				}
				if (cvout.find(name)) {
					std::clog << line << ": Name '" << name << "' already defined." << std::endl;
					good = false;
				}
				cvout.add_mapping(Mapping(name, channel, itype, ccmsb, cclsb, mrl, mru, crl, cru, latency, has_lsb));
			} else if (!strcmp(dir, "cvin")) {
				if (channel < 0 || channel > 15) {
					std::clog << line << ": MIDI channel " << channel << " must be between 0 and 15." << std::endl;
					good = false;
				}
				if (cvin.find(name)) {
					std::clog << line << ": Name '" << name << "' already defined." << std::endl;
					good = false;
				}
				cvin.add_mapping(Mapping(name, channel, itype, ccmsb, cclsb, mrl, mru, crl, cru, latency, has_lsb));
			}
		}
	}

	if (!good) {
		std::cerr << "Error(s) in configuration file, stopping." << std::endl;
	}

	return good;
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
