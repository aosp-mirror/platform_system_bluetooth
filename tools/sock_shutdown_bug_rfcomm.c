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

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv) {
    int fd;
    int ret;
    long flags;
    struct sockaddr_rc addr;

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = 19;
    str2ba("00:17:E8:2C:A8:00", &addr.rc_bdaddr);

    fd = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printf("%d errno %d %s\n", __LINE__, errno, strerror(errno));
    }

    sleep(2);
    shutdown(fd, SHUT_RDWR);


    fd = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printf("%d errno %d %s\n", __LINE__, errno, strerror(errno));
    }
    
    sleep(2);

    shutdown(fd, SHUT_RDWR);

    sleep(2);
    return 0;
}
