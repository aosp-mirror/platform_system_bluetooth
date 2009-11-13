/*
** Copyright 2009 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    int fd;
    int ret;
    long flags;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = 12348;

    fd = socket(PF_INET, SOCK_STREAM, 0);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printf("%d errno %d %s\n", __LINE__, errno, strerror(errno));
    }
    
    ret = listen(fd, 1);
    if (ret < 0) {
        printf("%d errno %d %s\n", __LINE__, errno, strerror(errno));
    }

    sleep(2);

    close(fd);

    sleep(2);

    fd = socket(PF_INET, SOCK_STREAM, 0);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printf("%d errno %d %s\n", __LINE__, errno, strerror(errno));
    }
    
    sleep(2000000000);

    return 0;
}

