//
// Created by Cato on 22.06.23.
//
#define _POSIX_C_SOURCE 199309L

#define _POSIX_C_SOURCE 199309L

#include "IOSystem/bmp_definitions.h"

#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifdef _WIN32
#include <pthread_time.h>
#endif

#include "IOSystem/IO_tools.h"
#include "IOSystem/bmp_parser.h"
#include "IOSystem/test_functionality.h"
#include "launcher.h"
#include "Implementierung/basic_sobel.h"
#include "Implementierung/simd_sobel.h"
#include "Implementierung/thread_sobel.h"
#include "Implementierung/test_basic_sobel.h"
#include "Implementierung/test_similarity.h"

#define BMP_EXTENSION ".bmp"
#define OUTPUT_MARK "_out" BMP_EXTENSION
#define IO_PERFORMANCE_TEST_CYCLES 100

void print_help_msg(void) {
    printf("Program to calculate sobel from BMP file\n");
    printf("Usage: ./program [arguments] output_path\n");
    printf("Arguments:\n");
    printf("\t-V <version_number>   Which implementation to use (0: reference, 1: SIMD, 2: SIMD+Threads\n");
    printf("\t                                                   3: grayscale reference, 4: grayscale SIMD\n");
    printf("\t                                                   5: grayscale SIMD+Threads)\n");
    printf("\t-o <output_file_path> Output path to write the resulting image to\n");
    printf("\t-B [cycles]           Run performance tests 1 or if provided cycle times\n");
    printf("\t-t                    Run all unit tests\n");
    printf("\t-h | --help           Display this message\n");
}

void dealloc_config_params(config *config_params) {
    if (config_params != NULL) {
        if (config_params->inputFilePath != NULL) {
            free(config_params->inputFilePath);
        }
        if (config_params->outputFilePath != NULL) {
            free(config_params->outputFilePath);
        }
    }
}

void freeImage(uBMPImage *img) {
    if (img != NULL) {
        if (img->pxArray != NULL) {
            free(img->pxArray);
        }
        free(img);
    }
}


void print_arg_error(char *errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    print_help_msg();
}

