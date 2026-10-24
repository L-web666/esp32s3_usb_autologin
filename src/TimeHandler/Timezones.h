//TimeZones.h

#pragma once

#include <stddef.h>

typedef struct {
    const char *name;
    const char *posix_str;
} tz_db_pair_t;

// Declare the tz_db_tzs array (defined in the .cpp file)
extern const tz_db_pair_t tz_db_tzs[];
extern const size_t tz_db_tzs_size;

// Function declarations
const char *tz_db_get_posix_str(const char *name);
