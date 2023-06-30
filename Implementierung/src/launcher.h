//
// Created by Cato on 27.06.23.
//

#ifndef IMPLEMENTIERUNG_LAUNCHER_H
#define IMPLEMENTIERUNG_LAUNCHER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct config_ {
    uint8_t version;
    bool measure_performance;
    uint32_t measure_performance_cycles;
    bool run_unit_tests;
    char *outputFilePath;
    char *inputFilePath;
} config;

#endif //IMPLEMENTIERUNG_LAUNCHER_H
