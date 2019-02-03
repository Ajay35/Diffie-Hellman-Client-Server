#include<bits/stdc++.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <limits>
#include <math.h>
#include <random>
#include "diffie_util.hpp"
#define MAX_SIZE 80
#define MAX_LEN 1024

#define ll long long
#define SERVER_PORT 8080
#define QUEUE_SIZE 5
using namespace std;

int start_server_connect();
void server_loop(int);
void comm_with_client(int,ll);

typedef struct {
int opcode; /* opcode for a message */
int s_addr; /* source address */
int d_addr; /* destination address */
} Hdr;

typedef struct {
  Hdr hdr;
  char buf[MAX_LEN];
  char ID[MAX_SIZE];
  ll q;
  char password[MAX_SIZE];
  int status;
  char file[MAX_SIZE]; 
  int dummy;
} Msg;

int start_server()
{
   int sfd; 
   struct sockaddr_in saddr;
   int status;

   sfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sfd == -1) {
      fprintf(stderr, "Server error: unable to get socket descriptor\n");
      exit(1);
   }

   saddr.sin_family = AF_INET; 
   saddr.sin_port = htons(SERVER_PORT);  
   saddr.sin_addr.s_addr = INADDR_ANY;
   bzero(&(saddr.sin_zero),8);

   status = bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if(status == -1){
      fprintf(stderr,"Server error: unable to bind to port %d\n", SERVER_PORT);
      exit(2);
   }

   status =listen(sfd,QUEUE_SIZE);
   if (status == -1) {
      fprintf(stderr, "Server error: unable to listen\n");
      exit(3);
   }

   fprintf(stderr, "Server successfully started, listening to port %hd\n", SERVER_PORT);
   return sfd;
}



void server_loop(int server_fd){
  char cs_buf[MAX_LEN];
  char sc_buf[MAX_LEN];
  int cfd;             
  struct sockaddr_in caddr;
  socklen_t size;
  
  
  while(true){
    cfd = accept(server_fd,(struct sockaddr *)&caddr,&size);
    if (cfd == -1){
      fprintf(stderr, "Server error: unable to accept request\n");
      continue;
    }
    cout<<"Connected with "<<inet_ntoa(caddr.sin_addr)<<endl;
    memset(cs_buf,0,sizeof(cs_buf));
    recv(cfd, &cs_buf, sizeof(cs_buf),0);
    //cout<<cs_buf<<endl;
    memset(sc_buf,0,sizeof(sc_buf));

    int i=0,j=0;
    char temp[MAX_LEN];
    
    while(cs_buf[i]!='$'){
      temp[j++]=cs_buf[i++];
    }    
    ll prime=atoll(temp);
    
    bzero(temp,strlen(temp)+2);
    j=0;
    i++;
    while(cs_buf[i]!='$'){
      temp[j++]=cs_buf[i++];
    }
    ll primitive_root=atoll(temp);
    
    bzero(temp,strlen(temp)+2);
    j=0;
    i++;
    while(cs_buf[i]!='\0'){
      temp[j++]=cs_buf[i++];
    }    
    ll client_public_key=atoll(temp);
    
    cout<<"Prime:"<<prime<<endl;
    cout<<"Primitive root:"<<primitive_root<<endl;
    cout<<"client public key:"<<client_public_key<<endl;
    
    ll server_private_key=rand()% (prime-1)+1;
    cout<<"Private key of server:"<<server_private_key<<endl;
    ll server_public_key = power(primitive_root,server_private_key,prime);
    cout<<"Public key of server:"<<server_public_key<<endl;
    ll shared_key_server=power(client_public_key,server_private_key,prime);
    cout<<"shared key from server:"<<shared_key_server<<endl;
    sprintf(cs_buf,"%lld",server_public_key);
    send(cfd,sc_buf,strlen(sc_buf),0);
    cout<<"Key exhange finished"<<endl;
    ll caesar_key=shared_key_server%66;
    cout<<"Caesar key:"<<caesar_key<<endl;
    std::thread t(comm_with_client,cfd,caesar_key);
    t.detach();
      }
}

void comm_with_client(int cfd,ll caesar_key){
  int nbytes;
  while(true){
    
    Msg send_msg,recv_msg;

    nbytes=recv(cfd,&recv_msg,sizeof(Msg), 0);
     if (nbytes == -1) {
       fprintf(stderr, "Server error: unable to receive\n");
     }

     switch(recv_msg.Hdr.opcode){

     case 10:
       cout<<recv_msg.Msg.ID<<endl;
       cout<<recv_msg.Msg.password<<endl;
       cout<<recv_msg.Msg.q<<endl;
       break;
     case 30:
       break;
     case 50:
       break;
       
     }
  }

  
}

int main(){
  int server_fd;
  server_fd = start_server();
  server_loop(server_fd);
  close(server_fd);
  return 0;
}



