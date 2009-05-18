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

/** socket testing  */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/l2cap.h>

enum sock_type {
    UNIX = 0,
    RFCOMM,
    SCO,
    L2CAP,
    TCP,
    SOCKETPAIR,
};

static void print_events(int events) {
    if (events & POLLIN) printf("POLLIN ");
    if (events & POLLPRI) printf("POLLPRI ");
    if (events & POLLOUT) printf("POLLOUT ");
    if (events & POLLERR) printf("POLLERR ");
    if (events & POLLHUP) printf("POLLHUP ");
    if (events & POLLNVAL) printf("POLLNVAL ");
    printf("\n");
}

static void print_fds(struct pollfd *ufds, nfds_t nfds) {
    unsigned int i;
    for (i=0; i<nfds; i++)
        printf("%d ", ufds[i].fd);
}

static int _socket(int type) {
    int ret;
    int family = -1;
    int typ = -1;
    int protocol = -1;

    switch (type) {
    case UNIX:
        family = PF_UNIX;
        typ = SOCK_STREAM;
        protocol = 0;
        break;
    case RFCOMM:
        family = PF_BLUETOOTH;
        typ = SOCK_STREAM;
        protocol = BTPROTO_RFCOMM;
        break;
    case SCO:
        family = PF_BLUETOOTH;
        typ = SOCK_SEQPACKET;
        protocol = BTPROTO_SCO;
        break;
    case L2CAP:
        family = PF_BLUETOOTH;
        typ = SOCK_SEQPACKET;
        protocol = BTPROTO_L2CAP;
        break;
    case TCP:
        family = PF_INET;
        typ = SOCK_STREAM;
        protocol = 0;
        break;
    case SOCKETPAIR:
        {
            int fd[2];
            printf("%d: socketpair()\n", gettid());
            ret = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
            printf("%d: socketpair() = %d\n", gettid(), ret);
            if (ret) {
                printf("\terr %d (%s)\n", errno, strerror(errno));
                return -1;
            }
            return fd[0];
        }
    }

    printf("%d: socket()\n", gettid());
    ret = socket(family, typ, protocol);
    printf("%d: socket() = %d\n", gettid(), ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _close(int fd) {
    int ret;

    printf("%d: close(%d)\n", gettid(), fd);
    ret = close(fd);
    printf("%d: close(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _shutdown(int fd) {
    int ret;

    printf("%d: shutdown(%d)\n", gettid(), fd);
    ret = shutdown(fd, SHUT_RDWR);
    printf("%d: shutdown(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _poll(struct pollfd *ufds, nfds_t nfds, int timeout) {
    int ret;
    unsigned int i;

    printf("%d: poll(", gettid());
    print_fds(ufds, nfds);
    printf(")\n");
    ret = poll(ufds, nfds, timeout);
    printf("%d: poll() = %d\n", gettid(), ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));
    if (ret > 0) {
        for (i=0; i<nfds; i++) {
            if (ufds[i].revents) {
                printf("\tfd %d ", ufds[i].fd); print_events(ufds[i].revents);
            }
        }
    }
    return ret;
}

static void thread_poll(int fd) {
    struct pollfd pfd;
    printf("%d: START\n", gettid());
    pfd.fd = fd;
    pfd.events = 0;
    _poll(&pfd, 1, -1);
    printf("%d: END\n", gettid());
}

static int do_poll_shutdown(int type) {
    const int MAX_T = 2;
    int fd;
    pthread_t t[MAX_T];
    int i;

    fd = _socket(type);

    for (i=0; i<MAX_T; i++)
        pthread_create(&t[i], NULL, (void *)thread_poll, (void *)fd);

    sleep(1);

    _shutdown(fd);

    for (i=0; i<MAX_T; i++)
        pthread_join(t[i], NULL);

    _close(fd);

    return 0;
}

struct {
    char *name;
    enum sock_type type;
} type_table[]  = {
    {"unix", UNIX},
    {"rfcomm", RFCOMM},
    {"sco", SCO},
    {"l2cap", L2CAP},
    {"tcp", TCP},
    {"socketpair", SOCKETPAIR},
    {NULL, -1},
};

static void usage() {
    int i;

    printf("sock_shutdown_test TYPE\n");
    printf("\nTYPE:\n");
    for (i = 0; type_table[i].name; i++) {
        printf("\t%s\n", type_table[i].name);
    }
}

int main(int argc, char **argv) {
    int i;
    int type = -1;

    if (argc != 2) {
        usage();
        return -1;
    }
    for (i = 0; type_table[i].name; i++) {
        if (!strcmp(argv[1], type_table[i].name)) {
            type = type_table[i].type;
            printf("TYPE = %s\n", type_table[type].name);
            return do_poll_shutdown(type);
        }
    }
    usage();
    return -1;
}
