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

int main(int argc, char *argv[]) {
  int sListen, sClient;
  struct sockaddr_in rp,client;
  char* buffer = (char*)malloc(101);
  int iPort = 28345;
  sListen = socket(AF_INET,SOCK_STREAM,0);

  if (sListen < 0) {
    printf("Eroare la initializare socket \n");
    exit(1);
  }
  
  rp.sin_addr.s_addr = htonl(INADDR_ANY);
  rp.sin_family = AF_INET;
  rp.sin_port = htons(iPort);

  if (bind(sListen,(struct sockaddr*)&rp,sizeof(rp))<0) {
    printf("Eroare la bind \n");
    exit(1);
  }

  listen(sListen,8);

  socklen_t addrSize = sizeof(client);
  sClient = accept(sListen, (struct sockaddr *)&client,&addrSize);

  if (sClient < 0) {
    printf("Nu o mers acceptul \n");
  }

  printf("Conectat la client %s:%d \n", inet_ntoa(client.sin_addr),ntohs(client.sin_port));

  int size;
  if (recv(sClient,(char*)&size,sizeof(int),0) <0) {
    printf("Eroare la primire lungime \n");
  }

  size = ntohs(size);
  if (recv(sClient,buffer,size,0) < 0) {
    printf("Eroare primire mesaj \n");
  }

  printf("Primit de la client: %s \n",buffer);
  
  size_t s = 100;
  size = getline(&buffer,&s,stdin);
  size = htons(strlen(buffer));
  if (send(sClient,(char *)&size,sizeof(size),0) < 0) {
    printf("Eroare la trimitere lungime \n");
  }
  if (send(sClient,buffer,strlen(buffer),0) <0) {
    printf("Eroare la trimitere raspuns \n");
  }

  close(sClient);
  close(sListen);
  return 0;


}
