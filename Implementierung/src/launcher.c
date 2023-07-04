//
// Created by Cato on 22.06.23.
//
#define _POSIX_C_SOURCE 199309L

#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <time.h>

#include "IOSystem/IO_tools.h"
#include "launcher.h"
#include "Implementierung/basic_sobel.h"
#include "Implementierung/test_basic_sobel.h"
#include "Implementierung/test_similarity.h"

#define BMP_EXTENSION ".bmp"
#define OUTPUT_MARK "_out" BMP_EXTENSION

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

void print_arg_error(char *errorMsg, config *config_params) {
    fprintf(stderr, "%s\n", errorMsg);
    print_help_msg();
    free(config_params);
    exit(1);
}

void parseArgs(int argc, char *argv[], config *config_params) {
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
                    print_arg_error("Version number can't start with a dash", config_params);
                }
                config_params->version = (uint8_t) strtoul(optarg, NULL, 10);
                break;
            case 'B':
                config_params->measure_performance = true;
                if (optarg != NULL) {
                    if (optarg[0] >= '0' && optarg[0] <= '9') {
                        config_params->measure_performance_cycles = strtoul(optarg, NULL, 10);
                    } else {
                        print_arg_error("Optional argument cycles has to be a number", config_params);
                    }
                }
                // enforce minimum of 3 cycles
                config_params->measure_performance_cycles =
                        config_params->measure_performance_cycles > 3 ? config_params->measure_performance_cycles : 3;
                break;
            case 't':
                config_params->run_tests = true;
                break;
            case 'h':
                print_help_msg();
                exit(0);
            case 'o':
                if (optarg[0] == '-') {
                    print_arg_error("Output path can't start with a dash", config_params);
                }
                config_params->outputFilePath = optarg;
                break;
            case '?':
                print_arg_error("Unknown argument", config_params);
            case ':':
                switch (optopt) {
                    case 'V':
                        print_arg_error("Missing version number -V version_num", config_params);
                    case 'o':
                        print_arg_error("No output path for generated file was given -o output_path", config_params);
                }
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        print_arg_error("No input path was given", config_params);
    } else if (argc > 1) {
        print_arg_error("Multiple input path were given", config_params);
    }

    size_t input_path_len = strlen(argv[0]) + 1;
    config_params->inputFilePath = malloc(input_path_len);
    strncpy(config_params->inputFilePath, argv[0], input_path_len);
    for (char* c = config_params->inputFilePath; *c; c++) {
        *c = tolower(*c);
    }

    //Set outputFilePath if not given
    if (config_params->outputFilePath == NULL) {
        size_t len_input_name;
        if (strstr(config_params->inputFilePath, BMP_EXTENSION) != config_params->inputFilePath + input_path_len - strlen(BMP_EXTENSION) - 1) {
            len_input_name = input_path_len - 1;
        } else {
            len_input_name = input_path_len - strlen(BMP_EXTENSION) - 1;
        }
        config_params->outputFilePath = malloc(len_input_name + strlen(OUTPUT_MARK) + 1);
        strncpy(config_params->outputFilePath, config_params->inputFilePath, len_input_name);
        strncat(config_params->outputFilePath, OUTPUT_MARK, strlen(OUTPUT_MARK) + 1);
    }
}

int main(int argc, char *argv[]) {
    config *config_params = malloc(sizeof(config));
    if (config_params == NULL) {
        fprintf(stderr, "Couldn't allocate memory for config parameters\n");
        exit(1);
    }
    parseArgs(argc, argv, config_params);
    uBMPImage *bmpImage = malloc(sizeof(uBMPImage));
    printf("Loading image from inputFilePath\n");
    size_t img_size = loadPicture(config_params->inputFilePath, bmpImage);
    if (img_size == 0) {
        fprintf(stderr, "Couldn't load picture from input file %s\n", config_params->inputFilePath);
        free(bmpImage);
        free(config_params);
        exit(1);
    }

    uint8_t *newPixels = calloc(bmpImage->pxArraySize, sizeof(3));
    if (newPixels == NULL) {
        fprintf(stderr, "Couldn't allocate memory for newPixels\n");
        free(bmpImage);
        free(config_params);
        exit(1);
    }

    if (config_params->run_tests) {
        runTestsSobel();
    }

    double time = 0;
    int num_of_execute_cycles = config_params->measure_performance ? config_params->measure_performance_cycles : 1;
    for (int i = 1; i <= num_of_execute_cycles; ++i) {
        printf("\rCycles run: %d/%d", i, num_of_execute_cycles);
        fflush(stdout);
        struct timespec start;
        struct timespec end;
        switch (config_params->version) {
            case 0:
                clock_gettime(CLOCK_MONOTONIC, &start);
                sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, newPixels);
                clock_gettime(CLOCK_MONOTONIC, &end);
                time += end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
                break;
            default:
                fprintf(stderr, "Version %d not implemented", config_params->version);
                free(config_params);
                free(bmpImage);
                free(newPixels);
                exit(1);
        }
    }

    printf("\nCalculated sobel for image %s using version %d\n", config_params->inputFilePath, config_params->version);

    //#---------------------
    //#Erste Output Integration im Launcher - up to change
    //#
    uBMPImage exportImage = {.pxArray = (pixel24_t *) newPixels, .pxArraySize = bmpImage->pxArraySize, .pxHeight = bmpImage->pxHeight, .pxWidth = bmpImage->pxWidth};
    size_t newSize;
    char *newBuf = arrayToBmp(&exportImage, &newSize);

    printf("Writing to file %s\n", config_params->outputFilePath);
    writeFile(config_params->outputFilePath, newBuf, newSize);
    free(newBuf);
    //#---------------------

    if (config_params->run_tests || config_params->measure_performance) {
        uint8_t *sobelReferenceVersion = calloc(bmpImage->pxArraySize, sizeof(pixel24_t));
        if (sobelReferenceVersion == NULL) {
            fprintf(stderr, "Couldn't allocate memory for sobelReferenceVersion\n");
            free(newPixels);
            free(bmpImage);
            free(config_params);
            exit(1);
        }
        // We use version 0 as a reference for the optimized Versions testing the output of version 0 against the output of version 0 is pointless
        //if (config_params->version != 0) {
        printf("Comparing generated output image to output of version 0 algorithm\n");
        sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, sobelReferenceVersion);
        runTestSimilarity(config_params->outputFilePath, sobelReferenceVersion,
                          bmpImage->pxArraySize * sizeof(pixel24_t));
        //}
        free(sobelReferenceVersion);
    }

    if (config_params->measure_performance) {
        printf("Sobel calculation from version %d took on average %f seconds\n", config_params->version,
               time / config_params->measure_performance_cycles);
    }

    free(newPixels);
    free(bmpImage);
    free(config_params);
}
