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

int read[2], write[2];
int size;
char *in, *out;
int64_t count, i, delta;

void *threadfn(char *in) {
  int i;

  for (i = 0; i < count; i++) {
      
      if (localread(1,out, in, size) != size) {
        perror("read");
        return 1;
      }
      
      if (localwrite(0,in, out, size) != size) {
        perror("write");
        return 1;
      }
    }
}

int localread(int fd, char *in, char * out, int size) {
  int nbytes;
  
  while(!write[fd])
    sleep(0);

  write[fd] = 0;

  memcpy(out, in, size);

  read[fd] = 1;
  return size;
}

int localwrite(int fd, char *in, char * out, int size) {
  memcpy(out, in, size);

  write[fd] = 1;
  while (!read[fd])
    sleep(0);

  read[fd] = 0;
  return size;
}

int main(int argc, char *argv[])
{
  struct timeval start, stop;

  pthread_t thread;


  if (argc != 3) {
    printf ("usage: common_lat <message-size> <message-count>\n");
    exit(1);
  }

  read[0] = read[1] = 0;
  write[0] = write[1] = 0;
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

      if (localwrite(1,in, out, size) != size) {
        perror("write");
        return 1;
      }

      if (localread(0,out, in, size) != size) {
        perror("read");
        return 1;
      }
      
    }

  gettimeofday(&stop, NULL);

  delta = ((stop.tv_sec - start.tv_sec) * (int64_t) 1000000 +
       stop.tv_usec - start.tv_usec);
    
    printf("average latency: %f us\n", (double)delta / (double)(count * 2));
  
  
  return 0;
}
