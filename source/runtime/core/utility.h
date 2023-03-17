#pragma once

#include <assert.h>

#define ASSERT_SUCCEEDED( hr, ... ) \
        if (FAILED(hr)) { \
            assert(false); \
        }

#define ASSERT(condition, ...) assert(condition)
