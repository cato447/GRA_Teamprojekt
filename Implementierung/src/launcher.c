#define _POSIX_C_SOURCE 199309L

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <pthread_time.h>
#endif

#include "IOSystem/bmp_definitions.h"
#include "IOSystem/IO_tools.h"
#include "IOSystem/bmp_parser.h"
#include "IOSystem/bmp_loader.h"
#include "Implementierung/sobel_basic.h"
#include "Implementierung/sobel_simd.h"
#include "Implementierung/sobel_threaded.h"
#include "Implementierung/test_sobel_basic.h"
#include "Implementierung/test_similarity.h"
#include "IOSystem/test_IO_parser.h"

#define BMP_FILE_EXT ".bmp"
#define OUT_FILE_SUFFIX "_out" BMP_FILE_EXT
#define IO_PERFORMANCE_TEST_CYCLES 100

typedef struct config {
    uint8_t version;
    bool measure_performance;
    unsigned long measure_performance_cycles;
    bool run_tests;
    char *output_file_path;
    char *input_file_path;
} config;

void print_help_msg(void) {
    printf("Program to calculate sobel from BMP file\n");
    printf("Usage: ./program [arguments] input_file_path\n");
    printf("arguments:\n");
    printf("\t-V <version_number>   Which implementation to use (0: reference, 1: SIMD, 2: SIMD+Threads\n");
    printf("\t                                                   3: grayscale reference, 4: grayscale SIMD\n");
    printf("\t                                                   5: grayscale SIMD+Threads)\n");
    printf("\t-o <output_file_path> Output path to write the resulting image to\n");
    printf("\t-B [cycles]           Run performance tests 3 or if provided [cycles] (at least 3) times\n");
    printf("\t-t                    Run all unit tests\n");
    printf("\t-h | --help           Display this message\n");
}

void dealloc_config_params(config *config_params) {
    if (config_params != NULL) {
        if (config_params->input_file_path != NULL) {
            free(config_params->input_file_path);
        }
        if (config_params->output_file_path != NULL) {
            free(config_params->output_file_path);
        }
    }
}

void free_image(s_image *img) {
    if (img != NULL) {
        if (img->px_array != NULL) {
            free(img->px_array);
        }
        free(img);
    }
}


void print_arg_error(const char *errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    print_help_msg();
}

