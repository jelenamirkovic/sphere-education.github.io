#define	NODE_NUMBER 8

#define SIMULATION_DURATION 100

int host_id;
char host_name[256];

#define	TCP_PROTOCOL 0
#define	UDP_PROTOCOL 1


/*CodeRed*/
/*
#define	SPACE_SIZE	0x4A7838B6	//1249392822	//size of IP space
#define	AS_NUMBER	12680	//number of ASes
#define	BGP_ATOM_NUMBER 17628
#define	LINK_NUMBER 26367 //inter-AS links

#define	VULNERABLE_NUMBER	360000	//vulnerable population
#define	SCAN_RATE	3.8	//11 per second //scans per second
#define	INFECTION_TIME	1	//infection time between [1, 255)
#define	WORM_SIZE	40	//size of the worm packet
#define	TRANSPORT_PROTOCOL	TCP_PROTOCOL	//Code Red is a TCP worm
*/


/*Slammer*/
#ifdef SLAMMER_WORM
#define	SPACE_SIZE	0x4C089406	//1275630598
#define	AS_NUMBER	15742
#define	BGP_ATOM_NUMBER 21062
#define	LINK_NUMBER	32428

#define	VULNERABLE_NUMBER 76000 //vulnerable population
#define	SCAN_RATE 4000 //scans per second
#define	INFECTION_TIME 1 //infection time  between [1, 255)
#define	WORM_SIZE 404
#define	TRANSPORT_PROTOCOL UDP_PROTOCOL //Slammer is a UDP worm
#define	LEGITIMATE_STREAM_NUMBER 341484 //number of inter-AS streams
#endif


/*Witty*/
#ifdef WITTY_WORM
#define	SPACE_SIZE	0x4C089406	//1275630598
#define	AS_NUMBER	15742
#define	BGP_ATOM_NUMBER 21062
#define	LINK_NUMBER	32428

#define VULNERABLE_NUMBER 15566 //11973 //15566 //32341
#define SCAN_RATE 0
#define WORM_SIZE 1070
#define TRANSPORT_PROTOCOL UDP_PROTOCOL
#define LEGITIMATE_STREAM_NUMBER 341484
#define WITTY_SPREAD_TIME 7440
int *worm_duration_list; //index is the infection time, the value is propagation duration in second
#endif

/*other worm*/
#ifdef OTHER_WORM
#define	SPACE_SIZE	0x4C089406	//1275630598
#define	AS_NUMBER	15742
#define	BGP_ATOM_NUMBER 21062
#define	LINK_NUMBER	32428

#define VULNERABLE_NUMBER 72000 //11973 //15566 //32341
#define SCAN_RATE 4300
#define WORM_SIZE 1070
#define TRANSPORT_PROTOCOL UDP_PROTOCOL
#define LEGITIMATE_STREAM_NUMBER 341484
#endif

#define DEFAULT_PROPAGATION_DURATION -1




int sock_list[NODE_NUMBER+1];
int sock_master;

#define	PAWS_PORT 10001
#define	MAX_SLAVE 48
#define	SEND_BLOCK_SIZE 257
#define	RECV_BLOCK_SIZE 512

#define REPORT_INTERVAL 1

#define	TRAFFIC_BLOCK_SIZE 1024
#define	TRAFFIC_BUFFER_SIZE sizeof(u_int)*LINK_NUMBER




struct ip_buf {
  int size;
  int volume;
  u_int *ip;
};

int vulnerable_number;
double vulnerable_ratio;
int ip_range_number;
int total_atom_number;

int scanning_stop_second; //This value is used to maintain the scanning rate slowed down due to buffered scan
int node_infected_number; //number of currently infected hosts on this simulation node

struct ip_buf host_buf;
struct ip_buf host_buf_array[NODE_NUMBER];

struct Worm_Vulnerable_Host {
  u_int scan_rate;
  double bandwidth; //kbps
  u_int worm_index;
  u_int prng_seed;
  int propagation_duration; //-1 means forever
  int quarantine;
};

struct BGP_Atom_Vulnerable {
  int size;
  int volume;
  u_int *ip;
  u_int *status;
  struct Worm_Vulnerable_Host *host_config; //configuration of the vulnerable host
};

struct BGP_Atom_Type {
  int BGP_atom_id;

  int parent_atom; //Pointer to the parent atom of a fake atome, negative if non fake

  //IP ranges
  u_int *table;
  int size;
  int volume;
  u_int ips;

  struct BGP_Atom_Vulnerable vulnerable; //Vulnerable hosts

  int host_machine;
};

struct Machine_Type {
  int machine_id;
  struct BGP_Atom_Type **machine_atom_list;
  int BGP_atom_number;
  u_int IP_number;
  u_int vulnerable_number;
};


int *BGPatom2machine;
int *BGPatom2AS;

//For retransmission of worm scans
#define INIT_SCAN_BUFFER_CAPACITY	1024
struct Worm_Scan_Buffer {
  int scan_list_head;
  int scan_list_tail;
  int scan_number;
  int buffer_capacity;

