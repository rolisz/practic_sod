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

pthread_mutex_t mutex;

void* thread(void *arg) {
  int sClient = (int)arg;
  int size;
  char* buffer = (char*)malloc(101),*copie = (char*)malloc(101);
  if (recv(sClient,(char*)&size,sizeof(int),0) <0) {
    printf("Eroare la primire lungime \n");
  }

  size = ntohs(size);
  if (recv(sClient,buffer,size,0) < 0) {
    printf("Eroare primire mesaj \n");
  }
  
  printf("Primit de la client: %s \n",buffer);
  strcpy(copie,buffer);
  int i;
  for (i = 0; i < size; i++) {
      if (buffer[i] == 'a' || buffer[i] == 'e' || buffer[i] == 'i' || buffer[i] == 'o'
            || buffer[i] == 'u') {
        buffer[i] = ' '; 
      }
  }
  
  printf("Modificat in: %s \n",buffer);

  pthread_mutex_lock(&mutex);
  FILE* file = fopen("a.txt","a+");
  int nrLinii = 0;
  char temp[100];
  while(fgets(temp,100,file)) {
    nrLinii++;
  }
  fputs(copie,file);
  fputs(buffer,file);
  fclose(file);
  pthread_mutex_unlock(&mutex);
  size_t s = 100;
  size = htonl(strlen(buffer));
  if (send(sClient,(char *)&size,sizeof(size),0) < 0) {
    printf("Eroare la trimitere lungime \n");
  }
  if (send(sClient,buffer,strlen(buffer),0) <0) {
    printf("Eroare la trimitere raspuns \n");
  }
  nrLinii = htonl(nrLinii);
  if (send(sClient,(char*)&nrLinii,sizeof(int),0) <0 ) {
    printf("Eroare la trimitere numar \n");
  }
  close(sClient);
}

int main(int argc, char *argv[]) {
  int sListen, sClient;
  struct sockaddr_in rp,client;
  int iPort = 28345;
  sListen = socket(AF_INET,SOCK_STREAM,0);

  pthread_t threads[100];

  pthread_mutex_init(&mutex,NULL);
  if (sListen < 0) {
    printf("Eroare la initializare socket \n");
    exit(1);
  }
  
  rp.sin_addr.s_addr = htonl(INADDR_ANY);
  rp.sin_family = AF_INET;
  rp.sin_port = htons(iPort);

  if (bind(sListen,(sockaddr*)&rp,sizeof(rp))<0) {
    printf("Eroare la bind \n");
    exit(1);
  }

  listen(sListen,8);
  
  int i = 0,s;
  socklen_t addrSize = sizeof(client);
  while (1) {
    sClient = accept(sListen, (struct sockaddr *)&client,&addrSize);

    if (sClient < 0) {
       printf("Nu o mers acceptul \n");
    }

    printf("Conectat la client %s:%d \n", inet_ntoa(client.sin_addr),ntohs(client.sin_port));

    s = pthread_create(&threads[i],NULL, &thread,(void*)sClient);
    i++;
  }
  close(sListen);
  int j;
  for (j = 0; j < i; j++) {
    pthread_join(threads[j],NULL);
  }
  pthread_mutex_destroy(&mutex);
  return 0;


}
