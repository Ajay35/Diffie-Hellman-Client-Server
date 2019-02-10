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
#include <fcntl.h>
#include "diffie_util.hpp"
#define MAX_SIZE 80
#define MAX_LEN 1024
#define ll long long
#define DEFAULT_SERVER "127.0.0.1"
#define SERVICE_PORT 8080
using namespace std;

int server_connect(char *);
void communicate_with_server (int,ll);


char encoding_scheme[]={' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',',','.','?','0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!'};


typedef struct {
int opcode; 
int s_addr; 
int d_addr; 
} Hdr;

typedef struct {
  Hdr hdr;
  char buf[MAX_LEN+1];
  char ID[MAX_SIZE];
  ll q;
  char password[MAX_SIZE];
  char status[MAX_SIZE];
  char file[MAX_SIZE]; 
  char dummy[MAX_SIZE];
} Msg;

typedef struct {
  char opcode[MAX_SIZE]; 
  char s_addr[MAX_SIZE]; 
  char d_addr[MAX_SIZE]; 
} EncHdr;

typedef struct {
  EncHdr hdr;
  char buf[MAX_LEN+1];
  char ID[MAX_SIZE];
  char q[MAX_SIZE];
  char password[MAX_SIZE];
  char status[MAX_SIZE];
  char file[MAX_SIZE]; 
  char dummy[MAX_SIZE];
} EncMsg;

string encrypt_util(string,ll);
EncMsg encrypt_msg(Msg,ll);
string decrypt_util(string,ll);
Msg decrypt_msg(EncMsg,ll);


string encrypt_util(string input,ll caesar_key){
  int n=input.size();
  string s;
  s.resize(n,' ');
  for(int i=0;i<n;i++)
	{
    if (input[i]==EOF) {
      cout<<"Rnd of file detected"<<endl;
      break;
    }
		for(int j=0;j<66;j++)
		{
			if(input[i]==encoding_scheme[j]){	
				s[i]=encoding_scheme[(j+caesar_key)%66];
				break;
			}	
		}	
	}
	return s;
}

EncMsg encrypt_msg(Msg send_msg,ll caesar_key)
{
	EncMsg enc_msg;
  if(send_msg.hdr.opcode==10){
    string op_code=to_string(send_msg.hdr.opcode);
    op_code=encrypt_util(op_code,caesar_key);
    string userid(send_msg.ID);
    userid=encrypt_util(userid,caesar_key);
    string password(send_msg.password);
    password=encrypt_util(password,caesar_key);
    string prime=to_string(send_msg.q);
    prime=encrypt_util(prime,caesar_key);
    strcpy(enc_msg.hdr.opcode,op_code.c_str());
    strcpy(enc_msg.ID,userid.c_str());
    strcpy(enc_msg.password,password.c_str());
    strcpy(enc_msg.q,prime.c_str());
    return enc_msg;
  }
  else if(send_msg.hdr.opcode==30){
    string op_code=to_string(send_msg.hdr.opcode);
    op_code=encrypt_util(op_code,caesar_key);
    string userid(send_msg.ID);
    userid=encrypt_util(userid,caesar_key);
    string password(send_msg.password);
    password=encrypt_util(password,caesar_key);
    strcpy(enc_msg.hdr.opcode,op_code.c_str());
    strcpy(enc_msg.ID,userid.c_str());
    strcpy(enc_msg.password,password.c_str());
    return enc_msg;
  }

  else if(send_msg.hdr.opcode==50){
    string op_code=to_string(send_msg.hdr.opcode);
    op_code=encrypt_util(op_code,caesar_key);
    string filename(send_msg.file);
    filename=encrypt_util(filename,caesar_key);    
    strcpy(enc_msg.hdr.opcode,op_code.c_str());
    strcpy(enc_msg.file,filename.c_str());
    return enc_msg;
  }
	return enc_msg;
}

