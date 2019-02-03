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
#define Q_SIZE 5

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
int startServer ( );
void Talk_to_client ( int );
void serverLoop ( int );

/* Start the server: socket(), bind() and listen() */
int startServer ()
{
   int sfd;                    /* for listening to port PORT_NUMBER */
   struct sockaddr_in saddr;   /* address of server */
   int status;


   /* Request for a socket descriptor */
   sfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sfd == -1) {
      fprintf(stderr, "*** Server error: unable to get socket descriptor\n");
      exit(1);
   }

   /* Set the fields of server's internet address structure */
   saddr.sin_family = AF_INET;            /* Default value for most applications */
   saddr.sin_port = htons(SERVICE_PORT);  /* Service port in network byte order */
   saddr.sin_addr.s_addr = INADDR_ANY;    /* Server's local address: 0.0.0.0 
                                             (htons not necessary) */
   bzero(&(saddr.sin_zero),8);            /* zero the rest of the structure */

   /* Bind the socket to SERVICE_PORT for listening */
   status = bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Server error: unable to bind to port %d\n", SERVICE_PORT);
      exit(2);
   }

   /* Now listen to the service port */
   status = listen(sfd,Q_SIZE);
   if (status == -1) {
      fprintf(stderr, "*** Server error: unable to listen\n");
      exit(3);
   }

   fprintf(stderr, "+++ Server successfully started, listening to port %hd\n", SERVICE_PORT);
   return sfd;
}


/* Accept connections from clients, spawn a child process for each request */
void serverLoop ( int sfd )
{
   int cfd;                    /* for communication with clients */
   struct sockaddr_in caddr;   /* address of client */
   int size;


    while (1) {
      /* accept connection from clients */
      cfd = accept(sfd, (struct sockaddr *)&caddr, &size);
      if (cfd == -1) {
         fprintf(stderr, "*** Server error: unable to accept request\n");
         continue;
      }

     printf("**** Connected with %s\n", inet_ntoa(caddr.sin_addr));
     
      /* fork a child to process request from client */
      if (!fork()) {
         Talk_to_client (cfd);
         fprintf(stderr, "**** Closed connection with %s\n", inet_ntoa(caddr.sin_addr));
         close(cfd);
         exit(0);
      }

      /* parent (server) does not talk with clients */
      close(cfd);

      /* parent waits for termination of child processes */
      while (waitpid(-1,NULL,WNOHANG) > 0);
   }
}


/* Interaction of the child process with the client */
void Talk_to_client ( int cfd )
{
   int status;
   int nbytes;
   int src_addr, dest_addr;
   int chk1, chk2; 
   RepMsg send_msg;
   ReqMsg recv_msg;


   dest_addr = inet_addr("192.168.1.245");
   src_addr = inet_addr("DEFAULT_SERVER");
 
   while (1) {
   /* Receive response from server */
   nbytes = recv(cfd, &recv_msg, sizeof(ReqMsg), 0);
   if (nbytes == -1) {
      fprintf(stderr, "*** Server error: unable to receive\n");
      return;
   }
   
   switch ( recv_msg.hdr.opcode ) {
    
   case REQ : /* Request message */
              printf("Message:: with opcode %d (REQ) received from source (%d)\n", 
                      recv_msg.hdr.opcode, recv_msg.hdr.src_addr);  
              send_msg.hdr.opcode = REP;
              send_msg.hdr.src_addr = src_addr;        
              send_msg.hdr.dest_addr = dest_addr;  
              printf("Received values in REQ message are: \n");
              printf("x = %d\n", recv_msg.x);
              printf("y = %d\n", recv_msg.y);
              printf("check1 = %d\n", recv_msg.check1);
              printf("check2 = %d\n", recv_msg.check2);

              /* recompute check1 and check2 on x and y in the received meesage REQ */
              chk1 = recv_msg.x & recv_msg.y;
              chk2 = recv_msg.x ^ recv_msg.y;
              if ( (chk1 != recv_msg.check1) && (chk2 != recv_msg.check2) )
                 send_msg.status = FAIL;
              else
                 send_msg.status = SUCCESS;
              /* send the reply message REP to the server */
              printf("Sending the reply message REP to the client \n"); 
              status = send(cfd, &send_msg, sizeof(RepMsg), 0);
               if (status == -1) {
                fprintf(stderr, "*** Client error: unable to send\n");
                return;
                }
              break;
    default: 
           printf("message received with opcode: %d\n", recv_msg.hdr.opcode);
           exit(0);  
   }
 }
}

int main ()
{
   int sfd;
   sfd = startServer();   
   serverLoop(sfd);
}

/*** End of server.c ***/     

