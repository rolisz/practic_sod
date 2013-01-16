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
  size_t size = 100; 
  int len = getline(&buffer,&size,stdin);

  len = htons(len);

  if (send(sServer,(char*)&len,sizeof(int),0) <0) {
    printf("eroare la trimitere lungime \n");
  }
  if (send(sServer,buffer,strlen(buffer),0) <0) {
    printf("eroare la trimitere mesaj");
  }
  
  if (recv(sServer,(char*)&len,sizeof(len),0) <0) {
    printf("Eroare la primire lungime");
  }
  if (recv(sServer,buffer,ntohs(len),0) <0) {
    printf("Eroare la primire mesaj");
  }

  printf("Primit server %s \n",buffer);
  
  return 0;
}
