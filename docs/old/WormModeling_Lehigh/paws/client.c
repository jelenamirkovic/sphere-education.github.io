/*
 * client.c: simulation of Internet worm propagation on Emulab/Deter testbed
 * running on slave nodes
 */
/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/timeb.h>


//#define NDEBUG

#define NETWORK_CONGESTION	//Consider network congestion
//#define BACKGROUND_TRAFFIC	//Consider legitimate traffic
//#define VULNERABLE_LOGNORMAL_DISTRIBUTION	//Distribution of vulnerable hosts
//#define TIME_UNIT_SCALING


//#define WITTY_WORM
#define SLAMMER_WORM
//#define OTHER_WORM


#include <assert.h>
#include "paws.h"


#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))


//Broadcast the newly infected to others
//t is the current time
void broadcast_new_infected(int t)
{
  u_int i, j;
  u_int k, r, s;
  u_char msg[64];
  u_char *p;

  for (i = 1; i < host_id; i++) {
    host_buf_array[i-1].size = 0;
    j = 0;
    p = (u_char*)host_buf_array[i-1].ip;
    while (1) {
      k = recv(sock_list[i], p+j, RECV_BLOCK_SIZE, 0);
      j += k;
      if (strcmp((char*)(p+j-4), "%^&") == 0)
	break;
    }
    j -= 16;
    memcpy(&r, p+j, 4);
    memcpy(&k, p+j+4, 4);
    memcpy(&s, p+j+8, 4);
    if (r != j || k != t || s != i)
      printf("Error in broadcast_new_infected: wrong socket recv %d:%d:%d\t%d:%d:%d\n", r, k, s, j, t, i);
    else
      host_buf_array[i-1].size = j / 3 / sizeof(u_int);
  }

  for (i = 1; i <= NODE_NUMBER; i++) {
    if (i != host_id) {
      j = 0;
      s = host_buf.size * sizeof(u_int) * 3;
      p = (u_char*)host_buf.ip;
      while (j < s) {
	r = (j+SEND_BLOCK_SIZE)<=s?SEND_BLOCK_SIZE:(s-j);
	k = send(sock_list[i], p+j, r, 0);
	if (k < 0) {
	  printf("Error in broadcast_new_infected: sending out %u %u %u", k, s, j);
	  exit(1);
	}
	j += k;
      }

      memcpy(msg, &s, 4);
      memcpy(msg+4, &t, 4);
      memcpy(msg+8, &host_id, 4);
      msg[12] = '%';
      msg[13] = '^';
      msg[14] = '&';
      msg[15] = '\0';
      k = send(sock_list[i], msg, 16, 0);
      if (k < 0 || k != 16) {
	printf("Error in broadcast_new_infected: sending out %u", k);
	exit(1);
      }
    }
  }

  for (i = host_id+1; i <= NODE_NUMBER; i++) {
    host_buf_array[i-1].size = 0;
    j = 0;
    p = (u_char*)host_buf_array[i-1].ip;
    while (1) {
      k = recv(sock_list[i], p+j, RECV_BLOCK_SIZE, 0);
      j += k;
      if (strcmp((char*)(p+j-4), "%^&") == 0)
	break;
    }
    j -= 16;
    memcpy(&r, p+j, 4);
    memcpy(&k, p+j+4, 4);
    memcpy(&s, p+j+8, 4);
    if (r != j || k != t || s != i)
      printf("Error in broadcast_new_infected: wrong socket recv %d:%d:%d\t%d:%d:%d\n", r, k, s, j, t, i);
    else
      host_buf_array[i-1].size = j / 3 / sizeof(u_int);
  }
}

//Broadcast the traffic status on this node to others
// t is the current time
void broadcast_AS_traffic(int t)
{
  register u_int i, j;
  u_int k, r, s;
  u_char msg[64];
  u_char *p;

  memset(current_Link_Traffic, 0, TRAFFIC_BUFFER_SIZE);

  for (i = 1; i < host_id; i++) {
    j = 0;
    p = (u_char*)traffic_Buffer;
    while (1) {
      k = recv(sock_list[i], p+j, TRAFFIC_BLOCK_SIZE, 0);
      j += k;
      if (j >= TRAFFIC_BUFFER_SIZE)
	break;
    }
    for (j = 0; j < LINK_NUMBER; j++)
      current_Link_Traffic[j] += traffic_Buffer[j];
  }

  for (i = 1; i <= NODE_NUMBER; i++) {
    if (i != host_id) {
      j = 0;
      s = TRAFFIC_BUFFER_SIZE;
      p = (u_char*)new_Link_Traffic;
      while (j < s) {
	r = (j+TRAFFIC_BLOCK_SIZE) <= s ? TRAFFIC_BLOCK_SIZE : (s-j);
	k = send(sock_list[i], p+j, r, 0);
	if (k < 0) {
	  printf("Error in broadcast_new_infected: sending out %u %u %u", k, s, j);
	  exit(1);
	}
	j += k;
      }
    }
  }

  for (i = host_id+1; i <= NODE_NUMBER; i++) {
    j = 0;
    p = (u_char*)traffic_Buffer;
    while (1) {
      k = recv(sock_list[i], p+j, TRAFFIC_BLOCK_SIZE, 0);
      j += k;
      if (j >= TRAFFIC_BUFFER_SIZE)
	break;
    }
    for (j = 0; j < LINK_NUMBER; j++)
      current_Link_Traffic[j] += traffic_Buffer[j];
  }

  for (i = 0; i < LINK_NUMBER; i++) 
    current_Link_Traffic[i] += new_Link_Traffic[i]; 

}

//Report to master the currently infected number
//t is the current time
//n is the currently infected number on this node
void report_master(int t, int n)
{
  register int i, j;
  u_char msg[32];

  memcpy(msg, &host_id, 4);
  memcpy(msg+4, &t, 4);
  memcpy(msg+8, &n, 4);

  if ((j=send(sock_master, msg, 12, 0)) < 0) {
    perror("Reporting to master fails:");
    exit(1);
  }
  if (j != 12)
    printf("Error in report_master: %d\n", j);
}

//start is the starting IP address
//size is the size of this IP range
//bgp_atom is the atom number
void add_IPrange_to_BGP_atom(u_int start, u_int size, int bgp_atom)
{
  if (BGP_atom_list[bgp_atom].size >= BGP_atom_list[bgp_atom].volume) {
    if (BGP_atom_list[bgp_atom].volume == 0) {
      BGP_atom_list[bgp_atom].volume = 64;
      BGP_atom_list[bgp_atom].table = malloc(sizeof(u_int) * BGP_atom_list[bgp_atom].volume * 2);
    }
    else {
      BGP_atom_list[bgp_atom].volume = BGP_atom_list[bgp_atom].volume << 1;
      BGP_atom_list[bgp_atom].table = realloc(BGP_atom_list[bgp_atom].table, sizeof(u_int) * BGP_atom_list[bgp_atom].volume * 2);
    }
  }

  BGP_atom_list[bgp_atom].table[BGP_atom_list[bgp_atom].size * 2] = start;
  BGP_atom_list[bgp_atom].table[BGP_atom_list[bgp_atom].size * 2 + 1] = size;
  BGP_atom_list[bgp_atom].size++;
  BGP_atom_list[bgp_atom].ips += size;
}

