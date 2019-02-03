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
#define DEFAULT_SERVER "127.0.0.1"
#define SERVICE_PORT 8080
using namespace std;

int server_connect(char *);
void communicate_with_server (int,ll);


typedef struct {
int opcode; 
int s_addr; 
int d_addr; 
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




int server_connect(char *sip)
{
   int client_fd;
   struct sockaddr_in saddr;
   int status;

   
   client_fd=socket(AF_INET,SOCK_STREAM,0);
   if(client_fd==-1){
      fprintf (stderr, "*** Client error: unable to get socket descriptor\n");
      exit(1);
   }

   saddr.sin_family = AF_INET; 
   saddr.sin_port = htons(SERVICE_PORT);
   saddr.sin_addr.s_addr = inet_addr(sip);
   bzero(&(saddr.sin_zero),8);
   status = connect(client_fd,(struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Client error: unable to connect to server\n");
      exit(1);
   }
   fprintf(stderr, "Connected to server\n");
   return client_fd;
}

void communicate_with_server(int cfd,ll caesar_key){
   char buffer[MAX_LEN];
   Msg send_msg;
   Msg recv_msg;
   int ch,status;
   while(true){
     cout<<"Choose operation:"<<endl;
     cout<<"1. Login Create"<<endl;
     cout<<"2. Auth Request"<<endl;
     cout<<"3. Service Request"<<endl;
     cin>>ch;
     
     switch(ch){
       
     case 1:
       Hdr header;
       Msg send_msg,recv_msg;
       string ID,password;
       ll q;
       header.opcode=10;
       header.s_addr=1;
       header.d_addr=2;
       send_msg.Hdr=header;
       cout<<"Enter user ID"<<endl;
       cin>>str;
       cout<<"Enter user password"<<endl;
       cin>>password;
       strcpy(send_msg.ID,ID.c_str());  
       strcpy(send_msg.password,password.c_str());  
       q=get_prime();
       send_msg.q=q;
       status=send(cfd, &send_msg,sizeof(Msg),0);
       if (status == -1) {
	 fprintf(stderr, "Client error: unable to send\n");
	 return;
       }
       nbytes = recv(cfd, &recv_msg, sizeof(RepMsg), 0);
       if (nbytes == -1) {
	 fprintf(stderr, "Client error: unable to receive\n");
       }
       break;
     case 2:
       break;
     case 3:
       break;
     default: exit(0);

     }
   }
   

}

int main(int argc, char* argv[]){

  char cs_buf[MAX_LEN];
  char sc_buf[MAX_LEN];
  char server_ip[16];
  int client_fd;


  ll prime=get_prime();
  cout<<"Prime number chosen:"<<prime<<endl;
  
  ll  primitive_root=get_primitive_root(prime);
  cout<<"primitive root:"<<primitive_root<<endl;
  
  ll client_private_key=rand()%(prime-1)+1;
  cout<<"Private key of Client:"<<client_private_key<<endl;
  
  ll client_public_key = power(primitive_root,client_private_key,prime);
  cout<<"Public key of Client:"<<client_public_key<<endl;
  
  
  strcpy(server_ip,(argc==2)?argv[1]:DEFAULT_SERVER);
  client_fd=server_connect(server_ip);
  memset(cs_buf,0,sizeof(cs_buf));
  sprintf(cs_buf,"%lld$%lld$%lld",prime, primitive_root, client_public_key);
  
  send(client_fd,cs_buf,strlen(cs_buf),0);
  memset(sc_buf,0,sizeof(sc_buf));
  recv(client_fd,sc_buf,sizeof(sc_buf),0);
  //sc_buf[i]='\0';

  int i=0,j=0;
  char temp[MAX_LEN];
  
  while(cs_buf[i]!='\0'){
    temp[j++]=cs_buf[i++];
  }    
  ll server_public_key=atoll(temp);
  
  cout<<"Public key of server:"<<endl;
  cout<<server_public_key<<endl;
  cout<<"shared key from client"<<endl;
  ll shared_key_client=power(server_public_key,client_private_key,prime);
  cout<<shared_key_client<<endl;
  ll caesar_key=shared_key_client%66;
  cout<<"Caesar key:"<<caesar_key<<endl;
  cout<<"Key exhange finished"<<endl;
  communicate_with_server(client_fd,caesar_key);
  close(client_fd);

  return 0;
}
