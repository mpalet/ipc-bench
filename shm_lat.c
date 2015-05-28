/* Measure latency of IPC using shm */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char *read, *write;


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
  //memcpy(out, in, size);

  write[fd] = 1;
  while (!read[fd])
    sleep(0);

  read[fd] = 0;
  return size;
}


int main(int argc, char *argv[])
{
  int pfds[2];
  
  char c;
  int shmid;
  key_t key;

  int size;
  char *buf, *shm;
  int64_t count, i, delta;
  struct timeval start, stop;

 

  if (argc != 3) {
    printf ("usage: pipe_thr <message-size> <message-count>\n");
    exit(1);
  }

  size = atoi(argv[1]);
  count = atol(argv[2]);

  buf = malloc(size);
  if (buf == NULL) {
    perror("malloc");
    exit(1);
  }

  /*
   * We'll name our shared memory segment
   * "5678".
   */
  key = 56562377;
  
  if (!fork()) {
    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, size+4, IPC_CREAT | 0666)) < 0) {
      perror("shmget");
      exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char*) -1) {
      perror("shmat");
      exit(1);
    }
    
    read = shm;
    write = shm+2;
    read[0] = read[1] = write[0] = write[1] = 0;

    for (i = 0; i < count; i++) {
          
          if (localread(1,shm, buf, size) != size) {
            perror("read");
            return 1;
          }
          
          if (localwrite(0,buf, shm, size) != size) {
            perror("write");
            return 1;
          }
    } 
  } else {
    sleep(1);

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, size+4, 0666)) < 0) {
      perror("shmget");
      exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
      perror("shmat");
      exit(1);
    }

    read = shm;
    write = shm+2;
    read[0] = read[1] = write[0] = write[1] = 0;


    gettimeofday(&start, NULL);

  for (i = 0; i < count; i++) {

      if (localwrite(1,buf, shm, size) != size) {
        perror("write");
        return 1;
      }

      if (localread(0,shm, buf, size) != size) {
        perror("read");
        return 1;
      }
      
    }

  gettimeofday(&stop, NULL);

    delta = ((stop.tv_sec - start.tv_sec) * (int64_t) 1000000 +
       stop.tv_usec - start.tv_usec);
    
    printf("average latency: %f us\n", (double)delta / (double)(count * 2));
  }
  
  return 0;
}
