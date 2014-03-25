#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

char get_bit(char a, int i) {
  return (a & (1 << i) ) >> i;
}

void send(char* mm, int size) {
  my_pkt p;
  msg t;
  int i,j;
  char sum = '\0';
  char *pp;
  
  memset(&p, 0, sizeof(my_pkt));

  p.type = TYPE1;
  p.err=0;
  memcpy(p.payload, &mm, size);
  
  //make packet
  t.len = sizeof(int) + sizeof(char) + strlen(p.payload);
  pp = &p;
  for (i=0; i<t.len; i++)
    for (j=0; j<8; j++)
      sum ^= get_bit(*(pp+i),j);
    
  p.err = sum;
  
  //make msg
  memcpy(t.payload, &p, t.len); 

  send_message(&t);
}

int main(int argc,char** argv){
  init(HOST,PORT);
  msg t;
  
  send(argv[1], strlen(argv[1]));

  if (recv_message(&t)<0){
    perror("receive error");
    return -1;
  }
  else {
    printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
  }

  printf("Resending...\n");
  send(argv[1], strlen(argv[1]));
  
  if (recv_message(&t)<0){
    perror("receive error");
    return -1;
  }
  else {
    printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
  }

  return 0;
}
