//
// Created by Cato on 22.06.23.
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

int main(int argc, char *argv[]) {

    uint8_t version = 0;
    bool measure_runtime = false;
    uint32_t measure_runtime_cycles = 1;
    bool display_help_msg = false;
    char *outputFilePath = "";
    char *inputFilePath = "";

    static struct option longopts[] = {
            {"help", no_argument, NULL, 'h'},
            {NULL, 0,             NULL, 0}
    };

    int ch;
    while ((ch = getopt_long(argc, argv, "+V:B::o:h", longopts, NULL)) != -1) {
        switch (ch) {
            case 'V':
                version = (uint8_t) strtoul(optarg, NULL, 10);
                break;
            case 'B':
                measure_runtime = true;
                // for optional arguments it is expected that the value follows the flag directly
                // -B12 would be correct
                // to allow for -B 12 we first check if another value is present in argv and test if it's a flag
                if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-') {
                    measure_runtime_cycles = strtoul(argv[optind], NULL, 10);
                    ++optind;
                    break;
                }
                break;
            case 'h':
                display_help_msg = true;
                break;
            case 'o':
                outputFilePath = optarg;
                break;
            case ':':
                switch (optopt) {
                    case 'V':
                        fprintf(stderr, "Missing version number -V version_num\n");
                        break;
                    case 'o':
                        fprintf(stderr, "No output path for generated file was given -o output_path\n");
                        break;
                }
            default:
                abort();

        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        fprintf(stderr, "More than one positional argument given\n");
    }

    inputFilePath = argv[0];

    printf("Version = %u\n"
           "Measure runtime = %s\n"
           "Runtime cycles = %d\n"
           "Display help message = %s\n"
           "Output file path = %s\n"
           "Input file path = %s\n", version, measure_runtime ? "true" : "false", measure_runtime_cycles,
           display_help_msg ? "true" : "false", outputFilePath, inputFilePath);

    return 0;

}
