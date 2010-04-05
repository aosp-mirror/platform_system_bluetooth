/*
** Copyright 2007, The Android Open Source Project
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define FAILIF(x, args...) do {          \
    if (x) {                             \
        fprintf(stderr, ##args);         \
        exit(1);                         \
    }                                    \
} while(0)

static void usage() {
    printf("Usage: brfpatch INPUT OUTPUT\n"
           "\n"
           "\tGenerates bluetooth firmware\n"
           "\n"
           "INPUT: Bluetooth script in ASCII format.\n"
           "       For TI BRF chips this can be generated from .bts files using the TI Bluetooth\n"
           "       script pad to save as .txt. This txt file can be used as input.\n"
           "       Alternately, run strings on the .bts and manually edit to change decimal\n"
           "       arguments into hex of the appropriate number of octets.\n"
           "       FORMAT: Send_HCI_xxxx OPCODE DATA1 DATA2 DATA3 ...\n"
           "       where OPCODE, DATA1 etc are one of:\n"
           "       0x12          (1 byte)\n"
           "       0x1234        (2 byte)\n"
           "       0x12345678    (4 byte)\n"
           "       \"0123456789ABCDEF0123\"            (multibyte)\n"
           "       \"01:23:45:67:89:AB:CD:EF:01:23\"   (multibyte)\n"
           "\n"
           "OUTPUT: Binary firmware\n"
           "        FORMAT: 0x01 OPCODE DATA_LEN DATA\n");
    exit(1);
}

static void dump_record(FILE *fpo, unsigned short opcode, unsigned char len,
                        unsigned char *data) {

    unsigned char prefix = 0x01;  // H4 UART command packet
    fwrite(&prefix, 1, 1, fpo);
    fwrite(&opcode, 2, 1, fpo);  // opcode
    fwrite(&len, 1, 1, fpo);     // data length
    fwrite(data, len, 1, fpo);   // data
}

// advance beyond next whitespace. Return -1 if end of string reached
static int advance(char **buf) {
    char *b = *buf;
    while (*b && !isspace(*b))
        b++;
    while (*b && isspace(*b))
        b++;
    *buf = b;
    if (!(*b))
        return -1;
    return 0;
}

static void process_line(FILE *file_out, char *buf, char *buffer) {
    FAILIF(strncmp(buf, "Send_", 5) != 0, "Not expecting: %s\n", buffer);


    unsigned int opcode;

    FAILIF(advance(&buf), "Could not find opcode in: %s\n", buffer);
    FAILIF(sscanf(buf, "0x%04x\n", &opcode) != 1,
           "Could not find opcode in: %s\n", buffer);


    unsigned char data[1024];
    unsigned char *dp = data;

    while (!advance(&buf)) {
        switch (*buf) {
        case '"':
            buf++;
            while (*buf != '"' && *buf != 0) {
                FAILIF(dp > data + sizeof(data),
                       "Too much data: %s\n", buffer);
                FAILIF(sscanf(buf, "%02x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                dp++;
                buf += 2;
                if (*buf == ':')
                    buf++;
            }
            break;
        case '0':
            buf++;
            FAILIF(*buf != 'x', "Error parsing: %s\n", buffer);
            buf++;

            // find length of this piece of data
            char *end = buf;
            while (isalnum(*end))
                end++;

            // switch on length
            switch ((unsigned int)end - (unsigned int)buf) {
            case 2:
                FAILIF(sscanf(buf, "%02x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 2;
                dp += 1;
                break;
            case 4:
                FAILIF(sscanf(buf, "%04x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 4;
                dp += 2;
                break;
            case 6:
                FAILIF(sscanf(buf, "%06x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 6;
                dp += 3;
                break;
            case 8:
                FAILIF(sscanf(buf, "%08x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 8;
                dp += 4;
                break;
            case 16:
                dp += 4;
                FAILIF(sscanf(buf, "%08x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 8;
                dp -= 4;
                FAILIF(sscanf(buf, "%08x", (unsigned int *)dp) != 1,
                       "Error parsing (%d): %s\n", __LINE__, buffer);
                buf += 8;
                dp += 8;
                break;
            default:
                FAILIF(1, "Error parsing (%d): %s\n", __LINE__, buffer);
            }
            break;
        default:
            FAILIF(1, "Error parsing (%d): %s\n", __LINE__, buffer);
        }
    }

    dump_record(file_out, opcode, dp - data, data);
}


int main(int argc, char **argv) {

    if (argc != 3)
        usage();

    FILE *file_in = fopen(argv[1], "r");
    FAILIF(!file_in, "Could not open %s: %s\n", argv[1], strerror(errno));

    FILE *file_out = fopen(argv[2], "w+");
    FAILIF(!file_out, "Could not open %s: %s\n", argv[2], strerror(errno));

    char buffer[1024];
    char *buf;

    while (fgets(buffer, 1024, file_in) != NULL) {
        buf = buffer;
        while (*buf && isspace(*buf))
            buf++;
        switch (*buf) {
        case 'S':
            process_line(file_out, buf, buffer);
            break;
        case 'W':  // Wait_HCI_Command... meta-data, not needed
        case '#':
        case 0:
            continue;
        default:
            FAILIF(1, "Don't know what to do with: %s\n", buffer);
        }
    }

    fclose(file_in);
    fclose(file_out);

    return 0;
}