  int *scan_src_as;
  int *scan_des_atom;
  u_int *scan_des_ip;
  int *scan_scheduled_time;
};
struct Worm_Scan_Buffer buffered_scan;


struct Machine_Type machine_list[NODE_NUMBER];
struct BGP_Atom_Type *BGP_atom_list;


//For simulating congestions
u_int *current_Link_Traffic;
u_int *new_Link_Traffic;
u_int *traffic_Buffer;
u_int *left_Link_Traffic;
u_int *right_Link_Traffic;
double total_background_traffic;

struct AS_Stream {
  u_short head_as;
  u_short tail_as;
  u_int desired_traffic;
  u_int current_traffic;
};

struct AS_Stream *stream_list;

struct Inter_AS_Link {
  int link_head;
  int link_tail;
  u_int link_bandwidth;
  u_int link_background_traffic;
  double pass_ratio;

  u_int *path_index;
  int path_counter;
  int path_capacity;
  
  int head_degree;
  int tail_degree;
};

struct Inter_AS_Link *link_List;

int current_interval;
int update_interval;

struct Routing_Path_Type {
  int *next_AS;
  int *BGP_index;
  int entry_count;
};

struct Routing_Path_Type *route_path;


/*******************************************************************************/
void setup_connection() {
  struct addrinfo* res[NODE_NUMBER+1], hints;
  struct addrinfo from;
  int len;


  int i, j, yes;
  unsigned char msg[64];


  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;

  for (i = 1; i < host_id; i++) {
    char service[20];
    char hostname[20];

    int err;

    sprintf(hostname, "node-%d", i);
    sprintf(service, "%d", PAWS_PORT+i);

    if ((err = getaddrinfo(hostname, service, &hints, &res[i])) != 0) {
      printf("error %d\n", err);
      return 1;
    }

    sock_list[i] = socket(res[i]->ai_family, res[i]->ai_socktype, res[i]->ai_protocol);
    if (sock_list[i] < 0) {
      printf("socket\n");
      return 1;
    }
  }


  {
    struct addrinfo *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char service[20];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    sprintf(service, "%d", PAWS_PORT+i);    

    if ((rv = getaddrinfo(NULL, service, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sock_list[host_id] = socket(p->ai_family, p->ai_socktype,
				       p->ai_protocol)) == -1) {
	perror("server: socket");
	continue;
      }
      
      if (setsockopt(sock_list[host_id], SOL_SOCKET, SO_REUSEADDR, &yes,
		     sizeof(int)) == -1) {
	perror("setsockopt");
	exit(1);
      }
      
      if (bind(sock_list[host_id], p->ai_addr, p->ai_addrlen) == -1) {
	close(sock_list[host_id]);
	perror("server: bind");
	continue;
      }
      
      break;
    }
  
    if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n");
      return 2;
    }
    
    freeaddrinfo(servinfo); // all done with this structure}
  }

  for (i=1; i<host_id;i++)
    {
      if (connect(sock_list[i], res[i]->ai_addr, res[i]->ai_addrlen)<0) {
	perror("Connecting to other slave node fails");
	exit(1);
      }
      else
	fprintf(stderr, "%d connects to %d\n", host_id, i);
    }

  for (i = host_id+1; i <= NODE_NUMBER; i++) {
    if (listen(sock_list[host_id], MAX_SLAVE) < 0) {
      perror("Listening to other slave nodes fails:");
      exit(1);
    }

    j = sizeof(struct sockaddr_in);
    
    if ((sock_list[i] = accept(sock_list[host_id], &from, &len)) < 0) {
      perror("Accepting connections fails:");
      exit(1);
    }
    else
      printf("%d connects from %d\n", host_id, i);
  }

  for (i = 1; i <= NODE_NUMBER; i++) {
    if (i < host_id) {
      sprintf((char*)msg, "Parallel Worm Simulator Node %d", i);
      j = send(sock_list[i], msg, strlen((char*)msg), 0);
      printf("Send <%s> to %d\n", msg, i);
    }
    else
      if (i > host_id) {
	j = recv(sock_list[i], msg, 63, 0);
	msg[j] = 0;
	printf("Receive <%s> from %d\n", msg, i);
      }
  }

  if ((sock_master = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socketing for master fails:");
    exit(1);
  }

  char service[20];
  char hostname[20];
  struct addrinfo* result;

  sprintf(hostname, "node-0", i);
  sprintf(service, "%d", PAWS_PORT);

  getaddrinfo(hostname, service, NULL, &result);
  
  if (connect(sock_master, result->ai_addr, result->ai_addrlen) < 0) {
    perror("Connecting to master fails");
      exit(1);
    }
    else
      fprintf(stderr, "%d connects to the master\n", host_id);

  if (send(sock_master, &host_id, 4, 0) != 4) {
    printf("Failed to send master the node_id\n");
    exit(1);
  }

  //Receiving rand_seed from master and initialize the generator
  if (recv(sock_master, msg, 63, 0) != 4) {
    printf("Invalid rand_seed value from master\n");
    exit(1);
  }
  memcpy(&i, msg, 4);
  printf("Seed = %d\n", i);
  srand(i);
}
