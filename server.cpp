#include<bits/stdc++.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
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

char encoding_scheme[]={' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',',','.','?','0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!'};


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
  char status[MAX_SIZE];
  char file[MAX_SIZE]; 
  int dummy;
} Msg;

typedef struct{
  string userid;
  ll salt;
  ll hash;
  ll prime;
}user;

typedef struct {
  char opcode[MAX_SIZE]; 
  char s_addr[MAX_SIZE]; 
  char d_addr[MAX_SIZE]; 
} EncHdr;

typedef struct {
  EncHdr hdr;
  char buf[MAX_LEN];
  char ID[MAX_SIZE];
  char q[MAX_SIZE];
  char password[MAX_SIZE];
  char status[MAX_SIZE];
  char file[MAX_SIZE]; 
  char dummy[MAX_SIZE];
} EncMsg;

Msg decrypt_msg(EncMsg,ll);
string decrypt_util(string,ll);
EncMsg encrypt_msg(Msg,ll);
string encrypt_util(string,ll);

vector<user> users;

EncMsg encrypt_msg(Msg send_msg,ll caesar_key)
{
	EncMsg enc_msg; 
  string op_code=to_string(send_msg.hdr.opcode);
  op_code=encrypt_util(op_code,caesar_key);
  string status(send_msg.status);
  status=encrypt_util(status,caesar_key);
  strcpy(enc_msg.hdr.opcode,op_code.c_str());
  strcpy(enc_msg.status,status.c_str());
  return enc_msg;
}


string encrypt_util(string input,ll caesar_key){
  cout<<input<<endl;
  int n=input.size();
  string s;
  s.resize(n,' ');
  for(int i=0;i<n;i++)
	{
		for(int j=0;j<66;j++)
		{
			if(input[i]==encoding_scheme[j])
			{	
				s[i]=encoding_scheme[(j + caesar_key) % 66];
				break;
			}	
		}	
	}
	return s;
}



Msg decrypt_msg(EncMsg enc_msg,ll caesar_key){

  Msg dec_msg;
  string opcode(enc_msg.hdr.opcode);
  string userid(enc_msg.ID);
  string password(enc_msg.password);
  string q(enc_msg.q);
  int op_code=atoi(decrypt_util(opcode,caesar_key).c_str());
  if(op_code==10){
      dec_msg.hdr.opcode=op_code;
      strcpy(dec_msg.ID,decrypt_util(userid,caesar_key).c_str());
      strcpy(dec_msg.password,decrypt_util(password,caesar_key).c_str());
      dec_msg.q=atoll(decrypt_util(q,caesar_key).c_str());
  }
  else if(enc_msg.hdr.opcode=="30"){

  }
  else if(enc_msg.hdr.opcode=="50"){

  }

  return dec_msg;
}

string decrypt_util(string input,ll caesar_key){
  int n=input.size();
  string s;
  s.resize(n,' ');
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<66;j++)
		{
			if(input[i]==encoding_scheme[j])
			{	
				s[i]=encoding_scheme[(j - caesar_key + 66) % 66];
				break;
			}	
		}
	}
	return s;
}


