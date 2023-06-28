//
// Created by Cato on 22.06.23.
//

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "launcher.h"
#include "Implementierung/basic_sobel.h"
#include "Implementierung/test_basic_sobel.h"

void print_help_msg(void) {
    printf("Program to calculate sobel from BMP file\n");
    printf("Usage: ./program [arguments] output_path\n");
    printf("Arguments:\n");
    printf("\t-V <version_number>   Which implementation to use (0: reference, 1: SIMD, 2: SIMD+Threads\n");
    printf("\t-o <output_file_path> Output path to write the resulting image to\n");
    printf("\t-B [cycles]           Run performance tests 1 or if provided cycle times\n");
    printf("\t-t                    Run all unit tests\n");
    printf("\t-h | --help           Display this message\n");
}

void print_arg_error(char *errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    print_help_msg();
    exit(1);
}

void parseArgs(int argc, char *argv[]) {
    static struct option longopts[] = {
            {"help", no_argument, NULL, 'h'},
            {NULL, 0,             NULL, 0}
    };

    int ch;
    int option_index = 0;
    while ((ch = getopt_long(argc, argv, ":V:o:hB::t", longopts, &option_index)) != -1) {
        switch (ch) {
            case 'V':
                if (optarg[0] == '-') {
                    print_arg_error("Version number can't start with a dash");
                }
                config_params->version = (uint8_t) strtoul(optarg, NULL, 10);
                break;
            case 'B':
                config_params->measure_performance = true;
                // for optional arguments it is expected that the value follows the flag directly
                // -B12 would be correct
                // to allow for -B 12 we first check if another value is present in argv and test if it's a flag
                if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-') {
                    config_params->measure_performance_cycles = strtoul(argv[optind], NULL, 10);
                    ++optind;
                    break;
                }
                break;
            case 't':
                config_params->run_unit_tests = true;
                break;
            case 'h':
                print_help_msg();
                exit(0);
            case 'o':
                if (optarg[0] == '-') {
                    print_arg_error("Output path can't start with a dash");
                }
                config_params->outputFilePath = optarg;
                break;
            case '?':
                print_arg_error("Unknown argument");
            case ':':
                switch (optopt) {
                    case 'V':
                        print_arg_error("Missing version number -V version_num");
                    case 'o':
                        print_arg_error("No output path for generated file was given -o output_path");
                }
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        print_arg_error("No input path was given");
    } else if (argc > 1) {
        print_arg_error("Multiple input path were given");
    }

    config_params->inputFilePath = argv[0];
}

int main(int argc, char *argv[]) {
    config_params = malloc(sizeof(config));
    parseArgs(argc, argv);
    uBMPImage *bmpImage = malloc(sizeof(uBMPImage));
    size_t img_size = loadPicture(config_params->inputFilePath, bmpImage);
    if (img_size == 0){
        fprintf(stderr, "Couldn't load picture from input file %s\n", config_params->inputFilePath);
        free(bmpImage);
        free(config_params);
        exit(1);
    }
    uint8_t *newPixels = malloc(bmpImage->pxHeight * bmpImage->pxWidth * sizeof(pixel24_t));
    if (newPixels == NULL){
        fprintf(stderr, "Couldn't allocate memory for newPixels\n");
        free(bmpImage);
        free(config_params);
        exit(1);
    }
    if (config_params->run_unit_tests){
        runTestsSobel();
    }
    if (config_params->version == 0){
        sobel((uint8_t*) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, newPixels);
        printf("Calculated sobel for image %s with naive implementation\n", config_params->inputFilePath);
    }
    free(newPixels);
    free(bmpImage);
    free(config_params);
}