//determine the configuration of an vulnerable host
unsigned short xsubi_witty1[3];
unsigned short xsubi_witty2[3];
unsigned short xsubi_witty3[3];
u_int witty_access_bw(u_int lbound, u_int ubound)
{
  uint d = lbound + (unsigned int)floor((ubound - lbound) * erand48(xsubi_witty1) + 0.5);
  /*  
  //dat
  if (d < 56)
    return d;
  else if (d < 64)
    return 56;
  else if (d < 128)
    return 64;
  else if (d < 768)
    return 64;
  else if (d < 1544)
    return 128;
  else if (d < 10000)
    return 128;
  else if (d < 100000)
    return 768;
  else if (d < 200000)
    return 1.544;
  else
    return 100000;
  */
  /*
  //txt
  if (d < 56)
    return d;
  else if (d < 128)
    return 56;
  else if (d < 768)
    return 64;
  else if (d < 1544)
    return 64;
  else if (d < 50000)
    return 128;
  else if (d < 100000)
    return 768;
  else if (d < 200000)
    return 1544;
  else if (d < 300000)
    return 10000;
  else
    return 100000;
  */

  return d;
}

u_int witty_first_hop_bw()
{
  double d = erand48(xsubi_witty3);

  //based on CAIDA witty connection-speed data
  if (d < 0.437) //broadband 10M 1.544M
    return witty_access_bw(1544, 10000);
  else if (d < 0.6974) //0.2604 xDSL 128K
    return witty_access_bw(64, 128);
  else if (d < 0.8708) //0.1734 dialup 56K
    return witty_access_bw(10, 56);
  else if (d < 0.9851) //0.1143 cable 768K
    return witty_access_bw(128, 768);
  else if (d < 0.9996) //0.0145 T1 1.544M
    return witty_access_bw(768, 1544);
  else if (d < 0.9999) //0.0003 ISDN 64K
    return witty_access_bw(56, 64);
  else //0.0001 100M
    return witty_access_bw(10000, 100000);
}

void host_configuration(struct BGP_Atom_Vulnerable *ptr)
{
#ifdef SLAMMER_WORM
  /*slammer*/
  if (rand() % 100 < 50)
    ptr->host_config[ptr->size].bandwidth = 3152;
  else
    ptr->host_config[ptr->size].bandwidth = 100000;
#else
  ptr->host_config[ptr->size].bandwidth = SCAN_RATE * WORM_SIZE * 8 / 1000.0;
#endif

  ptr->host_config[ptr->size].scan_rate = SCAN_RATE;
  ptr->host_config[ptr->size].prng_seed = rand();
  ptr->host_config[ptr->size].worm_index = vulnerable_number;
  ptr->host_config[ptr->size].propagation_duration = -1;
 
}

