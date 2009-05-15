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
};

struct thread_args {
    int fd;
    int type;
    int delay;
};

struct sockaddr_un  local_addr_un  = {AF_UNIX, "/data/foo"};
struct sockaddr_rc  local_addr_rc  = {AF_BLUETOOTH, *BDADDR_ANY, 4};
struct sockaddr_sco local_addr_sco = {AF_BLUETOOTH, *BDADDR_LOCAL};
struct sockaddr_l2  local_addr_l2  = {AF_BLUETOOTH, htobs(0x1001), *BDADDR_ANY, 0};
struct sockaddr_in  local_addr_in  = {AF_INET, 9999, {0}, {0}};

struct sockaddr_un  remote_addr_un  ;
struct sockaddr_rc  remote_addr_rc  ;
struct sockaddr_sco remote_addr_sco ;
struct sockaddr_l2  remote_addr_l2  ;
struct sockaddr_in  remote_addr_in  ;

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
    }

    printf("%d: socket()\n", gettid());
    ret = socket(family, typ, protocol);
    printf("%d: socket() = %d\n", gettid(), ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _close(int fd, int type) {
    int ret;

    printf("%d: close(%d)\n", gettid(), fd);
    ret = close(fd);
    printf("%d: close(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _bind(int fd, int type) {
    int len = 0;
    int ret;
    struct sockaddr *addr = NULL;

    switch (type) {
    case UNIX:
        unlink(local_addr_un.sun_path);
        addr = (struct sockaddr *) &local_addr_un;
        len = sizeof(local_addr_un);
        break;
    case RFCOMM:
        addr = (struct sockaddr *) &local_addr_rc;
        len = sizeof(local_addr_rc);
        break;
    case SCO:
        addr = (struct sockaddr *) &local_addr_sco;
        len = sizeof(local_addr_sco);
        break;
    case L2CAP:
        addr = (struct sockaddr *) &local_addr_l2;
        len = sizeof(local_addr_l2);
        break;
    case TCP:
        addr = (struct sockaddr *) &local_addr_in;
        len = sizeof(local_addr_in);
        break;
    }

    printf("%d: bind(%d)\n", gettid(), fd);
    ret = bind(fd, addr, len);
    printf("%d: bind(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _listen(int fd, int type) {
    int ret;

    printf("%d: listen(%d)\n", gettid(), fd);
    ret = listen(fd, 1);
    printf("%d: listen(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _accept(int fd, int type) {
    int ret;
    int len;
    struct sockaddr *addr = NULL;

    switch (type) {
    case UNIX:
        addr = (struct sockaddr *) &remote_addr_un;
        len = sizeof(remote_addr_un);
        break;
    case RFCOMM:
        addr = (struct sockaddr *) &remote_addr_rc;
        len = sizeof(remote_addr_rc);
        break;
    case SCO:
        addr = (struct sockaddr *) &remote_addr_sco;
        len = sizeof(remote_addr_sco);
        break;
    case L2CAP:
        addr = (struct sockaddr *) &remote_addr_l2;
        len = sizeof(remote_addr_l2);
        break;
    case TCP:
        addr = (struct sockaddr *) &remote_addr_in;
        len = sizeof(remote_addr_in);
        break;
    }

    printf("%d: accept(%d)\n", gettid(), fd);
    ret = accept(fd, addr, &len);
    printf("%d: accept(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));
    else {
        printf("\tlen = %d\n", len);
    }

    return ret;
}

static int _connect(int fd, int type) {
    int ret;
    int len = 0;
    struct sockaddr *addr = NULL;

    switch (type) {
    case UNIX:
        addr = (struct sockaddr *) &local_addr_un;
        len = sizeof(local_addr_un);
        break;
    case RFCOMM:
        addr = (struct sockaddr *) &local_addr_rc;
        len = sizeof(local_addr_rc);
        break;
    case SCO:
        addr = (struct sockaddr *) &local_addr_sco;
        len = sizeof(local_addr_sco);
        break;
    case L2CAP:
        addr = (struct sockaddr *) &local_addr_l2;
        len = sizeof(local_addr_l2);
        break;
    case TCP:
        addr = (struct sockaddr *) &local_addr_in;
        len = sizeof(local_addr_in);
        break;
    }

    printf("%d: connect(%d)\n", gettid(), fd);
    ret = connect(fd, addr, len);
    printf("%d: connect(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _write(int fd, int type) {
    int ret;
    char buf = 0;

    printf("%d: write(%d)\n", gettid(), fd);
    ret = write(fd, &buf, 1);
    printf("%d: connect(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static void thread_delay_close(struct thread_args *args) {
    printf("%d: START\n", gettid());
    sleep(args->delay);
    _close(args->fd, args->type);
    printf("%d: END\n", gettid());
}

static void thread_delay_close_write(struct thread_args *args) {
    printf("%d: START\n", gettid());
    sleep(args->delay);
    _close(args->fd, args->type);
    sleep(args->delay);
    _write(args->fd, args->type);
    printf("%d: END\n", gettid());
}

static void thread_delay_connect(struct thread_args *args) {
    printf("%d: START\n", gettid());
    sleep(args->delay);
    args->fd = _socket(args->type);
    _connect(args->fd, args->type);
    printf("%d: END\n", gettid());
}

static int do_accept_accept_accept(int type) {
    int fd;

    fd = _socket(type);
    if (fd < 0) goto error;

    if (_bind(fd, type) < 0) goto error;

    if (_listen(fd, type) < 0) goto error;

    while (1) {
        _accept(fd, type);
    }

    return 0;

error:
    return -1;
}

static int do_accept_and_close(int type) {
    int fd;
    pthread_t thread;
    struct thread_args args = {-1, type, 1};

    fd = _socket(type);
    if (fd < 0) goto error;

    if (_bind(fd, type) < 0) goto error;

    if (_listen(fd, type) < 0) goto error;

    args.fd = fd;
    pthread_create(&thread, NULL, (void *)thread_delay_close, (void *)&args);

    _accept(fd, type);

    pthread_join(thread, NULL);

    return 0;

error:
    return -1;
}

// accept in one thread. close then write in another
static int do_accept_close_write(int type) {
    int fd;
    pthread_t thread;
    struct thread_args args = {-1, type, 1};

    fd = _socket(type);
    if (fd < 0) goto error;

    if (_bind(fd, type) < 0) goto error;

    if (_listen(fd, type) < 0) goto error;

    args.fd = fd;
    pthread_create(&thread, NULL, (void *)thread_delay_close_write, (void *)&args);

    _accept(fd, type);

    pthread_join(thread, NULL);

    return 0;

error:
    return -1;
}

static int do_poll_poll_shutdown(int type) {
#if 0
    int fd;
    struct thread_args a1 = {-1, type, 1};
    struct thread_args a2 = {-1, type, 2};

    fd = _socket(type);

    pthread_create(&t1, NULL, (void *)thread_poll, (void *)fd[0]);
    pthread_create(&t2, NULL, (void *)thread_poll, (void *)fd[0]);

    sleep(1);

    _shutdown(fd[1], SHUT_RDWR);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
#endif
    return 0;
}

// accept in one thread, connect from two different threads
static int do_accept_connect_connect(int type) {
    int fd;
    pthread_t t1;
    pthread_t t2;
    struct thread_args a1 = {-1, type, 1};
    struct thread_args a2 = {-1, type, 2};

    fd = _socket(type);
    if (fd < 0) goto error;

    if (_bind(fd, type) < 0) goto error;

    if (_listen(fd, type) < 0) goto error;

    pthread_create(&t1, NULL, (void *)thread_delay_connect, (void *)&a1);
    pthread_create(&t2, NULL, (void *)thread_delay_connect, (void *)&a2);

    _accept(fd, type);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;

error:
    return -1;
}

struct {
    char *name;
    int (*ptr)(int);
} action_table[]  = {
    {"accept_accept_accept", do_accept_accept_accept},
    {"accept_and_close", do_accept_and_close},
    {"accept_close_write", do_accept_close_write},
    {"accept_connect_connect", do_accept_connect_connect},
    {NULL, NULL},
};

struct {
    char *name;
    enum sock_type type;
} type_table[]  = {
    {"unix", UNIX},
    {"rfcomm", RFCOMM},
    {"sco", SCO},
    {"l2cap", L2CAP},
    {"tcp", TCP},
    {NULL, -1},
};

static void usage() {
    int i;

    printf("socktest TYPE ACTION\n");
    printf("\nTYPE:\n");
    for (i = 0; type_table[i].name; i++) {
        printf("\t%s\n", type_table[i].name);
    }
    printf("\nACTION:\n");
    for (i = 0; action_table[i].name; i++) {
        printf("\t%s\n", action_table[i].name);
    }
}

int main(int argc, char **argv) {
    int i;
    int type = -1;

    if (argc != 3) {
        usage();
        return -1;
    }
    for (i = 0; type_table[i].name; i++) {
        if (!strcmp(argv[1], type_table[i].name)) {
            type = type_table[i].type;
            break;
        }
    }
    if (type == -1) {
        usage();
        return -1;
    }
    for (i = 0; action_table[i].name; i++) {
        if (!strcmp(argv[2], action_table[i].name)) {
            printf("TYPE = %s ACTION = %s\n", type_table[type].name,
                    action_table[i].name);
            return (*action_table[i].ptr)(type);
        }
    }
    usage();
    return -1;
}