int parse_args(int argc, char *argv[], config *config_params) {
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
                // check if version is 0, 1 or 2
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
                size_t output_path_len = strlen(optarg) + 1;
                config_params->output_file_path = malloc(output_path_len);
                if (config_params->output_file_path == NULL) {
                    fprintf(stderr, "Failed allocating memory for output_file_path\n");
                    return 1;
                }
                strncpy(config_params->output_file_path, optarg, output_path_len);
                break;
            case '?':
                print_arg_error("Unknown argument");
                return 1;
            case ':':
                switch (optopt) {
                    case 'V':
                        print_arg_error("Missing version number -V <version_num>");
                        return 1;
                    case 'o':
                        print_arg_error("No output path for generated file was given -o <output_file_path>");
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
    config_params->input_file_path = malloc(input_path_len);
    if (config_params->input_file_path == NULL) {
        fprintf(stderr, "Failed allocating memory for input_file_path\n");
        return 1;
    }

    strncpy(config_params->input_file_path, argv[0], input_path_len);

    //Set output_file_path if not given
    if (config_params->output_file_path == NULL) {
        char search_string[input_path_len];
        strncpy(search_string, config_params->input_file_path, input_path_len);
        for (char *c = search_string; *c; c++) {
            *c = tolower(*c);
        }

        size_t len_input_name;
        if (strstr(search_string, BMP_FILE_EXT) !=
            search_string + input_path_len - strlen(BMP_FILE_EXT) - 1) {
            len_input_name = input_path_len - 1;
        } else {
            len_input_name = input_path_len - strlen(BMP_FILE_EXT) - 1;
        }
        config_params->output_file_path = malloc(len_input_name + strlen(OUT_FILE_SUFFIX) + 1);
        if (config_params->output_file_path == NULL) {
            fprintf(stderr, "Failed allocating memory for output_file_path\n");
            return 1;
        }

        strncpy(config_params->output_file_path, config_params->input_file_path, len_input_name);
        strncat(config_params->output_file_path, OUT_FILE_SUFFIX,
                strlen(OUT_FILE_SUFFIX) + 1);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    config config_params = {
        .version = 0,
        .measure_performance = false,
        .measure_performance_cycles = 3,
        .run_tests = false,
        .output_file_path = NULL,
        .input_file_path = NULL
    };
    if (parse_args(argc, argv, &config_params)) {
        dealloc_config_params(&config_params);
        return 1;
    }

    if (config_params.run_tests) {
        run_tests_IO_parser();
        run_tests_sobel();
    }

    s_image *bmp_image = malloc(sizeof(s_image));
    if (bmp_image == NULL) {
        fprintf(stderr, "Failed allocating memory for bmp_image\n");
        dealloc_config_params(&config_params);
        return 1;
    }
    printf("Loading image from %s\n", config_params.input_file_path);

    size_t img_size;
    if (config_params.version < 3) {
        img_size = load_picture(config_params.input_file_path, bmp_image);
    } else {
        img_size = load_picture_graysc(config_params.input_file_path, bmp_image);
    }

    if (img_size == 0) {
        fprintf(stderr, "Couldn't load picture from input file %s\n", config_params.input_file_path);
        free(bmp_image);
        dealloc_config_params(&config_params);
        return 1;
    }

    struct timespec start_read_io;
    struct timespec end_read_io;
    double io_read_time = 0;
    if (config_params.measure_performance) {
        printf("Running performance tests for Image Read\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_read_io) != 0) {
            fprintf(stderr, "Couldn't get start_read_io time\n");
            free_image(bmp_image);
            dealloc_config_params(&config_params);
            return 1;
        }
        if (config_params.version < 3){
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                // This code is nearly the same as the one in load_picture
                // We had to copy it here to not mess up our memory
                void *buffer;
                size_t buffer_size;
                s_image *test_bmp_image = malloc(sizeof(s_image));
                //fprintf(stdout, "Loading BMP File: %s\n", path);
                buffer = read_file(config_params.input_file_path, &buffer_size);
                if (buffer == NULL) {
                    fprintf(stderr, "Couldn't read BMP File\n");
                    free(bmp_image);
                    return 1;
                }
                if (bmp_to_array(buffer, buffer_size, test_bmp_image) == 1) {
                    fprintf(stderr, "Couldn't parse BMP file\n");
                    free(buffer);
                    free(bmp_image);
                    return 1;
                }
                free(buffer);
                free_image(test_bmp_image);
            }
        } else {
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                // This code is nearly the same as the one in load_picture
                // We had to copy it here to not mess up our memory
                void *buffer;
                size_t buffer_size;
                s_image *test_bmp_image = malloc(sizeof(s_image));
                //fprintf(stdout, "Loading BMP File: %s\n", path);
                buffer = read_file(config_params.input_file_path, &buffer_size);
                if (buffer == NULL) {
                    fprintf(stderr, "Couldn't read BMP File\n");
                    free(bmp_image);
                    return 1;
                }
                if (bmp_to_array_graysc(buffer, buffer_size, test_bmp_image) == 1) {
                    fprintf(stderr, "Couldn't parse BMP file\n");
                    free(buffer);
                    free(bmp_image);
                    return 1;
                }
                free(buffer);
                free_image(test_bmp_image);
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

    uint8_t *new_pixels = calloc(bmp_image->px_array_size, sizeof(uint8_t));
    if (new_pixels == NULL) {
        fprintf(stderr, "Couldn't allocate memory for new_pixels\n");
        free_image(bmp_image);
        dealloc_config_params(&config_params);
        return 1;
    }

    struct timespec start_exec;
    struct timespec end_exec;
    double exec_time = 0;
    unsigned long num_of_execute_cycles = config_params.measure_performance ? config_params.measure_performance_cycles : 1;
    if (config_params.measure_performance) {
        printf("Running performance tests for Calculation\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_exec) != 0) {
            fprintf(stderr, "Couldn't get start_exec time\n");
            return 1;
        }
    }
    switch (config_params.version) {
        case 0:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                                                     new_pixels);
            }
            break;
        case 1:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_simd(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                                                     new_pixels);
            }
            break;
        case 2:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_thread(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                                                     new_pixels);
            }
            break;
        case 3:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_graysc(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                             new_pixels);
            }
            break;
        case 4:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_simd_graysc(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                             new_pixels);
            }
            break;
        case 5:
            for (unsigned long i = 0; i < num_of_execute_cycles; ++i) {
                sobel_thread_graysc(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height,
                             new_pixels);
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

    printf("Calculated sobel for image %s using version %d\n", config_params.input_file_path, config_params.version);

    s_image export_image = {.px_array = new_pixels, .px_array_size = bmp_image->px_array_size, .px_height = bmp_image->px_height, .px_width = bmp_image->px_width};
    size_t new_size;
    char *new_buf;
    if (config_params.version < 3) {
        new_buf = array_to_bmp(&export_image, &new_size);
    } else {
        new_buf = array_to_bmp_graysc(&export_image, &new_size);
    }

    printf("Writing to file %s\n", config_params.output_file_path);
    if (write_file(config_params.output_file_path, new_buf, new_size)) {
        free_image(bmp_image);
        dealloc_config_params(&config_params);
        free(new_buf);
        return 1;
    }

    struct timespec start_write_io;
    struct timespec end_write_io;
    double io_write_time = 0;
    if (config_params.measure_performance) {
        char *test_buf;
        printf("Running performance tests for image write\n");
        if (clock_gettime(CLOCK_MONOTONIC, &start_write_io) != 0) {
            fprintf(stderr, "Couldn't get start_write_io time\n");
            dealloc_config_params(&config_params);
            return 1;
        }
        if (config_params.version < 3){
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                test_buf = array_to_bmp(&export_image, &new_size);
                write_file(config_params.output_file_path, new_buf, new_size);
                free(test_buf);
            }
        } else {
            for (size_t i = 0; i < IO_PERFORMANCE_TEST_CYCLES; i++){
                test_buf = array_to_bmp_graysc(&export_image, &new_size);
                write_file(config_params.output_file_path, new_buf, new_size);
                free(test_buf);
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

    free(new_buf);

    if (config_params.run_tests || config_params.measure_performance) {
        uint8_t *sobel_reference_version = calloc(bmp_image->px_array_size, sizeof(uint8_t));
        if (sobel_reference_version == NULL) {
            fprintf(stderr, "Failed allocating memory for sobel_reference_version\n");
            free_image(bmp_image);
            dealloc_config_params(&config_params);
            return 1;
        }
        printf("Comparing generated output image to output of reference algorithm\n");
        if (config_params.version < 3) {
            sobel(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height, sobel_reference_version);
        } else {
            sobel_graysc(bmp_image->px_array, bmp_image->px_width, bmp_image->px_height, sobel_reference_version);
        }
        run_test_similarity(new_pixels, bmp_image->px_array_size, sobel_reference_version,
                          bmp_image->px_array_size, config_params.version > 2);
        free(sobel_reference_version);
    }

    free(new_pixels);

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
    free_image(bmp_image);
    dealloc_config_params(&config_params);
    return 0;
}