int parseArgs(int argc, char *argv[], config *config_params) {
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
                    return 1;
                }
                config_params->version = (uint8_t) strtoul(optarg, NULL, 10);
                // check if version is 0,1 or 2
                if (config_params->version > 5) {
                    print_arg_error("Version number is not valid");
                    return 1;
                }
                break;
            case 'B':
                config_params->measure_performance = true;
                if (optarg != NULL) {
                    if (optarg[0] >= '0' && optarg[0] <= '9') {
                        config_params->measure_performance_cycles = strtoul(optarg, NULL, 10);
                    } else {
                        print_arg_error("Optional argument cycles has to be a number");
                        return 1;
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
                    print_arg_error("Output path can't start with a dash");
                    return 1;
                }
                size_t outputPathLen = strlen(optarg) + 1;
                config_params->outputFilePath = malloc(outputPathLen);
                if (config_params->outputFilePath == NULL) {
                    fprintf(stderr, "Failed allocating memory for outputFilePath\n");
                    return 1;
                }
                strncpy(config_params->outputFilePath, optarg, outputPathLen);
                break;
            case '?':
                print_arg_error("Unknown argument");
                return 1;
            case ':':
                switch (optopt) {
                    case 'V':
                        print_arg_error("Missing version number -V version_num");
                        return 1;
                    case 'o':
                        print_arg_error("No output path for generated file was given -o output_path");
                        return 1;
                }
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        print_arg_error("No input path was given");
        return 1;
    } else if (argc > 1) {
        print_arg_error("Multiple input path were given");
        return 1;
    }

    size_t input_path_len = strlen(argv[0]) + 1;
    config_params->inputFilePath = malloc(input_path_len);
    if (config_params->inputFilePath == NULL) {
        fprintf(stderr, "Failed allocating memory for inputFilePath\n");
        return 1;
    }

    strncpy(config_params->inputFilePath, argv[0], input_path_len);

    //Set outputFilePath if not given
    if (config_params->outputFilePath == NULL) {
        char searchString[input_path_len];
        strncpy(searchString, config_params->inputFilePath, input_path_len);
        for (char *c = searchString; *c; c++) {
            *c = tolower(*c);
        }

        size_t len_input_name;
        if (strstr(searchString, BMP_EXTENSION) !=
            searchString + input_path_len - strlen(BMP_EXTENSION) - 1) {
            len_input_name = input_path_len - 1;
        } else {
            len_input_name = input_path_len - strlen(BMP_EXTENSION) - 1;
        }
        config_params->outputFilePath = malloc(len_input_name + strlen(OUTPUT_MARK) + 1);
        if (config_params->outputFilePath == NULL) {
            fprintf(stderr, "Failed allocating memory for outputFilePath\n");
            return 1;
        }

        strncpy(config_params->outputFilePath, config_params->inputFilePath, len_input_name);
        strncat(config_params->outputFilePath, OUTPUT_MARK,
                strlen(OUTPUT_MARK) + 1);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    config config_params = {0, false, 3, false, NULL, NULL};
    if (parseArgs(argc, argv, &config_params)) {
        fprintf(stderr, "Couldn't parse arguments\n");
        dealloc_config_params(&config_params);
        return 1;
    }
    uBMPImage *bmpImage = malloc(sizeof(uBMPImage));
    if (bmpImage == NULL) {
        fprintf(stderr, "Failed allocating memory for bmpImage\n");
        dealloc_config_params(&config_params);
        return 1;
    }
    printf("Loading image from %s\n", config_params.inputFilePath);

    size_t img_size;
    if (config_params.version < 3) {
        img_size = loadPicture(config_params.inputFilePath, bmpImage);
    } else {
        img_size = loadPicture_graysc(config_params.inputFilePath, bmpImage);
    }

    if (img_size == 0) {
        fprintf(stderr, "Couldn't load picture from input file %s\n", config_params.inputFilePath);
        freeImage(bmpImage);
        dealloc_config_params(&config_params);
        return 1;
    }

    struct timespec start_read_io;
    struct timespec end_read_io;
    double io_read_time;
    if (config_params.measure_performance) {
        printf("Running performance tests for Image Read\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_read_io) != 0) {
            fprintf(stderr, "Couldn't get start_read_io time\n");
            freeImage(bmpImage);
            dealloc_config_params(&config_params);
            return 1;
        }
        if (config_params.version < 3){
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                // This code is nearly the same as the one in loadPicture
                // We had to copy it here to not mess up our memory
                void *buffer;
                size_t buffer_size;
                uBMPImage *test_bmp_image = malloc(sizeof(uBMPImage));
                //fprintf(stdout, "Loading BMP File: %s\n", path);
                buffer = readFile(config_params.inputFilePath, &buffer_size);
                if (buffer == NULL) {
                    fprintf(stderr, "Couldn't read BMP File\n");
                    free(bmpImage);
                    return 1;
                }
                if (bmpToArray(buffer, buffer_size, test_bmp_image) == 1) {
                    fprintf(stderr, "Couldn't parse BMP file\n");
                    free(buffer);
                    free(bmpImage);
                    return 1;
                }
                free(buffer);
                freeImage(test_bmp_image);
            }
        } else {
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                // This code is nearly the same as the one in loadPicture
                // We had to copy it here to not mess up our memory
                void *buffer;
                size_t buffer_size;
                uBMPImage *test_bmp_image = malloc(sizeof(uBMPImage));
                //fprintf(stdout, "Loading BMP File: %s\n", path);
                buffer = readFile(config_params.inputFilePath, &buffer_size);
                if (buffer == NULL) {
                    fprintf(stderr, "Couldn't read BMP File\n");
                    free(bmpImage);
                    return 1;
                }
                if (bmpToArray_graysc(buffer, buffer_size, test_bmp_image) == 1) {
                    fprintf(stderr, "Couldn't parse BMP file\n");
                    free(buffer);
                    free(bmpImage);
                    return 1;
                }
                free(buffer);
                freeImage(test_bmp_image);
            }
        }
        if(clock_gettime(CLOCK_MONOTONIC, &end_read_io) != 0) {
            fprintf(stderr, "Couldn't get end_read_io time\n");
            dealloc_config_params(&config_params);
            return 1;
        }
        io_read_time = end_read_io.tv_sec - start_read_io.tv_sec + 1e-9 * (end_read_io.tv_nsec - start_read_io.tv_nsec);
        printf("IO read performance testing took %.9fs to run %d iterations\n", io_read_time, IO_PERFORMANCE_TEST_CYCLES);
    }

    uint8_t *newPixels = calloc(bmpImage->pxArraySize, sizeof(uint8_t));
    if (newPixels == NULL) {
        fprintf(stderr, "Couldn't allocate memory for newPixels\n");
        freeImage(bmpImage);
        dealloc_config_params(&config_params);
        return 1;
    }

    if (config_params.run_tests) {
        runTestsSobel();
    }

    struct timespec start_exec;
    struct timespec end_exec;
    double exec_time;
    long num_of_execute_cycles = config_params.measure_performance ? config_params.measure_performance_cycles : 1;
    if (config_params.measure_performance) {
        printf("Running performance tests for Calculation\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_exec) != 0) {
            fprintf(stderr, "Couldn't get start_exec time\n");
            return 1;
        }
    }
    switch (config_params.version) {
        case 0:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                                                     newPixels);
            }
            break;
        case 1:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                simd_sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                                                     newPixels);
            }
            break;
        case 2:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                thread_sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                                                     newPixels);
            }
            break;
        case 3:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_graysc((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                             newPixels);
            }
            break;
        case 4:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                simd_sobel_graysc((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                             newPixels);
            }
            break;
        case 5:
            for (long i = 0; i < num_of_execute_cycles; ++i) {
                thread_sobel_graysc((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight,
                             newPixels);
            }
            break;

    }
    if (config_params.measure_performance) {
        if(clock_gettime(CLOCK_MONOTONIC, &end_exec) != 0) {
            fprintf(stderr, "Couldn't get end_exec time\n");
            return 1;
        }
        exec_time = end_exec.tv_sec - start_exec.tv_sec + 1e-9 * (end_exec.tv_nsec - start_exec.tv_nsec);
        printf("Sobel calc performance testing took %.9fs to run %ld iterations\n", exec_time, config_params.measure_performance_cycles);
    }

    printf("Calculated sobel for image %s using version %d\n", config_params.inputFilePath, config_params.version);

    //#---------------------
    //#Erste Output Integration im Launcher - up to change
    //#
    uBMPImage exportImage = {.pxArray = newPixels, .pxArraySize = bmpImage->pxArraySize, .pxHeight = bmpImage->pxHeight, .pxWidth = bmpImage->pxWidth};
    size_t newSize;
    char *newBuf;
    if (config_params.version < 3) {
        newBuf = arrayToBmp(&exportImage, &newSize);
    } else {
        newBuf = arrayToBmp_graysc(&exportImage, &newSize);
    }

    printf("Writing to file %s\n", config_params.outputFilePath);
    if (writeFile(config_params.outputFilePath, newBuf, newSize)) {
        freeImage(bmpImage);
        dealloc_config_params(&config_params);
        free(newBuf);
        return 1;
    }

    struct timespec start_write_io;
    struct timespec end_write_io;
    double io_write_time;
    if (config_params.measure_performance) {
        char* testBuf;
        printf("Running performance tests for image write\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_write_io) != 0) {
            fprintf(stderr, "Couldn't get start_write_io time\n");
            dealloc_config_params(&config_params);
            return 1;
        }
        if (config_params.version < 3){
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                testBuf = arrayToBmp(&exportImage, &newSize);
                writeFile(config_params.outputFilePath, newBuf, newSize);
                free(testBuf);
            }
        } else {
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                testBuf = arrayToBmp_graysc(&exportImage, &newSize);
                writeFile(config_params.outputFilePath, newBuf, newSize);
                free(testBuf);
            }
        }
        if(clock_gettime(CLOCK_MONOTONIC, &end_write_io) != 0) {
            fprintf(stderr, "Couldn't get end_write_io time\n");
            dealloc_config_params(&config_params);
            return 1;
        }
        io_write_time = end_write_io.tv_sec - start_write_io.tv_sec + 1e-9 * (end_write_io.tv_nsec - start_write_io.tv_nsec);
        printf("IO write testing took %.9fs to run %d iterations\n", io_write_time, IO_PERFORMANCE_TEST_CYCLES);
    }

    free(newBuf);
    //#---------------------

    if (config_params.run_tests || config_params.measure_performance) {
        uint8_t *sobelReferenceVersion = calloc(bmpImage->pxArraySize, sizeof(uint8_t));
        if (sobelReferenceVersion == NULL) {
            fprintf(stderr, "Failed allocating memory for sobelReferenceVersion\n");
            freeImage(bmpImage);
            dealloc_config_params(&config_params);
            return 1;
        }
        printf("Comparing generated output image to output of reference algorithm\n");
        if (config_params.version < 3) {
            sobel(bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, sobelReferenceVersion);
        } else {
            sobel_graysc(bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, sobelReferenceVersion);
        }
        runTestSimilarity(newPixels, bmpImage->pxArraySize, sobelReferenceVersion,
                          bmpImage->pxArraySize, config_params.version > 2);
        free(sobelReferenceVersion);
    }

    free(newPixels);

    if (config_params.measure_performance) {
        double avg_exec_time = exec_time / config_params.measure_performance_cycles;
        double avg_io_read_time = io_read_time / IO_PERFORMANCE_TEST_CYCLES;
        double avg_io_write_time = io_write_time / IO_PERFORMANCE_TEST_CYCLES;
        printf("Sobel calculation from version %d took on average %.9fs\n", config_params.version,
               avg_exec_time);
        printf("Loading the image took on average %.9fs\n", avg_io_read_time);
        printf("Writing the image took on average %.9fs\n", avg_io_write_time);
        printf("Percentage io/calc = %f%% \n", ((avg_io_read_time + avg_io_write_time) / avg_exec_time) * 100);
    }

    freeImage(bmpImage);
    dealloc_config_params(&config_params);
    return 0;
}