int start_server()
{
   int sfd; 
   struct sockaddr_in saddr;
   int status;
   sfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sfd==-1) {
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
  struct sockaddr_in caddr;
  socklen_t size;
  
  
  while(true){
    cout<<"Ready to accept connections............"<<endl;
    int cfd = accept(server_fd,(struct sockaddr *)&caddr,&size);
    if (cfd == -1){
      fprintf(stderr, "Server error: unable to accept request\n");
      continue;
    }
    cout<<"Connected with "<<inet_ntoa(caddr.sin_addr)<<endl;
    memset(cs_buf,0,sizeof(cs_buf));
    int nbytes=recv(cfd, &cs_buf, sizeof(cs_buf),0);

     if (nbytes==0) {
       fprintf(stderr, "Server error: unable to receive\n");
       //exit(EXIT_FAILURE);
       break;
     }
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
    sprintf(sc_buf,"%lld",server_public_key);
    int c=send(cfd,sc_buf,strlen(sc_buf),0);
    if(c>0){
      cout<<"Message sent"<<endl;
    }
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
    
    Msg dec_recv_msg,send_msg;
    EncMsg enc_send_msg,enc_recv_msg;
    nbytes=recv(cfd,&enc_recv_msg,sizeof(enc_recv_msg),0);
    cout<<"received encrypted message bytes from client:"<<nbytes<<endl;
     if (nbytes<=0){
       fprintf(stderr, "Server error: unable to receive\n");
       //exit(EXIT_FAILURE);
       break;
     }
     /* decrypt here  */
      dec_recv_msg=decrypt_msg(enc_recv_msg,caesar_key);
      int opcode=dec_recv_msg.hdr.opcode;

      cout<<"opcode received:"<<opcode<<endl;


      if(opcode==10){
        cout<<"opcode:"<<dec_recv_msg.hdr.opcode<<endl;
        string userid(dec_recv_msg.ID);
        string password(dec_recv_msg.password);
        ll q=dec_recv_msg.q;
        cout<<"Decrypted data"<<endl;
        cout<<"User id:"<<userid<<endl;
        cout<<"password:"<<password<<endl;
        cout<<"Prime:"<<q<<endl;
        bool found=false;
        for(int i=0;i<users.size();i++){
          if(userid==users[i].userid){
            found=true;
          }
        }
        if(found){
          send_msg.hdr.opcode=20;
          string rep="NO";
          strcpy(send_msg.status,rep.c_str());
          EncMsg enc_send_msg=encrypt_msg(send_msg,caesar_key);
          int nbytes=send(cfd,&enc_send_msg,sizeof(enc_send_msg),0);
          if(nbytes<=0){
            cout<<"Failed to send data to client"<<endl;
          }
          else{
            cout<<"Data sent to client:"<<nbytes<<endl;
          }
        }
        else{
          ll sum=0;
          ll salt=rand();
          for(int i=0;password[i];i++){
            sum+=(int)password[i];
          }
          ll pass_hash=power((sum+q+salt),3,q);
          user temp;
          temp.userid=userid;
          temp.salt=salt;
          temp.hash=pass_hash;
          temp.prime=q;
          users.push_back(temp);
          send_msg.hdr.opcode=20;
          string rep="YES";
          strcpy(send_msg.status,rep.c_str());
          EncMsg enc_send_msg=encrypt_msg(send_msg,caesar_key);
          int nbytes=send(cfd,&enc_send_msg,sizeof(enc_send_msg),0);
          if(nbytes<=0){
            cout<<"Failed to send data to client"<<endl;
          }
          else{
            cout<<"Data sent to client:"<<nbytes<<endl;
          }
        }
      }
      else if(opcode==30){
        cout<<"AUTH REQUEST"<<endl;
        string userid(dec_recv_msg.ID);
        string password(dec_recv_msg.password);
        ll q=dec_recv_msg.q;
        cout<<"User id:"<<userid<<endl;
        cout<<"password:"<<password<<endl;

        bool found=false;
        user temp;
        for(int i=0;i<users.size();i++){
          if(userid==users[i].userid){
            found=true;
            temp=users[i];
          }
        }
        if(found){
          cout<<"User does not exist"<<endl;
          send_msg.hdr.opcode=40;
          string rep="NO";
          strcpy(send_msg.status,rep.c_str());
          EncMsg enc_send_msg=encrypt_msg(send_msg,caesar_key);
          send(cfd,&enc_send_msg,sizeof(enc_send_msg),0);
        }
        else{
          ll sum=0;
          ll salt=temp.salt;
          ll prime=temp.prime;
          for(int i=0;password[i];i++){
            sum+=(int)password[i];
          }
          ll pass_hash=power((sum+prime+salt),3,prime);
          if(pass_hash==temp.hash){
            cout<<"Password matched"<<endl;
            Msg send_msg;
            send_msg.hdr.opcode=40;
            string rep="YES";
            strcpy(send_msg.status,rep.c_str());
            EncMsg enc_send_msg=encrypt_msg(send_msg,caesar_key);
            send(cfd,&enc_send_msg,sizeof(enc_send_msg),0);
          }
          else{
            cout<<"Password not matched"<<endl;
            Msg send_msg;
            send_msg.hdr.opcode=40;
            string rep="NO";
            strcpy(send_msg.status,rep.c_str());
            EncMsg enc_send_msg=encrypt_msg(send_msg,caesar_key);
            send(cfd,&enc_send_msg,sizeof(enc_send_msg),0);
          }
        }
      }
      else if(opcode==50){
        
      }    
  }

}

int main(){
  int server_fd;
  signal(SIGPIPE, SIG_IGN);
  server_fd = start_server();
  server_loop(server_fd);
  close(server_fd);
  return 0;
}



