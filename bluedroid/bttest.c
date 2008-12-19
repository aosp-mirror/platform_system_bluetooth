/*
** Copyright 2008 The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/** Bluedroid testing  */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include <bluedroid/bluetooth.h>

static int do_enable() {
    int ret;
    ret = bt_enable();
    printf("= %d\n", ret);
    return ret;
}

static int do_disable() {
    int ret;
    ret = bt_disable();
    printf("= %d\n", ret);
    return ret;
}

static int do_is_enabled() {
    int ret;
    ret = bt_is_enabled();
    printf("= %d\n", ret);
    return ret;
}

struct {
    char *name;
    int (*ptr)();
} function_table[]  = {
    {"enable", do_enable},
    {"disable", do_disable},
    {"is_enabled", do_is_enabled},
    {NULL, NULL},
};

static void usage() {
    int i;

    printf("Usage:\n");
    for (i = 0; function_table[i].name; i++) {
        printf("\tbttest %s\n", function_table[i].name);
    }
}

int main(int argc, char **argv) {
    int i;

    if (argc != 2) {
        usage();
        return -1;
    }
    for (i = 0; function_table[i].name; i++) {
        if (!strcmp(argv[1], function_table[i].name)) {
            printf("%s\n", function_table[i].name);
            return (*function_table[i].ptr)();
        }
    }
    usage();
    return -1;
}
