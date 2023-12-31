#define _POSIX_C_SOURCE 199309L

#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <pthread_time.h>
#endif

#include "IOSystem/s_image.h"
#include "IOSystem/load_save_utils.h"
#include "Implementierung/sobel_basic.h"
#include "Implementierung/sobel_simd.h"
#include "Implementierung/sobel_threaded.h"
#include "IOSystem/test_IO_parser.h"
#include "Implementierung/test_sobel_basic.h"
#include "Implementierung/test_similarity.h"

#define BMP_FILE_EXT ".bmp"
#define OUT_FILE_SUFFIX "_out" BMP_FILE_EXT

// Number of pixel an image must have to cause a 1 sec read/write
// Set this accordingly to your specs for testing. These values fit our testsetup
#define PIXEL_NUM_1SEC_READ 1000000000
#define PIXEL_NUM_1SEC_WRITE 105000000

typedef struct config {
    uint8_t version;
    bool measure_performance;
    size_t measure_performance_cycles;
    bool run_tests;
    char *output_file_path;
    char *input_file_path;
} config;

static void print_help_msg(void) {
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

static void dealloc_config_params(config *config_params) {
    if (config_params->input_file_path != NULL) {
        free(config_params->input_file_path);
    }
    if (config_params->output_file_path != NULL) {
        free(config_params->output_file_path);
    }
}

static void print_arg_error(const char *errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    print_help_msg();
}

static int parse_args(int argc, char *argv[], config *config_params) {
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
                // check if version is 0, 1, 2, 3, 4 or 5
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
                dealloc_config_params(config_params);
                exit(0);
            case 'o':
                if (optarg[0] == '-') {
                    print_arg_error("Output path can't start with a dash");
                    return 1;
                }
                if (optarg[0] == '\0') {
                    print_arg_error("Output path can't be an empty string");
                    return 1;
                }
                if (config_params->output_file_path != NULL) {
                    free(config_params->output_file_path);
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

    if (argc == 0 || strlen(argv[0]) == 0) {
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

        size_t input_justname_len;
        if (strstr(search_string, BMP_FILE_EXT) != search_string + input_path_len - strlen(BMP_FILE_EXT) - 1) {
            input_justname_len = input_path_len;
        } else {
            input_justname_len = input_path_len - strlen(BMP_FILE_EXT);
        }
        config_params->output_file_path = malloc(input_justname_len + strlen(OUT_FILE_SUFFIX));
        if (config_params->output_file_path == NULL) {
            fprintf(stderr, "Failed allocating memory for output_file_path\n");
            return 1;
        }

        strncpy(config_params->output_file_path, config_params->input_file_path, input_justname_len - 1);
        config_params->output_file_path[input_justname_len - 1] = '\0';
        strncat(config_params->output_file_path, OUT_FILE_SUFFIX, strlen(OUT_FILE_SUFFIX)+1);
    }
    return 0;
}

static void (*const sobel_implementations[6])(const uint8_t *, size_t, size_t, uint8_t *) = {
        sobel, sobel_simd, sobel_threaded, sobel_graysc, sobel_simd_graysc, sobel_threaded_graysc
};

static double run_performance_io_load(size_t cycles, bool graysc, const char *input_file_path) {
    struct timespec t_start_io_load;
    struct timespec t_stop_io_load;

    printf("\n");
    printf("Running performance tests for BMP Input\n");
    if (clock_gettime(CLOCK_MONOTONIC, &t_start_io_load) != 0) {
        fprintf(stderr, "Couldn't get t_start_io_load time\n");
        return -1;
    }

    for (size_t i = 0; i < cycles; i++){
        s_image *test_bmp_image = load_image(input_file_path, graysc);
        if (test_bmp_image == NULL) {
            fprintf(stderr, "Failed to load image on cycle %lu in performance test", i);
            return -1;
        }
        free_image(test_bmp_image);
    }

    if(clock_gettime(CLOCK_MONOTONIC, &t_stop_io_load) != 0) {
        fprintf(stderr, "Couldn't get t_stop_io_load time\n");
        return -1;
    }
    double input_time = t_stop_io_load.tv_sec - t_start_io_load.tv_sec + 1e-9 * (t_stop_io_load.tv_nsec - t_start_io_load.tv_nsec);
    printf(" ❯ IO read performance testing took %.9fs to run %zu iterations\n", input_time, cycles);
    printf("\n");

    return input_time;
}

static double run_performance_sobel_calc(size_t cycles, uint8_t version, s_image *bmp_image, uint8_t *new_pixels) {
    struct timespec t_start_sobel_calc;
    struct timespec t_stop_sobel_calc;
    printf("\n");
    printf("Running performance tests for calculation\n");
    if (clock_gettime(CLOCK_MONOTONIC, &t_start_sobel_calc) != 0) {
        fprintf(stderr, "Couldn't get t_start_sobel_calc time\n");
        return -1;
    }

    for (size_t i = 0; i < cycles; ++i) {
        sobel_implementations[version](bmp_image->px_array, bmp_image->px_width, bmp_image->px_height, new_pixels);
    }

    if(clock_gettime(CLOCK_MONOTONIC, &t_stop_sobel_calc) != 0) {
        fprintf(stderr, "Couldn't get t_stop_sobel_calc time\n");
        return -1;
    }
    double calculation_time = t_stop_sobel_calc.tv_sec - t_start_sobel_calc.tv_sec + 1e-9 * (t_stop_sobel_calc.tv_nsec - t_start_sobel_calc.tv_nsec);
    printf(" ❯ Sobel calc performance testing took %.9fs to run %ld iterations\n", calculation_time, cycles);
    printf("\n");

    return calculation_time;
}

static double run_performance_io_save(size_t cycles, bool graysc, s_image *export_image, const char* output_file_path) {
    struct timespec t_start_io_save;
    struct timespec t_stop_io_save;
    printf("\n");
    printf("Running performance tests for BMP output\n");
    if (clock_gettime(CLOCK_MONOTONIC, &t_start_io_save) != 0) {
        fprintf(stderr, "Couldn't get t_start_io_save time\n");
        return -1;
    }

    for (size_t i = 0; i < cycles; i++){
        if (save_image(export_image, graysc, output_file_path)) {
            fprintf(stderr, "Failed to save image in cycle %lu in performance test", i);
            return -1;
        }
    }

    if(clock_gettime(CLOCK_MONOTONIC, &t_stop_io_save) != 0) {
        fprintf(stderr, "Couldn't get t_stop_io_save time\n");
        return -1;
    }
    double output_time = t_stop_io_save.tv_sec - t_start_io_save.tv_sec + 1e-9 * (t_stop_io_save.tv_nsec - t_start_io_save.tv_nsec);
    printf(" ❯ IO write testing took %.9fs to run %zu iterations\n", output_time, cycles);
    printf("\n");

    return output_time;
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

    printf("Loading image from %s\n", config_params.input_file_path);
    s_image *bmp_image = load_image(config_params.input_file_path, config_params.version >= 3);
    if (bmp_image == NULL) {
        dealloc_config_params(&config_params);
        return 1;
    }

    double input_time = 0;
    size_t read_cycles = (PIXEL_NUM_1SEC_READ / (bmp_image->px_height * bmp_image->px_width));
    read_cycles = read_cycles > 0 ? read_cycles : 1;

    if (config_params.measure_performance) {
        input_time = run_performance_io_load(read_cycles, config_params.version >= 3, config_params.input_file_path);
        if (input_time < 0) {
            fprintf(stderr, "Failed io_load performance test\n");
        }
    }

    uint8_t *new_pixels = calloc(bmp_image->px_array_size, sizeof(uint8_t));
    if (new_pixels == NULL) {
        fprintf(stderr, "Couldn't allocate memory for new_pixels\n");
        free_image(bmp_image);
        dealloc_config_params(&config_params);
        return 1;
    }

    printf("Calculating sobel for image using version %d\n", config_params.version);
    sobel_implementations[config_params.version](bmp_image->px_array, bmp_image->px_width, bmp_image->px_height, new_pixels);
    
    double calculation_time = 0;
    if (config_params.measure_performance) {
        calculation_time = run_performance_sobel_calc(config_params.measure_performance_cycles, config_params.version, bmp_image, new_pixels);
        if (calculation_time < 0) {
            fprintf(stderr, "Failed sobel_calc performance test\n");
        }
    }

    s_image export_image = {.px_array = new_pixels, .px_array_size = bmp_image->px_array_size, .px_height = bmp_image->px_height, .px_width = bmp_image->px_width};
    printf("Saving image to %s\n", config_params.output_file_path);
    save_image(&export_image, config_params.version >= 3, config_params.output_file_path);
    
    double output_time = 0;
    size_t write_cycles = (PIXEL_NUM_1SEC_WRITE / (bmp_image->px_height * bmp_image->px_width));
    write_cycles = write_cycles > 0 ? write_cycles : 1;
    if (config_params.measure_performance) {
        output_time = run_performance_io_save(write_cycles, config_params.version >= 3, &export_image, config_params.output_file_path);
        if (output_time < 0) {
            fprintf(stderr, "Failed io_save performance test\n");
        }
    }

    if (config_params.run_tests || config_params.measure_performance) {
        uint8_t *sobel_reference_version = calloc(bmp_image->px_array_size, sizeof(uint8_t));
        if (sobel_reference_version == NULL) {
            fprintf(stderr, "Failed allocating memory for sobel_reference_version\n");
            free_image(bmp_image);
            free(new_pixels);
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

    free_image(bmp_image);
    free(new_pixels);

    if (config_params.measure_performance) {
        double avg_exec_time = calculation_time / config_params.measure_performance_cycles;
        double avg_io_read_time = input_time / read_cycles;
        double avg_io_write_time = output_time / write_cycles;
        printf("Sobel calculation from version %d took on average %.9fs\n", config_params.version,
               avg_exec_time);
        printf("Loading the image took on average %.9fs\n", avg_io_read_time);
        printf("Saving the image took on average %.9fs\n", avg_io_write_time);
        printf("Percentage io/calc = %f%% \n", ((avg_io_read_time + avg_io_write_time) / avg_exec_time) * 100);
    }
    dealloc_config_params(&config_params);
    return 0;
}
