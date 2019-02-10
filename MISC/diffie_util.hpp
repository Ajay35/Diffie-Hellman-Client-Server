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
#define ENCODE 66
#define ll long long
using namespace std;



ll power(ll,ll);
ll get_prime();
bool is_prime(ll,ll);
ll get_primitive(ll);
bool miller_primality_test(ll,ll);


ll power(ll a,ll b,ll p){
    ll res=1;
    while(b){
      if(b&1) res= (res*a) %p;
      a=(a*a)% p;
      b>>=1;
    }
    return res;
}

ll get_prime(){
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(100000000,200000000);
  while(true){
    ll p=distr(eng);
    if(is_prime(p,4))
      return p;
  }
}

/* get primitive root of prime number  */
ll get_primitive_root(ll p) {
    vector<ll> fact;
    ll phi=p-1;
    ll n=phi;
    for (ll i=2;i*i<=n;++i)
        if (n % i == 0) {
            fact.push_back(i);
            while (n % i == 0)
                n /= i;
        }
    if (n > 1)
        fact.push_back (n);
 
    for(ll res=2; res<=p; ++res) {
        bool flag=true;
        for (size_t i=0; i<fact.size() && flag;++i)
            flag &= power(res,phi/fact[i],p) != 1;
        if(flag)
	  return res;
    }
    return -1;
}



bool miller_primality_test(ll d,ll n) 
{ 
    ll a=2+rand()%(n-4);  
    ll x=power(a,d,n); 
  
    if(x==1 || x==n-1) 
       return true; 
   
    while(d!=n-1) 
    { 
        x=(x*x)%n; 
        d*=2; 
        if(x==1)
	  return false; 
        if(x==n-1)
	  return true; 
    } 
    return false; 
} 
bool is_prime(ll n,ll k) 
{ 
    if(n<=1 || n==4)
      return false; 
    if(n<=3)
      return true; 
  
    ll d=n-1; 
    while(d%2==0) 
        d=d/2; 
  
    for(int i=0;i<k;i++) 
         if(!miller_primality_test(d, n)) 
              return false; 
    return true; 
} 
