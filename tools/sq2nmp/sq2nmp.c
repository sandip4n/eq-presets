// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Filter types to be used with filter.type below */
#define FILTER_TYPE_NONE	0x0
#define FILTER_TYPE_PEAK	0x1	/* Peak */
#define FILTER_TYPE_LO_SHELF	0x2	/* Low Shelf */
#define FILTER_TYPE_HI_SHELF	0x3	/* High Shelf */

#define NR_FILTER_TYPES		4
#define NR_LINE_CHARS		64
#define NR_STATE_CHARS		4

struct filter {
	unsigned int	type;		/* Type */
	unsigned int	freq;		/* Frequency */
	float		gain;		/* Gain */
	float		quality;	/* Quality Factor */
};

/* neutron filter types */
const static char *nftypes[] = {
	"PEAKEQ",
	"LOWSHELF",
	"HIGHSHELF",
};

int main(int argc, char **argv)
{
	char state[NR_STATE_CHARS];
	char type[NR_FILTER_TYPES];
	char line[NR_LINE_CHARS];
	struct filter f;
	float preamp;
	FILE *file;

	if (argc < 2) {
		fprintf(stderr, "error: invalid file\n");
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	file = fopen(argv[1], "r");
	if (!file) {
		fprintf(stderr, "error: inaccessible file\n");
		return EXIT_FAILURE;
	}

	if (!fgets(line, NR_LINE_CHARS, file)) {
		fprintf(stderr, "error: invalid file format\n");
		return EXIT_FAILURE;
	}

	/* Preamp: x.x dB */
	if (sscanf(line, "Preamp: %f dB\n", &preamp) != 1) {
		fprintf(stderr, "error: invalid file format\n");
		return EXIT_FAILURE;
	}

	printf("\t<preset id=\"0\" name=\"lorem-ipsum\" bind=\"0\" lock=\"3\" preamp=\"%f\">\n", preamp);

	while (fgets(line, NR_LINE_CHARS, file)) {
		/* Filter x: ON PK Fc x Hz Gain x.x dB Q x.x */
		if (sscanf(line, "Filter %*u: %s %s Fc %u Hz Gain %f dB Q %f\n", state, type, &f.freq, &f.gain, &f.quality) != 5) {
			fprintf(stderr, "error: invalid file format\n");
			return EXIT_FAILURE;
		}

		if (strncmp(state, "ON", sizeof(state)))
			continue;

		if (!strncmp(type, "PK", sizeof(type)))
			f.type = FILTER_TYPE_PEAK;
		else if (!strncmp(type, "LSC", sizeof(type)))
			f.type = FILTER_TYPE_LO_SHELF;
		else if (!strncmp(type, "HSC", sizeof(type)))
			f.type = FILTER_TYPE_HI_SHELF;
		else
			f.type = FILTER_TYPE_NONE;

		if (!f.type) {
			fprintf(stderr, "error: invalid file format\n");
			return EXIT_FAILURE;
		}

		/* Filters without Gain or Q factor do not change anything */
		if (!f.gain || !f.quality)
			continue;

		printf("\t\t<band type=\"%s\" gain=\"%f\" freq=\"%u\" Q=\"%f\" />\n", nftypes[f.type - 1], f.gain, f.freq, f.quality);
	}

	printf("\t</preset>\n");
	fclose(file);

	return EXIT_SUCCESS;
}
