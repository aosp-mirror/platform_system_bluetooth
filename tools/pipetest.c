/*
 * Copyright 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Helper to test linux pipe's */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/socket.h>

static void print_events(int events) {
    if (events & POLLIN) printf("POLLIN ");
    if (events & POLLPRI) printf("POLLPRI ");
    if (events & POLLOUT) printf("POLLOUT ");
    if (events & POLLERR) printf("POLLERR ");
    if (events & POLLHUP) printf("POLLHUP ");
    if (events & POLLNVAL) printf("POLLNVAL ");
    printf("\n");
}

static int _socketpair(int fd[2]) {
    int ret;
    printf("%d: socketpair()\n", gettid());
    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    printf("%d: socketpair() = %d\n", gettid(), ret);
    if (ret) printf("\terr %d (%s)\n", errno, strerror(errno));
    return ret;
}   

static int _close(int fd) {
    int ret;
    printf("%d: close(%d)\n", gettid(), fd);
    ret = close(fd);
    printf("%d: close(%d) = %d\n", gettid(), fd, ret);
    if (ret) printf("\terr %d (%s)\n", errno, strerror(errno));
    return ret;
}

static int _poll(struct pollfd *ufds, nfds_t nfds, int timeout) {
    int ret;
    unsigned int i;
    printf("%d: poll()\n", gettid());
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

static int _write(int fd, char *buf, int len) {
    int ret;

    printf("%d: write(%d)\n", gettid(), fd);
    ret = write(fd, buf, len);
    printf("%d: write(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _read(int fd) {
    int ret;
    char buf;

    printf("%d: read(%d)\n", gettid(), fd);
    ret = read(fd, &buf, 1);
    printf("%d: read(%d) = %d [%d]\n", gettid(), fd, ret, (int)buf);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}

static int _shutdown(int fd, int how) {
    int ret;

    printf("%d: shutdown(%d)\n", gettid(), fd);
    ret = shutdown(fd, how);
    printf("%d: shutdown(%d) = %d\n", gettid(), fd, ret);
    if (ret < 0) printf("\terr %d (%s)\n", errno, strerror(errno));

    return ret;
}
static void thread_poll(void *args) {
    int fd = (int)args;
    struct pollfd pfd;
    printf("%d: START\n", gettid());
    pfd.fd = fd;
    pfd.events = 0;
    _poll(&pfd, 1, -1);
    printf("%d: END\n", gettid());
}

static void thread_pollin(void *args) {
    int fd = (int)args;
    struct pollfd pfd;
    printf("%d: START\n", gettid());
    pfd.fd = fd;
    pfd.events = POLLIN;
    _poll(&pfd, 1, -1);
    printf("%d: END\n", gettid());
}

static void thread_pollin_rand_delay(void *args) {
    int fd = (int)args;
    struct pollfd pfd;
    int delay = (int)((double)random() * (10000000.0 / 2147483647.0));
    printf("%d: START (delay = %d)\n", gettid(), delay);
    pfd.fd = fd;
    pfd.events = POLLIN;
    usleep(delay);
    _poll(&pfd, 1, -1);
    printf("%d: END\n", gettid());
}

static void thread_read(void *args) {
    int fd = (int)args;
    printf("%d: START\n", gettid());
    _read(fd);
    printf("%d: END\n", gettid());
}

static void thread_close(void *args) {
    int fd = (int)args;
    printf("%d: START\n", gettid());
    _close(fd);
    printf("%d: END\n", gettid());
}

static int do_poll_poll_close() {
    pthread_t t1;
    pthread_t t2;
    int fd[2];

    if (pipe(fd)) return -1;

    pthread_create(&t1, NULL, (void *)thread_poll, NULL);
    pthread_create(&t2, NULL, (void *)thread_poll, NULL);

    sleep(1);

    _close(fd[1]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

static int do_socketpair_poll1_shutdown2() {
    int fd[2];
    pthread_t t;

    if (_socketpair(fd)) return -1;

    pthread_create(&t, NULL, (void *)thread_poll, (void *)fd[1]);

    sleep(1);

    _shutdown(fd[0], SHUT_RDWR);    

    sleep(1);

    _close(fd[0]);

    pthread_join(t, NULL);

    return 0;
}

static int do_socketpair_poll1_shutdown1() {
    int fd[2];
    pthread_t t;

    if (_socketpair(fd)) return -1;

    pthread_create(&t, NULL, (void *)thread_poll, (void *)fd[0]);

    sleep(1);

    _shutdown(fd[0], SHUT_RDWR);    

    sleep(1);

    _close(fd[0]);

    pthread_join(t, NULL);

    return 0;
}

static int do_socketpair_poll1_close1() {
    int fd[2];
    pthread_t t;

    if (_socketpair(fd)) return -1;

    pthread_create(&t, NULL, (void *)thread_poll, (void *)fd[0]);

    sleep(1);

    _close(fd[0]);

    pthread_join(t, NULL);

    return 0;
}

static int do_socketpair_read1_shutdown1() {
    int fd[2];
    pthread_t t;

    if (_socketpair(fd)) return -1;

    pthread_create(&t, NULL, (void *)thread_read, (void *)fd[0]);

    sleep(1);

    _shutdown(fd[0], SHUT_RDWR);    

    sleep(1);

    _close(fd[0]);

    pthread_join(t, NULL);

    return 0;
}

static int do_pipe_pipe_pipe() {
    int fd[2];
    int i;

    while (1) {
        if (pipe(fd)) {
            printf("pipe: %s\n", strerror(errno));
            return -1;
        }
        printf("%d %d\n", fd[0], fd[1]);
        close(fd[0]);
        close(fd[1]);
    }

    return 0;
}
static int do_pollin_pollin_write() {
    pthread_t t1;
    pthread_t t2;
    int fd[2];
    char buf = 'a';
    int i;

    if (pipe(fd)) return -1;

    pthread_create(&t1, NULL, (void *)thread_pollin, (void *)fd[0]);
    pthread_create(&t2, NULL, (void *)thread_pollin, (void *)fd[0]);

    sleep(1);

    for (i = 0; i < 100; i++)
        _write(fd[1], &buf, 1);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

static int do_pollin_pollin_pollin_write_pollin_pollin_pollin() {
    const int MAX_T = 10;
    pthread_t t[MAX_T];
    int fd[2];
    char buf = 'a';
    int i;

    if (pipe(fd)) return -1;

    for (i=0; i<MAX_T; i++)
        pthread_create(&t[i], NULL, (void *)thread_pollin_rand_delay, (void *)fd[0]);

    sleep(5);

    _write(fd[1], &buf, 1);

    for (i=0; i<MAX_T; i++)
        pthread_join(t[i], NULL);

    _close(fd[0]);
    _close(fd[1]);

    return 0;
}

static int do_poll_poll_shutdown() {
#if 0
    pthread_t t1;
    pthread_t t2;
    int fd[2];

    if (pipe(fd)) return -1;

    pthread_create(&t1, NULL, (void *)thread_poll, (void *)fd[0]);
    pthread_create(&t2, NULL, (void *)thread_poll, (void *)fd[0]);

    sleep(1);

    _shutdown(fd[1], SHUT_RDWR);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
#endif

    return -1;
}

static int THREADS = 100;

static int do_close_poll_poll_poll() {
    pthread_t t[THREADS];
    int i;
    int fd[2];

    if (pipe(fd)) return -1;

    _close(fd[1]);

    for (i = 0; i < THREADS; i++)
        pthread_create(&t[i], NULL, (void *)thread_poll, (void *)fd[0]);

    for (i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    return 0;
}

static int do_close_close_close() {
    pthread_t t[THREADS];
    int i;
    int fd[2];

    if (pipe(fd)) return -1;

    for (i = 0; i < THREADS; i++)
        pthread_create(&t[i], NULL, (void *)thread_close, (void *)fd[i%2]);

    return 0;
}

static int pipe_close_w_close_r_repeat() {
    int fd[2];
    pthread_t t;
    int i;

    for (i = 0; i < THREADS; i++) {
        if (pipe(fd)) return -1;
        pthread_create(&t, NULL, (void *)thread_poll, (void *)fd[0]);
        _close(fd[1]);
        _close(fd[0]);
        pthread_join(t, NULL);
    }

    return 0;
}

struct {
    char *name;
    int (*ptr)();
} function_table[]  = {
    {"socketpair_poll1_shutdown2", do_socketpair_poll1_shutdown2},
    {"socketpair_poll1_shutdown1", do_socketpair_poll1_shutdown1},
    {"socketpair_poll1_close1", do_socketpair_poll1_close1},
    {"socketpair_read1_shutdown1", do_socketpair_read1_shutdown1},
    {"pipe_pipe_pipe", do_pipe_pipe_pipe},
    {"poll_poll_close", do_poll_poll_close},
    {"pollin_pollin_write", do_pollin_pollin_write},
    {"pollin_pollin_pollin_write_pollin_pollin_pollin", do_pollin_pollin_pollin_write_pollin_pollin_pollin},
    {"poll_poll_shutdown", do_poll_poll_shutdown},
    {"close_poll_poll_poll", do_close_poll_poll_poll},
    {"close_close_close", do_close_close_close},
    {"pipe_close_w_close_w_repeat", pipe_close_w_close_r_repeat},
    {NULL, NULL},
};

static void usage() {
    int i;

    printf("Usage:\n");
    for (i = 0; function_table[i].name; i++) {
        printf("\tpipetest %s\n", function_table[i].name);
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
