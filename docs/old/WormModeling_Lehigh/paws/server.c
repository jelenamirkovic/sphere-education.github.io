/*
 * server.c: simulation of Internet worm propagation on Emulab/Deter testbed
 * running on control node (node0)
 */
/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 */


#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include "paws.h"

int main(int argc, char** argv) {
  int sock_i;
  struct sockaddr_in my_addr;
  struct sockaddr_in your_addr;
  int i, j, k, t, m, n, yes;
  unsigned char msg[256];
  struct hostent *h;

  printf("************************************\n");
  printf("Master machine starts ..............\n");
  printf("At most %2d slaves can be enrolled.\n", MAX_SLAVE);

  if ((sock_master = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Master socket fails:");
    exit(1);
  }

//set the socket option so that the the port can be resued if already occupied
  yes = 1;
  if (setsockopt(sock_master, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt() fails");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PAWS_PORT);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(my_addr.sin_zero), 0, 8);
  if (bind(sock_master, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) < 0) {
    perror("Binding master socket fails:");
    exit(1);
  }

  //Receiving connections from slave nodes
  for (i = 1; i <= NODE_NUMBER; i++) {
    if (listen(sock_master, MAX_SLAVE) < 0) {
      perror("Listening to slave nodes fails:");
      exit(1);
    }

    j = sizeof(struct sockaddr_in);
    if ((sock_i = accept(sock_master, (struct sockaddr*)&your_addr, &j)) < 0) {
      perror("Accepting slave socket fails:");
      exit(1);
    }
    if ((j=recv(sock_i, msg, 256, 0)) < 0) {
      perror("Receiving from slave fails:");
      exit(1);
    }
    k = *((int*)msg);
    sock_list[k] = sock_i;
    printf("%d gets connected (%d)\n", k, j);
  }

  //Sending the seed for random number generator on slave nodes
  srand(time(NULL));
  i = rand();
  //i = 1084061792;
  memcpy(msg, &i, sizeof(int));
  for (i = 1; i <= NODE_NUMBER; i++)
    if ((j = send(sock_list[i], msg, 4, 0)) != 4) {
      printf("Invalid packet size %d\n", j);
      i--;
    }

  //Starting the simulation, getting reports from slave nodes periodically
  t = 0;
  printf("sec\th:m\t"); //printing current time
  for (i = 1; i <= NODE_NUMBER; i++)
    printf("inf%d\t", i);
  printf("infected\n");
  while (1) {
    m = 0;
    printf("%d\t%d:%d\t", t, t/3600, t/60%60); //printing current time
    for (i = 1; i <= NODE_NUMBER; i++) {
      if ((j = recv(sock_list[i], msg, 12, 0)) < 0) {
	perror("Receiving from slave nodes fails:");
	exit(1);
      }

      if (j == 0) {
	printf("Node %d has closed the connection.", i);
	exit(1);
      }
      
      if (j != 12)
	printf("Invalid packet size %d\n", j);
				
      memcpy(&n, msg, 4); //Node id
      if (n != i)
	printf("Invalid node %d:%d\n", n, i);

      memcpy(&n, msg+4, 4); //Time stamp
      if (n != t)
	printf("Invalid t %d:%d\n", n, t);
				
      memcpy(&n, msg+8, 4); //Current infected #
      m += n;
      printf("%d\t", n);
    }
    printf("%d\n", m);
    fflush(stdout);
    t+=REPORT_INTERVAL;
    if (m == 0)
      {
	printf("No infected hosts at the start of the simulation. Rerun the code!\n");
	exit(1);
      }
  }
}
