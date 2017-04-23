/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "catch.h"

#define EXPECT(operation, exception) \
    { \
        bool check = false; \
        try \
        { \
            operation; \
        } \
        catch(exception) \
        { \
            check = true; \
        } \
        REQUIRE(check == true); \
    }

#define EXPECT_ANY(operation) EXPECT(operation, ...)
