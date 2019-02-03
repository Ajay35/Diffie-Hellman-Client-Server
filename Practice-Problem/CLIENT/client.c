/*****************************************************************************
 *** Program explaining the client-server model                            ***
 *** developed by Ashok Kumar Das, CSE Department, IIT Kharagpur           ***
 ***                                                                       ***
 *****************************************************************************/

/****************************************************************************
Problem: User A (client) sends the request message REQ to the user B (server).
In response, user B (server) replies the response message REP to the user A
(client).
REQ contains:
1. message header
2. integer x
3. integer y
4. integer check1 = x AND y
5. integer check2 = x XOR y

REP contains:
1. message header
2. integer status: 1 (SUCCESS) and 0 (FAIL)
if both check1 and check2 are valid, then return 1;
else return 0.
*******************************************************************************/ 


#include <stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

/* Global constants */
#define SERVICE_PORT 41041
#define MAX_SIZE 20
#define MAX_LEN 1024
#define SUCCESS 1
#define FAIL 0


#define DEFAULT_SERVER "192.168.1.241"

#define REQ 10  /* Request message */
#define REP 20  /* Reply message */

/* Define a message structure */
typedef struct {
 int opcode;
 int src_addr;
 int dest_addr;
 } Hdr;

/* REQ message */
typedef struct {
 Hdr hdr;
 int x;
 int y;
 int check1; /* x AND y */
 int check2; /* x XOR y */
} ReqMsg;

/* REP message */
typedef struct {
 Hdr hdr;
 int status;
} RepMsg;

/* Function prototypes */
int serverConnect ( char * );
void Talk_to_server ( int );

/* Connect with the server: socket() and connect() */
int serverConnect ( char *sip )
{
   int cfd;
   struct sockaddr_in saddr;   /* address of server */
   int status;

   /* request for a socket descriptor */
   cfd = socket (AF_INET, SOCK_STREAM, 0);
   if (cfd == -1) {
      fprintf (stderr, "*** Client error: unable to get socket descriptor\n");
      exit(1);
   }

   /* set server address */
   saddr.sin_family = AF_INET;              /* Default value for most applications */
   saddr.sin_port = htons(SERVICE_PORT);    /* Service port in network byte order */
   saddr.sin_addr.s_addr = inet_addr(sip);  /* Convert server's IP to short int */
   bzero(&(saddr.sin_zero),8);              /* zero the rest of the structure */

   /* set up connection with the server */
   status = connect(cfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Client error: unable to connect to server\n");
      exit(1);
   }

   fprintf(stderr, "Connected to server\n");

   return cfd;
}

/* Interaction with the server */
void Talk_to_server ( int cfd )
{
   char buffer[MAX_LEN];
   int nbytes, status;
   int src_addr, dest_addr;
   ReqMsg send_msg;
   RepMsg recv_msg;

   dest_addr = inet_addr("DEFAULT_SERVER");
   src_addr = inet_addr("192.168.1.245");

   /* send the request message REQ to the server */
   printf("Sending the request message REQ to the server\n");          
   send_msg.hdr.opcode = REQ;
   send_msg.hdr.src_addr = src_addr;
   send_msg.hdr.dest_addr = dest_addr;
   
   srand((unsigned int) time(NULL));
   send_msg.x = random() %  MAX_SIZE;
   send_msg.y = random() %  MAX_SIZE;
   send_msg.check1 = send_msg.x & send_msg.y;
   send_msg.check2 = send_msg.x ^ send_msg.y;

   status = send(cfd, &send_msg, sizeof(ReqMsg), 0);
   if (status == -1) {
      fprintf(stderr, "*** Server error: unable to send\n");
      return;
    }

  while (1) {
  /* receive greetings from server */
   nbytes = recv(cfd, &recv_msg, sizeof(RepMsg), 0);
   if (nbytes == -1) {
      fprintf(stderr, "*** Client error: unable to receive\n");
      
   }
   switch ( recv_msg.hdr.opcode ) {
    
   case REP : printf("Message:: with opcode %d (REP) received from source (%d)\n", 
                      recv_msg.hdr.opcode, recv_msg.hdr.src_addr);  
              /* Check the status of REP message */
              if (recv_msg.status) 
                printf("Message REQ has received successfully by the server\n");
              else    
               printf("Message REQ has NOT received successfully by the server\n");
              break;
   default: 
           printf("message received with opcode: %d\n", recv_msg.hdr.opcode);
           exit(0);  
   }
 }
}

int main ( int argc, char *argv[] )
{
   char sip[16];
   int cfd;
   

   printf("******* This is demo program using sockets ***** \n\n");
   
   strcpy(sip, (argc == 2) ? argv[1] : DEFAULT_SERVER);
   cfd = serverConnect(sip);
   Talk_to_server (cfd);
   close(cfd);
}

/*** End of client.c ***/