//return 1 if ip already exists
int add_vulnerable_host(u_int ip, int bgp_atom)
{
  register u_int i, j, k;
  struct BGP_Atom_Vulnerable *p;

  p = &BGP_atom_list[bgp_atom].vulnerable;

  for (i = 0; i < p->size; i++)
    if (p->ip[i] == ip)
      return 1;

  if (p->size >= p->volume) {
    if (p->volume == 0) {
      p->volume = 16;
      p->ip = malloc(sizeof(u_int) * p->volume);
      p->status = malloc(sizeof(u_int) * p->volume);
      p->host_config = malloc(sizeof(struct Worm_Vulnerable_Host) * p->volume);
    }
    else {
      p->volume = p->volume << 1;
      p->ip = realloc(p->ip, sizeof(u_int) * p->volume);
      p->status = realloc(p->status, sizeof(u_int) * p->volume);
      p->host_config = realloc(p->host_config, sizeof(struct Worm_Vulnerable_Host) * p->volume);
    }
  }
  p->ip[p->size] = ip;

  host_configuration(p);


#ifdef WITTY_WORM
  if (rand() % 100 == 0) {
#else
  if (rand() % 50000 == 0) {
#endif
    p->status[p->size] = 1;
    p->host_config[p->size].propagation_duration = DEFAULT_PROPAGATION_DURATION;
  }
  else
    p->status[p->size] = 0;

  (p->size)++;

  vulnerable_number++;
  
  return 0;
  }

//Return f(x) when given x. f is the pdf of log-normal distribution with (M,S)
double pLog(int x)
{
  double p1, p2;
  double M = 6;
  double S = 2;

  p1 = log(x) - M;
  p1 = p1 * p1;
  p1 = p1 / (S * S * 2);
  p1 = exp(-p1);
  p2 = S * sqrt(2 * 3.1415926) * x;

  return p1/p2;
}

//Determine the vulnerable host inside each BGP atom and return the total number of vulnerables
int determine_vulnerable()
{
  FILE* infile;
  char c_buf[256];
  register u_int i, j, k, progress_count, report_count;
  register u_int range_vul_num, range_size, range_ip_start, pLog_sum, split_pieces;
  register u_int new_range_start, new_range_size, fake_atom_id, new_range_vul_num;
  int i1, i2, i3, i4, i5, i6;
  double pLog_value;

  infile = fopen("IPRangeTable.dat", "r");
  if (infile == NULL) {
    printf("Error: Failed to open the file IPRangeTable.dat\n");
    exit(1);
  }

  //read the number of ranges
  fgets(c_buf, 256, infile);
  ip_range_number = atoi(c_buf);

  //skip the legend line
  fgets(c_buf, 256, infile);

  progress_count = 0;
  report_count = 0;
  pLog_sum = 0;
  for (i = 0; i < ip_range_number; i++){
    fgets(c_buf, 256, infile);
    sscanf(c_buf, "%d\t%d\t%d\t%d\t%d\t%d", &i1, &i2, &i3, &i4, &i5, &i6);
    range_size = 1 << (32-i5);
    range_ip_start = (i1 << 24) + (i2 << 16) + (i3 << 8) + i4;
    range_ip_start = (range_ip_start >> (32-i5)) << (32-i5);

#ifdef VULNERABLE_LOGNORMAL_DISTRIBUTION
    if (i5 < 16) {
      pLog_value = 0;
      for (k = 0; k < (1<<(16-i5)); k++)
	pLog_value += pLog((i1 << 8) + i2 + k);

      range_vul_num = (u_int)floor(pLog_value * VULNERABLE_NUMBER * 10.0); //number of vulnerables in this IP range
      pLog_sum += pLog_value * 6.5;
    }
    else {
      range_vul_num = (u_int)floor(pLog((i1 << 8) + i2) * VULNERABLE_NUMBER * range_size / 65536 * 10.0);
      pLog_sum += pLog((i1 << 8) + i2) * 6.5 * range_size / 65536;
    }
#else
    range_vul_num = floor(vulnerable_ratio * range_size + 0.5); //number of vulnerables in this IP range
#endif


    assert(range_vul_num < range_size);

    //split big range
    split_pieces = 1; //number of pieces to split
    while (range_vul_num / split_pieces * 8 > VULNERABLE_NUMBER / NODE_NUMBER)
      split_pieces <<= 1;
    
    for (k = 0; k < split_pieces; k++) {
      if (total_atom_number > BGP_ATOM_NUMBER + 1000) {
	printf("More space needed for extended fake BGP atoms.\n");
	exit(1);
      }
      
      if (k == 0) {
	new_range_start = range_ip_start;
	new_range_size = range_size / split_pieces;
	fake_atom_id = i6;
        new_range_vul_num = range_vul_num - range_vul_num / split_pieces * (split_pieces - 1);
      }
      else {
	new_range_start = range_ip_start + k * (range_size / split_pieces);
	new_range_size = range_size / split_pieces;
	fake_atom_id = total_atom_number;
	total_atom_number++;
	new_range_vul_num = range_vul_num / split_pieces;
	BGP_atom_list[fake_atom_id].parent_atom = i6;
      }
      add_IPrange_to_BGP_atom(new_range_start, new_range_size, fake_atom_id);
      for (;new_range_vul_num > 0; new_range_vul_num--) {
	j = rand() % new_range_size;
	if (add_vulnerable_host(new_range_start + j, fake_atom_id))
	  new_range_vul_num++;
      }
    }
    
    //progress report
    progress_count += range_size;
    if (progress_count / 0xFFFFFFF > report_count ) {
      report_count++;
      printf("%.0f%%\tis done\n", progress_count * 1.0 / SPACE_SIZE * 100);
    }
  }
  fclose(infile);

  printf("Totally there are %u/%u vulnerable IPs\n", vulnerable_number, VULNERABLE_NUMBER);
#ifdef VULNERABLE_LOGNORMAL_DISTRIBUTION
  printf("Accummulated pLog value is %f\n", pLog_sum);
#endif
  fflush(stdout);
  return vulnerable_number;
}


//Buffer an scan for future retransmission
void add_buffered_scan(int src, int des, u_int ip, int rtx_time)
{
  register int i, j, k;

  i = buffered_scan.scan_list_head;
  j = buffered_scan.scan_list_tail;

  if (buffered_scan.buffer_capacity == 0) {
    buffered_scan.buffer_capacity = INIT_SCAN_BUFFER_CAPACITY;
    buffered_scan.scan_src_as = (int*)malloc(sizeof(int) * buffered_scan.buffer_capacity);
    if (buffered_scan.scan_src_as == NULL) {
      printf("Error in add_buffered_scan(): failed to allocate memory\n");
      exit(1);
    }
    buffered_scan.scan_des_atom = (int*)malloc(sizeof(int) * buffered_scan.buffer_capacity);
    if (buffered_scan.scan_des_atom == NULL) {
      printf("Error in add_buffered_scan(): failed to allocate memory\n");
      exit(1);
    }
    buffered_scan.scan_des_ip = (u_int*)malloc(sizeof(u_int) * buffered_scan.buffer_capacity);
    if (buffered_scan.scan_des_ip == NULL) {
      printf("Error in add_buffered_scan(): failed to allocate memory\n");
      exit(1);
    }
    buffered_scan.scan_scheduled_time = (int*)malloc(sizeof(int) * buffered_scan.buffer_capacity);
    if (buffered_scan.scan_scheduled_time == NULL) {
      printf("Error in add_buffered_scan(): failed to allocate memory\n");
      exit(1);
    }
  }
  else
    if (buffered_scan.buffer_capacity <= buffered_scan.scan_number) {
      k = buffered_scan.buffer_capacity;
      buffered_scan.buffer_capacity <<= 1;
      buffered_scan.scan_src_as = (int*)realloc(buffered_scan.scan_src_as, sizeof(int) * buffered_scan.buffer_capacity);
      if (buffered_scan.scan_src_as == NULL) {
	printf("Error in add_buffered_scan(): failed to allocate memory\n");
	exit(1);
      }
      memcpy(&(buffered_scan.scan_src_as[k+i]), &(buffered_scan.scan_src_as[i]), sizeof(int)*(k-i));
      buffered_scan.scan_des_atom = (int*)realloc(buffered_scan.scan_des_atom, sizeof(int) * buffered_scan.buffer_capacity);
      if (buffered_scan.scan_des_atom == NULL) {
	printf("Error in add_buffered_scan(): failed to allocate memory\n");
	exit(1);
      }
      memcpy(&(buffered_scan.scan_des_atom[k+i]), &(buffered_scan.scan_des_atom[i]), sizeof(int)*(k-i));
      buffered_scan.scan_des_ip = (u_int*)realloc(buffered_scan.scan_des_ip, sizeof(u_int) * buffered_scan.buffer_capacity);
      if (buffered_scan.scan_des_ip == NULL) {
	printf("Error in add_buffered_scan(): failed to allocate memory\n");
	exit(1);
      }
      memcpy(&(buffered_scan.scan_des_ip[k+i]), &(buffered_scan.scan_des_ip[i]), sizeof(u_int)*(k-i));
      buffered_scan.scan_scheduled_time = (int*)realloc(buffered_scan.scan_scheduled_time, sizeof(int) * buffered_scan.buffer_capacity);
      if (buffered_scan.scan_scheduled_time == NULL) {
	printf("Error in add_buffered_scan(): failed to allocate memory\n");
	exit(1);
      }
      memcpy(&(buffered_scan.scan_scheduled_time[k+i]), &(buffered_scan.scan_scheduled_time[i]), sizeof(int)*(k-i));
      buffered_scan.scan_list_head = i + k;
    }

  buffered_scan.scan_src_as[j] = src;
  buffered_scan.scan_des_atom[j] = des;
  buffered_scan.scan_des_ip[j] = ip;
  buffered_scan.scan_scheduled_time[j] = rtx_time;
  buffered_scan.scan_list_tail = (j + 1) % buffered_scan.buffer_capacity;
  buffered_scan.scan_number ++;
}

void add_newly_infected_host(u_int ip, int atom, int offset, int delay)
{
  if (host_buf.size >= host_buf.volume) {
    host_buf.volume <<= 1;
    host_buf.ip = realloc(host_buf.ip, sizeof(u_int) * host_buf.volume * 3);
  }
  host_buf.ip[host_buf.size*3] = delay;
  host_buf.ip[host_buf.size*3+1] = atom;
  host_buf.ip[host_buf.size*3+2] = offset;
  host_buf.size++;
}

//update the status of each infectee host
void update_infectee_status(struct Worm_Vulnerable_Host *host)
{
}

//a vulnerable host is scanned and might be infected
void infect_vulnerable_host(struct ip_buf buf, int t)
{
  struct BGP_Atom_Vulnerable *p;
  register u_int m, k;

  for (m = 0; m < buf.size; m++) {
    k = buf.ip[m*3+1];
    p = &BGP_atom_list[k].vulnerable;
    k = buf.ip[m*3+2];

    if (buf.ip[m*3] > p->status[k]) {
      p->status[k] = buf.ip[m*3];
      p->host_config[k].propagation_duration = DEFAULT_PROPAGATION_DURATION;
    }
  }
}

//return 1 if ip is vulnerable, 0 if ip is infected or not vulnerable
int vulnerable(u_int ip, int atom, int *offset)
{
  struct BGP_Atom_Vulnerable *p;
  register int i, j, k;

  p = &BGP_atom_list[atom].vulnerable;

  i = 0;
  j = p->size - 1;
  while (i <= j) {
    k = (i+j)/2;
    if (ip == p->ip[k]) {
      *offset = k;
      if (p->status[k])
	return 0;
      else
	return 1;
    }
    else
      if (ip < p->ip[k])
	j = k - 1;
      else
	i = k + 1;
  }

  return 0;
}

void generate_ip_target(u_int host, u_int *ip, int *atom)
{
  register u_int k;
  register int i, j, r, s;

#ifdef SLAMMER_RNG_ERROR
  k = host % 4; //group of this host (0, 1, 2, 3)
  i = SPACE_SIZE >> 2; //
  j = SPACE_SIZE - i * 3;
  if (k == 3)
    r = j;
  else
    r = i;

  s = rand() % r;
  k = s + k * i;
#else
  do {
    k = rand();
  } while (k >= SPACE_SIZE);
#endif


  i = 0;
  j = total_atom_number - 1;
  while (i < j) {
    r = (i+j)/2;
    if (k >= BGP_atom_list[r].ips)
      i = r + 1;
    else
      j = r;
  }

  *atom = j;  
  s = j;
  if (j > 0) {
    //	assert(BGP_atom_list[j].ips != BGP_atom_list[j-1].ips);
    k -= BGP_atom_list[j-1].ips;
  }	
  assert(BGP_atom_list[s].size > 0);

  i = 0;
  j = BGP_atom_list[s].size - 1;
  while (i < j) {
    r = (i+j)/2;
    if (k >= BGP_atom_list[s].table[r*2+1])
      i = r + 1;
    else
      j = r;
  }

  if (j > 0)
    k -= BGP_atom_list[s].table[j*2-1];

  *ip = BGP_atom_list[s].table[i*2]+k;
}

//return 0 for no congestion, 1 for congestion (packet drop)
int calculate_routing_path(int src_as, int des_atom, int t /*time*/)
{
  register int i, j, k;
  register u_int worm_traffic;

  if (BGP_atom_list[des_atom].parent_atom >= 0)
    des_atom = BGP_atom_list[des_atom].parent_atom;

#ifndef NETWORK_CONGESTION
  return 0;
#endif

  while (1) {
    if (src_as == BGPatom2AS[des_atom]) {
	return 0;
    }

    i = 0;
    j = route_path[src_as].entry_count-1;
    while (i < j) {
      k = (i+j)/2;
      if (des_atom <= route_path[src_as].BGP_index[k])
	j = k;
      else
	i = k+1;
    }

    assert(i == j);
    assert(des_atom <= route_path[src_as].BGP_index[i]);

    j = route_path[src_as].next_AS[i];

    if (j < 0) {
      printf("Unreachable destination: AS%d->Atom%d\n", src_as, des_atom);
      exit(-1);
    }

    new_Link_Traffic[j]++;
    
    if (link_List[j].link_bandwidth == 0)
      return 1;

    worm_traffic = current_Link_Traffic[j] / 1000 * WORM_SIZE; //in KBytes

#ifdef BACKGROUND_TRAFFIC
    if (worm_traffic + link_List[j].link_background_traffic > link_List[j].link_bandwidth) {
      k = rand() % (worm_traffic + link_List[j].link_background_traffic);
      if (k >= link_List[j].link_bandwidth)
	return 1;
    }
#else
    if (worm_traffic > link_List[j].link_bandwidth) {
      k = rand() % worm_traffic;
      if (k >= link_List[j].link_bandwidth)
	return 1;
    }
#endif

    assert(link_List[j].link_head == src_as || link_List[j].link_tail == src_as);
    if (link_List[j].link_head != src_as) {
      src_as = link_List[j].link_head;
    }
    else {
      src_as = link_List[j].link_tail;
    }
  }

  return 0;
}


void update_link_traffic_trace_route(int src_as, int des_atom, int old_traffic_volume, int new_traffic_volume)
{
  register int i, j, k;
  int des_as = BGPatom2AS[des_atom];

  while (1) {
    if (src_as == des_as)
      break;

    i = 0;
    j = route_path[src_as].entry_count-1;
    while (i < j) {
      k = (i+j)/2;
      if (des_atom <= route_path[src_as].BGP_index[k])
	j = k;
      else
	i = k+1;
    }

    assert(i == j);
    assert(des_atom <= route_path[src_as].BGP_index[i]);

    j = route_path[src_as].next_AS[i];

    if (j < 0) {
      printf("Unreachable destination: AS%d->Atom%d in update_link_traffic()\n", src_as, des_atom);
      return;
    }

    assert(link_List[j].link_background_traffic > old_traffic_volume);
    link_List[j].link_background_traffic -= old_traffic_volume;
    link_List[j].link_background_traffic += new_traffic_volume;

    assert(link_List[j].link_head == src_as || link_List[j].link_tail == src_as);
    if (link_List[j].link_head != src_as)
      src_as = link_List[j].link_head;
    else
      src_as = link_List[j].link_tail;
  }
}


void report_congestion(u_int path_index, double pass_ratio) {
  register u_int old_traffic, new_traffic;
  register u_int i, j;
	
  old_traffic = stream_list[path_index].current_traffic;
  if (pass_ratio < 1) {
    new_traffic = old_traffic * pass_ratio;
  }
  else {
    new_traffic = stream_list[path_index].desired_traffic;
  }

  update_link_traffic_trace_route(stream_list[path_index].head_as, stream_list[path_index].tail_as, old_traffic, new_traffic);
  stream_list[path_index].current_traffic = new_traffic;
}

int com_time; //cumulative time used for inter-machine communication
void worm_infectee_scan(int t)
{
  register u_int i, j, k;
  register double congestion_severity;
  struct BGP_Atom_Type **p;
  u_int ip;
  int atom, offset, real_atom;

  int currently_infected_number, newly_infected_number;

  int scan_quota, scan_dropped, scan_passed, scan_counter;

  int atom_scan_passed, atom_scanned;

  double current_scan_rate;
  struct timeb start_time, end_time;

  
  currently_infected_number = 0;
  newly_infected_number = 0;

  memset(new_Link_Traffic, 0, TRAFFIC_BUFFER_SIZE);

  scan_dropped = 0;
  scan_passed = 0;
  scan_counter = 0;
  atom_scan_passed = 0;
  atom_scanned = 0;

  //*** retransmit previously dropped worm scans
#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
  i = buffered_scan.scan_list_head;
  j = buffered_scan.scan_list_tail;
  k = buffered_scan.buffer_capacity;
  while ((buffered_scan.scan_number > 0) && (abs(buffered_scan.scan_scheduled_time[i]) == t || abs(buffered_scan.scan_scheduled_time[i]) == 0)){
    buffered_scan.scan_number --;
    if (buffered_scan.scan_scheduled_time[i] >= 0) {
      scan_counter++;
      if (calculate_routing_path(buffered_scan.scan_src_as[i], buffered_scan.scan_des_atom[i], t) == 0) {
	scan_passed++;
	if (vulnerable(buffered_scan.scan_des_ip[i], buffered_scan.scan_des_atom[i], &offset)) {
	  add_newly_infected_host(buffered_scan.scan_des_ip[i], 
				  buffered_scan.scan_des_atom[i], 
				  offset, update_interval+1);
	  newly_infected_number++;
	}
	else {
	  if (buffered_scan.scan_scheduled_time[i] > 0) {
	    buffered_scan.scan_src_as[j] = buffered_scan.scan_src_as[i];
	    buffered_scan.scan_des_atom[j] = buffered_scan.scan_des_atom[i];
	    buffered_scan.scan_des_ip[j] = buffered_scan.scan_des_ip[i];
	    buffered_scan.scan_scheduled_time[j] = - t - 3;
	    j = (j + 1) % k;
	    buffered_scan.scan_number ++;
	  }
	}
      }
      else {
	scan_dropped++;
	if (buffered_scan.scan_scheduled_time[i] > 0) {
	  buffered_scan.scan_src_as[j] = buffered_scan.scan_src_as[i];
	  buffered_scan.scan_des_atom[j] = buffered_scan.scan_des_atom[i];
	  buffered_scan.scan_des_ip[j] = buffered_scan.scan_des_ip[i];
	  buffered_scan.scan_scheduled_time[j] = - t - 3;
	  j = (j + 1) % k;
	  buffered_scan.scan_number ++;
	}
      }
    }
    else {
      buffered_scan.scan_src_as[j] = buffered_scan.scan_src_as[i];
      buffered_scan.scan_des_atom[j] = buffered_scan.scan_des_atom[i];
      buffered_scan.scan_des_ip[j] = buffered_scan.scan_des_ip[i];
      buffered_scan.scan_scheduled_time[j] = 0;
      j = (j + 1) % k;
      buffered_scan.scan_number ++;
    }
    i = (i + 1) % k;
  }
  buffered_scan.scan_list_head = i;
  buffered_scan.scan_list_tail = j;
#endif


#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
  if (node_infected_number == 0)
    current_scan_rate = 0;
  else
    current_scan_rate = SCAN_RATE * (1 - scan_counter * 1.0 / (SCAN_RATE * node_infected_number));
  if (current_scan_rate < 0) {
    current_scan_rate = 0;
    scanning_stop_second += scan_counter / (SCAN_RATE * node_infected_number);
  }
#else
  current_scan_rate = SCAN_RATE;
#endif


  //Normal scanning
#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
  if (scanning_stop_second > 0)
    scanning_stop_second--;
#endif
  
  p = machine_list[host_id-1].machine_atom_list;
  for (i = 0; i < machine_list[host_id-1].BGP_atom_number; i++) {
    for (j = 0; j < p[i]->vulnerable.size; j++) {
      if (p[i]->vulnerable.status[j] >= 1) {
	currently_infected_number++;

	
	

#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
	//Skip scanning for these seconds if many scans are buffered
	if (scanning_stop_second > 0)
	  continue;
#endif

	current_scan_rate = 
	  p[i]->vulnerable.host_config[j].bandwidth * 
	  1000.0 / 8 / WORM_SIZE / (0xFFFFFFFF * 1.0 / SPACE_SIZE);

#ifdef SLAMMER_WORM
	if (current_scan_rate > 26000)
	  current_scan_rate = 26000;
#endif
	
	scan_quota = (int)floor((p[i]->vulnerable.status[j] - 1) * 
				SCAN_RATE + current_scan_rate + 0.5);
	
	p[i]->vulnerable.status[j] = 1;

	
	if (p[i]->vulnerable.host_config[j].propagation_duration > 0)
	  p[i]->vulnerable.host_config[j].propagation_duration -= 1;
	else if (p[i]->vulnerable.host_config[j].propagation_duration == 0)
	  scan_quota = 0;

	  
	for (k = 0; k < scan_quota; k++) {
	  generate_ip_target(p[i]->vulnerable.ip[j], &ip, &atom);
	  
	  if (BGP_atom_list[atom].parent_atom >= 0)
	    real_atom = BGP_atom_list[atom].parent_atom;
	  else
	    real_atom = atom;
	  
	  if (calculate_routing_path(BGPatom2AS[p[i]->BGP_atom_id], atom, t) == 0) {
	    scan_passed++;
	    if (vulnerable(ip, atom, &offset)) {
	      add_newly_infected_host(ip, atom, offset, update_interval+1);
	      newly_infected_number++;
	    }
	    else {
#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
	      //Scans to those invulnerable hosts will not be responded and thus retransmitted
	      add_buffered_scan(BGPatom2AS[p[i]->BGP_atom_id], atom, ip, t+3);
#endif
	    }
	  }
	  else {
#if (TRANSPORT_PROTOCOL == TCP_PROTOCOL)
	    add_buffered_scan(BGPatom2AS[p[i]->BGP_atom_id], atom, ip, t+3);
#endif
	    scan_dropped++;
	  }
	}

	update_infectee_status((&p[i]->vulnerable.host_config[j]));
      }
    }
  }
  
  node_infected_number = currently_infected_number;

  if (t % REPORT_INTERVAL == 0)
    report_master(t, currently_infected_number);
  
  j = 0;
  congestion_severity = 0;

#ifdef BACKGROUND_TRAFFIC
  for (i = 0; i < LINK_NUMBER; i++)
    link_List[i].pass_ratio = link_List[i].link_bandwidth * 1.0 / (link_List[i].link_background_traffic + current_Link_Traffic[i] / 1000 * WORM_SIZE);
#endif

#ifdef NETWORK_CONGESTION
  for (i = 0; i < LINK_NUMBER; i++) {
    if (link_List[i].pass_ratio < 1) {
      j++;
      congestion_severity += link_List[i].link_background_traffic/total_background_traffic;
    }
#endif	

#ifdef BACKGROUND_TRAFFIC
    for (k = 0; k < link_List[i].path_counter; k++)
      report_congestion(link_List[i].path_index[k], link_List[i].pass_ratio);
#endif
#ifdef NETWORK_CONGESTION
  }
#endif


  if (t % REPORT_INTERVAL == 0) {
#if (TRANSPORT_PROTOCOL == UDP_PROTOCOL)
    printf("%d\t%d\t%d\t%.8d %.8d\n", 
	   t, 
	   currently_infected_number, 
	   current_interval, 
	   scan_dropped, 
	   scan_passed);
#else
    printf("%d\t%d\t%d\t%f\t%d\n", 
	   t, 
	   currently_infected_number, 
	   current_interval, 
	   current_scan_rate, 
	   buffered_scan.scan_number);
#endif
    fflush(stdout);
  }

  if (update_interval == 0) {
    ftime(&start_time);
    
    broadcast_new_infected(t);
#ifdef NETWORK_CONGESTION
    broadcast_AS_traffic(t);
#endif

    ftime(&end_time);

    j  = 0;
    for (i = 0; i < NODE_NUMBER; i++) {
      if (i+1 == host_id) {
	j += host_buf.size;
	infect_vulnerable_host(host_buf, t);
	host_buf.size = 0;
      }
      else {
	j += host_buf_array[i].size;
	infect_vulnerable_host(host_buf_array[i], t);
	host_buf_array[i].size = 0;
      }
    }
    
    
#ifdef TIME_UNIT_SCALING
    if (j < vulnerable_number/20000)
      update_interval = 20;
    else if (j < vulnerable_number/10000)
      update_interval = 10;
    else if (j < vulnerable_number/5000)
      update_interval = 5;
    else if (j < vulnerable_number/3000)
      update_interval = 3;
    else if (j < vulnerable_number/1000)
      update_interval = 2;
    else
      update_interval = 1;
#else
    update_interval = 0;
#endif
    
    current_interval = update_interval;

    com_time += end_time.time*1000 + end_time.millitm - start_time.time*1000 - start_time.millitm;
  }
  else
    update_interval--;
}


void load_BGP_AS_info()
{
  FILE* infile;
  int i, i1, i2;
  char buf[256];

  infile = fopen("BGPAtom2AS.dat", "r");
  if (infile == NULL) {
    printf("Error: Failed to open the file BGPAtom2AS.dat\n");
    exit(1);
  }
  fgets(buf, 256, infile);
  BGPatom2AS = (int*)malloc(sizeof(int) * (BGP_ATOM_NUMBER + 1000));
  for (i = 0; i < BGP_ATOM_NUMBER; i++) {
    fgets(buf, 256, infile);
    sscanf(buf, "%d\t%d", &i1, &i2);
    assert(i == i1);
    BGPatom2AS[i] = i2;
  }
  fclose(infile);
}


int possible_bw[8] = {10000, 51840, 100000, 155520, 466560, 622080, 1244160, 2488320};
void load_bandwidth()
{
  char buf[256];
  register int i;
  int a1, a2, a3, a4;
  FILE *infile;

  link_List = (struct Inter_AS_Link*)malloc(sizeof(struct Inter_AS_Link) * LINK_NUMBER);

  if ((infile = fopen("ASLinks.dat", "r")) == NULL) {
    printf("Error: failed to open the file ASLinks.dat\n");
    exit(1);
  }

  for (i = 0; i < LINK_NUMBER; i++) {
    fgets(buf, 256, infile);
    sscanf(buf, "%d\t%d\t%d\t%d", &a1, &a2, &a3, &a4);
    assert(i == a1);

    link_List[i].link_bandwidth = a4/8;
    link_List[i].link_background_traffic = 1;
    link_List[i].link_head = a2;
    link_List[i].link_tail = a3;
    link_List[i].pass_ratio = 1;
    link_List[i].path_index = NULL;
    link_List[i].path_counter = 0;
    link_List[i].path_capacity = 0;
  }
  fclose(infile);


  printf("Bandwidth definition file ASLinks.dat loaded successfully.\n");
}

///////////////////////////////////////////////////////////////////////////////////////////
#ifdef BACKGROUND_TRAFFIC
void load_traffic_to_link(struct Inter_AS_Link *link, int traffic_volumn, int stream_index)
{
  link->link_background_traffic += traffic_volumn;

  if (link->path_counter >= link->path_capacity) {
    if (link->path_capacity == 0) {
      link->path_capacity = 64;
      link->path_index = (u_int*)malloc(sizeof(u_int) * link->path_capacity);
    }
    else {
      link->path_capacity<<1;
      link->path_index = (u_int*)realloc(link->path_index, sizeof(u_int) * link->path_capacity);
    }

    link->path_index[link->path_counter] = stream_index;
    link->path_counter++;
  }
}

void trace_route(int src_as, int des_as, int des_atom, int traffic_volumn, int stream_index)
{
  register int i, j, k;

  while (1) {
    if (src_as == des_as)
      break;

    i = 0;
    j = route_path[src_as].entry_count-1;
    while (i < j) {
      k = (i+j)/2;
      if (des_atom <= route_path[src_as].BGP_index[k])
	j = k;
      else
	i = k+1;
    }

    assert(i == j);
    assert(des_atom <= route_path[src_as].BGP_index[i]);

    j = route_path[src_as].next_AS[i];

    if (j < 0) {
      printf("Unreachable destination: AS%d->Atom%d in trace_route()\n", src_as, des_atom);
      return;
    }

    load_traffic_to_link(&(link_List[j]), traffic_volumn, stream_index);

    assert(link_List[j].link_head == src_as || link_List[j].link_tail == src_as);
    if (link_List[j].link_head != src_as)
      src_as = link_List[j].link_head;
    else
      src_as = link_List[j].link_tail;
  }
}

void load_background_traffic()
{
  int register i, j, k, n;
  FILE *infile;
  char buf[256];
  int as1, as2, traffic_volume;

  if ((infile = fopen("backgroundTraffic.dat", "r")) == NULL) {
    printf("Error: failed to open backgroundTraffic.dat\n");
    exit(1);
  }

  stream_list = (struct AS_Stream*)malloc(sizeof(struct AS_Stream) * LEGITIMATE_STREAM_NUMBER);
  n = 0;

  while (!feof(infile)) {
    fgets(buf, 256, infile);
    if (strlen(buf) <= 1)
      continue;

    sscanf(buf, "%d\t%d\t%d", &as1, &as2, &traffic_volume);
    if (traffic_volume < 1000)
      traffic_volume = 1000;
			
    traffic_volume = traffic_volume/1000;

    i = 0;
    j = BGP_ATOM_NUMBER-1;
    while (i <= j) {
      k = (i+j)/2;
      if (BGPatom2AS[k] < as2)
	i = k + 1;
      else
	if (BGPatom2AS[k] > as2)
	  j = k - 1;
	else
	  break;
    }
    if (i > j) {
      printf("Error in load_background_traffic()*** %d %d %d %d ***\n", i, j, as1, as2);
      exit(1);
    }
    assert(i <= j);
    assert(BGPatom2AS[k] == as2);

    stream_list[n].head_as = as1;
    stream_list[n].tail_as = k;
    stream_list[n].desired_traffic = traffic_volume;
    stream_list[n].current_traffic = traffic_volume;
    trace_route(as1, as2, k, traffic_volume, n);
    n++;
  }

  total_background_traffic = 0;	
  for (i = 0; i < LINK_NUMBER; i++) {
    total_background_traffic += 1 + link_List[i].link_background_traffic;
    if (link_List[i].link_bandwidth <= link_List[i].link_background_traffic) {
      printf("Leg. traffic is too big on link %d: %u %u\n", i, link_List[i].link_bandwidth, link_List[i].link_background_traffic);
//      link_List[i].link_background_traffic = link_List[i].link_bandwidth;
    }
  }
}
#endif
/////////////////////////////////////////////////////////////////////



//begin*********************************Test
//accummulate the routing path numbers on each link between src_as and des_as
void test_trace_route(int src_as, int des_as, int des_atom, double product, double* link_weight)
{
  register int i, j, k;

  while (1) {
    if (src_as == des_as)
      break;

    i = 0;
    j = route_path[src_as].entry_count-1;
    while (i < j) {
      k = (i+j)/2;
      if (des_atom <= route_path[src_as].BGP_index[k])
	j = k;
      else
	i = k+1;
    }

    assert(i == j);
    assert(des_atom <= route_path[src_as].BGP_index[i]);

    j = route_path[src_as].next_AS[i];

    if (j < 0) {
      printf("Unreachable destination: AS%d->Atom%d\n", src_as, des_atom);
      return;
    }

    link_weight[j] += product;

    assert(link_List[j].link_head == src_as || link_List[j].link_tail == src_as);
    if (link_List[j].link_head != src_as)
      src_as = link_List[j].link_head;
    else
      src_as = link_List[j].link_tail;
  }
}

//test all the src-dst pair and accummulate the link weight which is defined
//as the number of routing paths passing on this link
void test_background_traffic()
{
  int register src_as, dst_as, i, j, k;
  FILE *infile;
  char buf[256];
  int as1, as2, ip_number;
  double *as_size;
  double * link_weight;


  if ((infile = fopen("sortedASbySize.dat", "r")) == NULL) {
    perror("Error: failed to open sortedASbySize.dat\n");
    exit(1);
  }
  
  as_size = (double*)malloc(sizeof(double) * AS_NUMBER);
  memset(as_size, 0, sizeof(double) * AS_NUMBER);

  link_weight = (double*)malloc(sizeof(double) * LINK_NUMBER);
  memset(link_weight, 0, sizeof(double) * LINK_NUMBER);

  while (!feof(infile)) {
    fgets(buf, 256, infile);
    if (strlen(buf) <= 1)
      continue;

    sscanf(buf, "%d\t%d\t%d", &as1, &as2, &ip_number);
    if (as1 < AS_NUMBER)
      as_size[as1] = ip_number;
  }

  for (src_as = 0; src_as < AS_NUMBER; src_as++) {
    for (dst_as = 0; dst_as < AS_NUMBER; dst_as++) {
      if (src_as == dst_as)
	continue;
      
      i = 0;
      j = BGP_ATOM_NUMBER-1;
      while (i <= j) {
	k = (i+j)/2;
	if (BGPatom2AS[k] < dst_as)
	  i = k + 1;
	else
	  if (BGPatom2AS[k] > dst_as)
	    j = k - 1;
	  else
	    break;
      }
      if (i > j) {
	//printf("Error in test_background_traffic()*** %d %d (%d->%d) ***\n", i, j, src_as, dst_as);
	//printf("%d->%d\n", src_as, dst_as);
	continue;
      }

      test_trace_route(src_as, dst_as, k, as_size[src_as]*as_size[dst_as], link_weight);
    }
  }

  for (i = 0; i < LINK_NUMBER; i++)
    printf("Link\t%d\t%f\t%d\n", i, link_weight[i], link_List[i].link_bandwidth);	
}
//end************************************Test


void load_RT_file()
{
  char buf[256], *ptr;
  register int i, j, n, k;
  int i1, i2, i3;
  FILE *infile;
  int *path, *index;

  printf("Loading route paths from file paws_RT.dat ....\n");

  route_path= (struct Routing_Path_Type*)malloc(sizeof(struct Routing_Path_Type) * AS_NUMBER);
  path = (int*)malloc(sizeof(int) * BGP_ATOM_NUMBER);
  index = (int*)malloc(sizeof(int) * BGP_ATOM_NUMBER);

  infile = fopen("/mnt/local/paws_RT.dat", "r");
  if (infile == NULL) {
    printf("Error: Failed to open the file paws_RT.dat\n");
    exit(1);
  }

  n = 0;
  while (!feof(infile)) {
    fgets(buf, 256, infile);
    n++;

    if (strlen(buf) <= 2)
      continue;
    
    if ((ptr = strstr(buf, "AS")) == NULL)
      continue;

    ptr += 3;
    i = atoi(ptr);
    j = 0;
    while (1) {
      fgets(buf, 256, infile);
      n++;
      sscanf(buf, "%d\t%d", &i1, &i2);
      index[j] = i1;
      path[j] = i2;
      j++;
      if (i1 >= BGP_ATOM_NUMBER - 1)
	break;
    }
    route_path[i].BGP_index = (int*)malloc(sizeof(int)*j);
    route_path[i].next_AS = (int*)malloc(sizeof(int)*j);
    route_path[i].entry_count = j;
    memcpy(route_path[i].BGP_index, index, sizeof(int)*j);
    memcpy(route_path[i].next_AS, path, sizeof(int)*j);
  }

  fclose(infile);
  free(path);
  free(index);
  printf("%d lines in the routing file are processed\n", n);
  printf("Route paths loaded successfully ...\n");
}


//Sort IPs of all the vulnerable hosts so that binary search can be used later
void sort_vulnerable_ip()
{
	 register int i, j, k;
	 register int min_index, atom_size;
	 register u_int min;

	for (i = 0; i < total_atom_number; i++) {
  		atom_size = BGP_atom_list[i].vulnerable.size;
  		for (j = 0; j < atom_size - 1; j++) {
    			min = BGP_atom_list[i].vulnerable.ip[j];
    			min_index = j;
    			for (k = j + 1; k < atom_size; k++)
      			if (BGP_atom_list[i].vulnerable.ip[k] < min) {
					min = BGP_atom_list[i].vulnerable.ip[k];
					min_index = k;
      			}
    			min = BGP_atom_list[i].vulnerable.ip[min_index];
    			BGP_atom_list[i].vulnerable.ip[min_index] = BGP_atom_list[i].vulnerable.ip[j];
    			BGP_atom_list[i].vulnerable.ip[j] = min;
    		}
  	}
}

double max_avg(uint* data, uint size)
{
  uint i, j, k;
  uint max_data[3];

  for (i = 0; i < 3; i++) {
    max_data[i] = 0;
    k = 0;
    for (j = 0; j < size; j++)
      if (data[j] > max_data[i]) {
	max_data[i] = data[j];
	k = j;
      }
    data[k] = 0;
  }

  return (max_data[0] + max_data[1] + max_data[2])/3.0;
}

double min_avg(uint* data, uint size)
{
  uint i, j, k;
  uint min_data[3];

  for (i = 0; i < 3; i++) {
    min_data[i] = 0x7FFFFFFF;
    k = 0;
    for (j = 0; j < size; j++)
      if (data[j] > 0 && data[j] < min_data[i]) {
	min_data[i] = data[j];
	k = j;
      }
    if (min_data[i] >= 0x7FFFFFFF)
      min_data[i] = 0;
    data[k] = 0x7FFFFFFF;
  }

  return (min_data[0] + min_data[1] + min_data[2])/3.0;
}



int main(int argc, char** argv)
{
  register u_int i, j, k, r, m, n;
  u_int uns;
  FILE *infile, *fp;
  char buf1[256], buf2[256], str[64];
  char *host_name;
  struct timeb start_t, end_t;
  u_int *as_size;
  int i1, i2;
  char output[1035];


  printf("--------------* Parallel Worm Simulator v5 (Feb 2009) *--------------\n");
  
  //Get the host id
  fp = popen("hostname", "r");
  if (fp == NULL) {
    printf("Failed to run command hostname\n" );
    exit;
  }

  /* Read the output a line at a time - output it. */
  fgets(output, sizeof(output)-1, fp);
  host_name = strtok(output, ".");
  host_id = atoi(host_name+5);
  printf("Slave %d  starts...\n", host_id);

  /*Set up connection with all the other simulation nodes*/
  setup_connection();
  
  /*Initialize the host buffer and socket buffers*/
  for (i = 0; i < NODE_NUMBER; i++) {
    host_buf_array[i].size = 0;
    host_buf_array[i].volume = 0x2000;
    host_buf_array[i].ip = (u_int*)malloc(sizeof(u_int) * host_buf_array[i].volume * 3);
  }
  host_buf.size = 0;
  host_buf.volume = 256;
  host_buf.ip = (u_int*)malloc(sizeof(u_int) * host_buf.volume * 3);

  /*Compute the vulnerable ratio*/
  printf("_____________________________________________________\n");
  vulnerable_ratio = VULNERABLE_NUMBER * 1.1 / SPACE_SIZE;
  printf("Vulnerable ratio = %.9f = %u/%u\n", vulnerable_ratio, VULNERABLE_NUMBER, SPACE_SIZE);

  /*Loading all the BGP atoms and determining the vulnerable hosts inside each atom*/
  BGP_atom_list = malloc(sizeof(struct BGP_Atom_Type) * (BGP_ATOM_NUMBER + 1000)); //1000 extra space is used to split large atoms
  for (i = 0; i < BGP_ATOM_NUMBER + 1000; i++) {
    BGP_atom_list[i].BGP_atom_id = i;
    BGP_atom_list[i].size = 0;
    BGP_atom_list[i].volume = 0;
    BGP_atom_list[i].ips = 0;
    BGP_atom_list[i].host_machine = -1;
    BGP_atom_list[i].vulnerable.size = 0;
    BGP_atom_list[i].vulnerable.volume = 0;
    BGP_atom_list[i].parent_atom = -1;
  }
  total_atom_number = BGP_ATOM_NUMBER;

  /*Determine the vulnerable hosts inside each atom*/
  load_BGP_AS_info();
  vulnerable_number = 0;
#ifdef WITTY_WORM
  //determine_vulnerable_by_AS("as_witty_vulnerable_ratio.txt");
#else
  //determine_vulnerable_by_AS("as_slammer_vulnerable_ratio.txt");
  determine_vulnerable();
#endif
  for (i = BGP_ATOM_NUMBER; i< total_atom_number; i++)
    BGPatom2AS[i] = BGPatom2AS[BGP_atom_list[i].parent_atom];

  /*Distributing BGP atoms onto multiple machines*/
  for (i = 0; i < NODE_NUMBER; i++) {
    machine_list[i].machine_id = i+1;
    machine_list[i].BGP_atom_number = 0;
    machine_list[i].IP_number = 0;
    machine_list[i].vulnerable_number = 0;
    machine_list[i].machine_atom_list = (struct BGP_Atom_Type**)malloc(sizeof (struct BGP_Atom_Type*) * 
								       (total_atom_number/ NODE_NUMBER + 1));
  }
  BGPatom2machine = malloc(sizeof(int) * total_atom_number);
  for (i = 0; i < total_atom_number; i++) {
    r = 0; //Atom id
    k = 0; //number of vulnerables inside
    //find the largest atom currently not assigned
    for (j = 0; j < total_atom_number; j++)
      if (BGP_atom_list[j].host_machine < 0 && BGP_atom_list[j].vulnerable.size >= k) {
	r = j;
	k = BGP_atom_list[j].vulnerable.size;
      }
    k = i % NODE_NUMBER;
    BGPatom2machine[r] = k;
    BGP_atom_list[r].host_machine = k;
    machine_list[k].machine_atom_list[machine_list[k].BGP_atom_number] = &(BGP_atom_list[r]);
    machine_list[k].BGP_atom_number++;
    machine_list[k].IP_number += BGP_atom_list[r].ips;
    machine_list[k].vulnerable_number += BGP_atom_list[r].vulnerable.size;
  }

  /*check the distribution on multiple machines*/
  m = 0; //vulnerable number
  n = 0; //ip number
  for (i = 0; i < NODE_NUMBER; i++) {
    m += machine_list[i].vulnerable_number;
    n += machine_list[i].IP_number;
    printf("%u vulnerables %u IPs %u atoms on machine %d\n", 
	   machine_list[i].vulnerable_number, 
	   machine_list[i].IP_number, 
	   machine_list[i].BGP_atom_number, 
	   i+1);
  }
  if (SPACE_SIZE != n || vulnerable_number != m) {
    printf("Error: inconsistent IP space size \t<%u/%u>\n", n, SPACE_SIZE);
    exit(1);
  }
  printf("Totally %u IPs in %u ranges within %d BGP atoms\n", SPACE_SIZE, ip_range_number, total_atom_number);

  /*Sorting the vulnerables inside each atom by ip addresses*/
  printf("Sorting vulnerable IPs....\n");
  sort_vulnerable_ip();

  /*Accummulate the atom size along the atom array*/
  for (i = 1; i < total_atom_number; i++) {
    if (BGP_atom_list[i].ips <= 0)
      printf("Empty BGP atom %u\n", i);
    BGP_atom_list[i].ips += BGP_atom_list[i-1].ips;
  }

  /*Accummulate the ip range size inside each atom*/
  for (i = 0; i < total_atom_number; i++)
    for (j = 1; j < BGP_atom_list[i].size; j++)
      BGP_atom_list[i].table[j*2+1] += BGP_atom_list[i].table[j*2-1];

  /*Initialize the traffic buffers*/
  current_Link_Traffic = malloc(TRAFFIC_BUFFER_SIZE);
  new_Link_Traffic = malloc(TRAFFIC_BUFFER_SIZE);
  traffic_Buffer = malloc(TRAFFIC_BUFFER_SIZE);
  memset(current_Link_Traffic, 0, TRAFFIC_BUFFER_SIZE);

  /*Load the routing and bandwidth data*/
  printf("Loading bandwidth files......\n");
  load_bandwidth();
  printf("Loading routing files......\n");
  load_RT_file();

  /*Load the background traffic*/
#ifdef BACKGROUND_TRAFFIC
  printf("Loading background traffic......\n");
  load_background_traffic();
#endif

  /*Load the worm propagation duration, for witty worm only*/
#ifdef WITTY_WORM
  infile = fopen("infectedtime_duration.txt", "r");
  if (infile == NULL) {
  	printf("Error: failed to open the file infected_duration.txt for witty worm simulation\n");
  	exit(1);
  }
  worm_duration_list = (int*)malloc(sizeof(int) * WITTY_SPREAD_TIME);
  for (i = 0; i < WITTY_SPREAD_TIME; i++) {
  	fgets(buf1, 256, infile);
  	sscanf(buf1, "%i\t%i\n", &i1, &i2);
  	assert(i == i1);
  	worm_duration_list[i] = i2;
  }
  fclose(infile);
#endif

  /*Initialize the worm-scan buffer*/
  buffered_scan.buffer_capacity = 0;
  buffered_scan.scan_number = 0;
  buffered_scan.scan_list_head = 0;
  buffered_scan.scan_list_tail = 0;

  //Initialize the RNG
  j = time(NULL) + host_id * 1000;
  srand(j);
  printf("Random Seed: %d\n", j);
  //srand(1087427255+host_id); //this make the simulation repeatable, for debugging only


  /*initialization for time unit scalling*/
  update_interval = 0;

  /*statistics*/
  com_time = 0;	//inter-machine communication time

  /*for TCP retransmission simulation*/
  node_infected_number = 0;
  scanning_stop_second = 0; //used for TCP worm for retransmission


 //Start simulation
  ftime(&start_t); //simulation start time
  printf("_____________________________________________________\n");
  printf("Time\tInf\tUpdate\tDropped\tPassed\tBuffered_scans\n");
  i = 0;
  while (i < SIMULATION_DURATION) {
    worm_infectee_scan(i);
    i++;
  }
  ftime(&end_t);
  j = end_t.time*1000 + end_t.millitm - start_t.time*1000 - start_t.millitm;
  printf("Real time used on machine <%d>: %dh%dm%d\n", host_id, j/3600000, j%3600000/60000, j%60000/1000);
  printf("Total time used for communication: %dm%ds\n", com_time/60000, com_time/60000%1000);
  fflush(stdout);
}
