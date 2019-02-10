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
#define ENCODE 66
#define ll long long

using namespace std;
int main(){
  ll k=5;
  ll prime=get_prime();
  cout<<"prime chosen at random:"<<prime<<endl;
  ll p_root=get_primitive_root(prime);
  cout<<"primitive root:"<<p_root<<endl;

  cout<<"For client"<<endl;
  
  ll client_private_key = rand() % (prime-1) + 1;
  cout<<"Private key of Client:"<<client_private_key<<endl;
  ll client_public_key = power(p_root,client_private_key,prime);
  cout<<"Public key of Client:"<<client_public_key<<endl;

  cout<<"----------------"<<endl;
  cout<<"For server"<<endl;

  ll server_private_key = rand() % (prime-1) + 1;
  cout<<"Private key of Client:"<<server_private_key<<endl;
  ll server_public_key = power(p_root,server_private_key,prime);
  cout<<"Public key of server:"<<client_public_key<<endl;

  cout<<"----------------"<<endl;

  cout<<"shared key from client:"<<endl;
  ll shared_key_client=power(server_public_key,client_private_key,prime);
  cout<<shared_key_client<<endl;
  cout<<"----------------"<<endl;
  cout<<"shared key from server"<<endl;
  ll shared_key_server=power(client_public_key,server_private_key,prime);
  cout<<shared_key_server<<endl;
  cout<<"----------------"<<endl;
  return 0;  
}
