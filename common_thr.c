/* 
    Measure throughput of IPC using pipes


    Copyright (c) 2010 Erik Rigtorp <erik@rigtorp.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

int read, write;
int size;
char *in, *out;
int64_t count, i, delta;

void *threadfn(char *in) {
  int i;

  for (i = 0; i < count; i++) {      
      if (localread(out, in, size) != size) {
        perror("read");
        exit(1);
      }
    }
}

int localread(char *in, char * out, int size) {
  int nbytes;
  
  while(!write)
    sleep(0);

  write = 0;

  memcpy(out, in, size);

  read = 1;
  return size;
}

int localwrite(char *in, char * out, int size) {
  memcpy(out, in, size);

  write = 1;
  while (!read)
    sleep(0);

  read = 0;
  return size;
}

int main(int argc, char *argv[])
{
  struct timeval start, stop;

  pthread_t thread;


  if (argc != 3) {
    printf ("usage: common_thr <message-size> <message-count>\n");
    exit(1);
  }

  read = 0;
  write = 0;
  size = atoi(argv[1]);
  count = atol(argv[2]);

  in = malloc(size);
  out = malloc(size);
  if ((in == NULL) || (out == NULL)) {
    perror("malloc");
    exit(1);
  }

  printf("message size: %i octets\n", size);
  printf("message count: %lli\n", count);


  pthread_create(&thread, NULL, threadfn, (void *)in);

  /* parent */

  gettimeofday(&start, NULL);

  for (i = 0; i < count; i++) {
    if (localwrite(in, out, size) != size) {
      perror("write");
      exit(1);
    }
  }

  gettimeofday(&stop, NULL);

    delta = ((stop.tv_sec - start.tv_sec) * (int64_t) 1e6 +
	     stop.tv_usec - start.tv_usec);

  printf("average throughput: %lli msg/s\n", (count * (int64_t) 1e6) / delta);
  printf("average throughput: %lli Mb/s\n", (((count * (int64_t) 1e6) / delta) * size * 8) / (int64_t) 1e6);
  
  
  return 0;
}
