#ifndef VCF_DIVIDER_H

#define VCF_DIVIDER_H

#define LINE_BUFFER_SIZE 500000000
#define SMALL_BUFFER_SIZE 1024
#define NAME_BUFFER_SIZE 1024

#include "crelib/crelib.h"

void vcfdivide(const char * InFileName, const char * SSampleFileName);

#endif