Msg decrypt_msg(EncMsg recv_msg,ll caesar_key){
  
    
    Msg msg;
    string temp(recv_msg.hdr.opcode);
    string status(recv_msg.status);
    msg.hdr.opcode=atoi(decrypt_util(temp,caesar_key).c_str());
    if(msg.hdr.opcode==60){  
      string data(recv_msg.buf);
      cout<<"Data received"<<recv_msg.buf<<endl;
      strcpy(msg.buf,decrypt_util(data,caesar_key).c_str());
      cout<<"decryped data:"<<msg.buf<<endl;
      //cout<<"opcode received:"<<msg.hdr.opcode<<endl;      
    }
    strcpy(msg.dummy,recv_msg.dummy);
    strcpy(msg.status,decrypt_util(status,caesar_key).c_str());
    return msg;
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



int server_connect(char *sip)
{
   int client_fd;
   struct sockaddr_in saddr;
   int status;

   
   client_fd=socket(AF_INET,SOCK_STREAM,0);
   if(client_fd==-1){
      fprintf (stderr, "Client error: unable to get socket descriptor\n");
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
   int ch,status,nbytes;
   while(true){

     cout<<"Choose operation:"<<endl;
     cout<<"1. Login Create"<<endl;
     cout<<"2. Service Request"<<endl;
     cin>>ch;
       
     if(ch== 1){
        Msg send_msg,recv_msg;
        string ID,password;
        ll q;
        send_msg.hdr.opcode=10;
        send_msg.hdr.s_addr=1;
        send_msg.hdr.d_addr=2;
        cout<<"Enter user ID"<<endl;
        cin>>ID;
        cout<<"Enter user password"<<endl;
        cin>>password;
        strcpy(send_msg.ID,ID.c_str());  
        strcpy(send_msg.password,password.c_str());  
        q=get_prime();
        send_msg.q=q;

        EncMsg enc_msg=encrypt_msg(send_msg,caesar_key);
        cout<<"encryped userid is:"<<enc_msg.ID<<endl;
        status=send(cfd, &enc_msg,sizeof(EncMsg),0);
        if (status==0) {
            fprintf(stderr, "Client error: unable to send\n");
            return;
        }
     }
    else if(ch==2){
      cout<<"****************AUTH REQUEST***************"<<endl;
      Msg send_msg,recv_msg;
      string ID,password;
      send_msg.hdr.opcode=30;
      send_msg.hdr.s_addr=1;
      send_msg.hdr.d_addr=2;
      cout<<"Enter user ID"<<endl;
      cin>>ID;
      cout<<"Enter user password"<<endl;
      cin>>password;
      strcpy(send_msg.ID,ID.c_str());  
      strcpy(send_msg.password,password.c_str());  
      EncMsg enc_msg=encrypt_msg(send_msg,caesar_key);
      status=send(cfd, &enc_msg,sizeof(enc_msg),0);
      if (status <= 0) {
          fprintf(stderr, "Client error: unable to send\n");
          return;
      }
      cout<<"***************************"<<endl;
    }
    else{
      cout<<"wrong choice:"<<endl;
      continue;
    }

    EncMsg recv_msg;
    nbytes=recv(cfd, &recv_msg, sizeof(recv_msg), 0);
    if (nbytes<=0){
        fprintf(stderr, "Client error: unable to receive\n");
        break;
    }

    /* decrypt here (according to header of msg)*/
    Msg dec_recv_msg=decrypt_msg(recv_msg,caesar_key);


    if(dec_recv_msg.hdr.opcode==20){
      cout<<"LOGIN REPLY:"<<endl;
      string test(dec_recv_msg.status);
      if(test=="NO"){
        cout<<"xxxxxxxxxxx-----------LOGIN CREATE FAILED-------------xxxxxxxxxxxxxx"<<endl;
      }
      else if(test=="YES"){
        cout<<"*******************LOGIN CREATE SUCCESSFUL*****************"<<endl;
      }
      else{
        cout<<"INVALID RESPONSE"<<endl;
      }
    }  
    else if(dec_recv_msg.hdr.opcode==40){
        cout<<"AUTH REPLY:"<<endl;
        string test(dec_recv_msg.status);
        if(test=="NO"){
          cout<<"xxxxxxxx-------AUTH FAILED----xxxxxxxxxxx"<<endl;
        }
        else if(test=="YES"){
          string file_name;
          cout<<"****************AUTH SUCCESSFUL************"<<endl;
        Msg send_msg,dec_recv_msg;
        EncMsg enc_recv_msg,enc_send_msg;
        send_msg.hdr.opcode=50;
        cout<<"Enter file name:"<<endl;
        cin>>file_name;
        strcpy(send_msg.file,file_name.c_str());
        enc_send_msg=encrypt_msg(send_msg,caesar_key);
        nbytes=send(cfd, &enc_send_msg,sizeof(EncMsg),0);
        if (nbytes == -1) {
            fprintf(stderr, "Client error: unable to send\n");
            return;
        }

        nbytes=recv(cfd, &enc_recv_msg,sizeof(EncMsg),0);
        if (nbytes == -1) {
            fprintf(stderr, "Client error: unable to send\n");
            return;
        }
        dec_recv_msg=decrypt_msg(enc_recv_msg,caesar_key);
        cout<<"*********SERVER REPLY********************"<<endl;
        if(dec_recv_msg.hdr.opcode==0){
          cout<<"File Not found"<<endl;
          exit(0);
        }
          else if(dec_recv_msg.hdr.opcode==100){
            cout<<"File Found"<<endl;
            file_name=file_name+"_copy";
                  
            int file_size=atoi(dec_recv_msg.dummy);

            cout<<"File size is:"<<file_size<<endl;
            
            
            int to_file=creat(file_name.c_str(),0777);
            if(to_file<0){
              cout<<"Error creating destination file\n";
              return;
            }
            
            int w,rec,total=0;
            char buf[1024];
            while(total!=file_size){
              memset(buf,0,sizeof(buf));
              rec=recv(cfd,&buf,sizeof(buf),0);
              if(rec<0){
                cout<<"Error receiving\n";
                return ;
              }
              cout<<"Encrypted data"<<buf<<endl;
              string str(buf);
              cout<<str;
              str=decrypt_util(str,caesar_key);
              cout<<"decrypted data:"<<str<<endl;;
              strcpy(buf,str.c_str());
              w=write(to_file,buf,rec);
              total+=rec;
            }

            cout<<"File downloading finished"<<endl;
      }    
    }     
  }
   }

}

int main(int argc, char* argv[]){

  char cs_buf[MAX_LEN];
  char sc_buf[MAX_LEN];
  char server_ip[16];
  int client_fd;

  cout<<"***************** KEY EXCHANGE STARTS **************************"<<endl;
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

  int i=0,j=0;
  char temp[MAX_LEN];
  
  while(sc_buf[i]!='\0'){
    temp[j++]=sc_buf[i++];
  }    
  ll server_public_key=atoll(temp);
  
  cout<<"Public key of server:"<<endl;
  cout<<server_public_key<<endl;
  cout<<"shared key from client"<<endl;
  ll shared_key_client=power(server_public_key,client_private_key,prime);
  cout<<shared_key_client<<endl;
  ll caesar_key=shared_key_client%66;
  cout<<"Caesar key:"<<caesar_key<<endl;
  cout<<"***************** KEY EXCHANGE FINISHED **************************"<<endl;
  communicate_with_server(client_fd,caesar_key);
  close(client_fd);

  return 0;
}
