//
// Created by Cato on 22.06.23.
//

#ifndef IMPLEMENTIERUNG_TEST_BASIC_SOBEL_H
#define IMPLEMENTIERUNG_TEST_BASIC_SOBEL_H

#include <stddef.h>
#include "../IOSystem/bmp_parser.h"

size_t loadPicture(char *path, uBMPImage *img);
int runTestsSobel(void);

#endif //IMPLEMENTIERUNG_TEST_BASIC_SOBEL_H
