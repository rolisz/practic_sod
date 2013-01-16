#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void send(int length,char* mesaj,int sServer) {
  length = htonl(length);
  if (send(sServer,(char*)&length,sizeof(int),0) <0) {
    printf("eroare la trimitere lungime \n");
  }
  if (send(sServer,mesaj,strlen(mesaj),0) <0) {
    printf("eroare la trimitere mesaj");
  }
}

int receive(char* buffer, int sServer) {
  int len;
  if (recv(sServer,(char*)&len,sizeof(len),0) <0) {
    printf("Eroare la primire lungime");
  }
  len = ntohl(len);
  if (recv(sServer,buffer,len,0) <0) {
    printf("Eroare la primire mesaj");
  }
  return len;
}

int main(int argc, char* argv[]) {
  int sServer;
  char* buffer = (char*) malloc(101);
  struct sockaddr_in server;

  int iPort = 28345;
  char adServer[100] = "127.0.0.1";

  sServer = socket(AF_INET,SOCK_STREAM,0);
  if (sServer < 0) {
    printf("O puscat la initi socket \n");
    exit(1);
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(iPort);
  server.sin_addr.s_addr = inet_addr(adServer);

  if (connect(sServer,(struct sockaddr*)&server,sizeof(server))<0) {
    printf("O puscat conectarea \n");
    exit(1);
  }
  size_t size = 20; 
  printf("Username: \n");
  int len = getline(&buffer,&size,stdin);

  send(len,buffer,sServer);
  printf("Parola: \n");
  len = getline(&buffer,&size,stdin);
  send(len,buffer,sServer);
  
  printf("Float: \n");
  double f;
  scanf("%lf",&f);
  if (send(sServer,(char*)&f,sizeof(f),0) < 0) {
    printf("Erreur du travail");
  }
  if (recv(sServer,(char*)&f,sizeof(f),0) < 0) {
    printf("Eroare la primire medie");
  }
  printf("Primit server %f\n",f);
  
  return 0;
}
