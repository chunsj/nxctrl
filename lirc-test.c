/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Basic LIRC Test(with Apple Remote) Program
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lirc/lirc_client.h>

// XXX create an rc file named /etc/lirc/lircrc if not exists

int
main (int argc, const char *argv[]) {
  int fd;
  struct lirc_config *config;
  char *code;

  if ((fd = lirc_init("lirc", 1)) == -1) {
    fprintf(stderr, "cannot init lirc\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "LIRC FD = %d\n", fd);
  fflush(stdout);

#if 0 // XXX test non-block socket (for integration with tcdr)
  if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
    fprintf(stderr, "cannot make socket as non block\n");
    exit(EXIT_FAILURE);
  }
#endif

  if (lirc_readconfig(NULL, &config, NULL) == 0) {
    while (lirc_nextcode(&code) == 0) {
      if (!code)
        continue;

      if (strstr(code, "KEY_MENU")) {
        fprintf(stdout, "MENU\n");
        fflush(stdout);
      } else if (strstr(code, "KEY_PLAYPAUSE")) {
        fprintf(stdout, "PLAY\n");
        fflush(stdout);
      } else if (strstr(code, "KEY_FORWARD")) {
        fprintf(stdout, "FWRD\n");
        fflush(stdout);
      } else if (strstr(code, "KEY_REWIND")) {
        fprintf(stdout, "RWND\n");
        fflush(stdout);
      } else if (strstr(code, "KEY_VOLUMEUP")) {
        fprintf(stdout, "VLUP\n");
        fflush(stdout);
      } else if (strstr(code, "KEY_VOLUMEDOWN")) {
        fprintf(stdout, "VLDN\n");
        fflush(stdout);
      } else {
        fprintf(stderr, "UNKN\n");
      }

      free(code);
    }

    lirc_freeconfig(config);
  }

  lirc_deinit();
  
  return 0;
}
