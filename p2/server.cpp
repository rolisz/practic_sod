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
#include <signal.h>
pthread_mutex_t mutex;

pthread_t threads[100];
int i;
void sighandler(int num) {
  printf("Catched signal \n");
  int j;
  for (j = 0; j < i; j++ ) {
    pthread_join(threads[j],NULL);
  }
  exit(0);
}
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

struct elem {
  char user[20];
  char parola[20];
  int folosit;
  elem* next;
} nod;
elem* lista;
double medie = 0;
int n = 0;
void adauga_lista(elem* start, char user[20],char pw[20]) {
  elem* current = start;
  while (current->next != NULL) {
    current = current->next;
  }
  elem* nou = (elem*)malloc(sizeof(elem));
  strcpy(nou->user,user);
  strcpy(nou->parola,pw);
  nou->folosit = 0;
  nou->next = NULL;
  current->next = nou;
}

int cauta(elem* start, char user[20],char pw[20]) {
  int gasit = 0;
  elem* current = start;
  while (current != NULL) {
    if (strcmp(user,current->user) == 0 &&
        strcmp(pw,current->parola) == 0) {
      if (current->folosit == 0) {
        current->folosit = 1;
        return 1;
      }
      else {
        return -1;
      }
    }
    current = current->next;
  }
  return 0;
}
void* thread(void *arg) {
  int sClient = (int)arg;
  int size;
  char* user = (char*)malloc(20),*pass = (char*)malloc(20);
  int len = receive(user,sClient);
  user[len-1] = '\0';
  len = receive(pass,sClient);
  pass[len-1] = '\0';
  double f;
  if (recv(sClient,(char*)&f,sizeof(f),0) < 0) {
    printf("Eroare la primire float \n");
  }
  
  printf("Primit de la client: %s %s %f\n",user,pass,f);
  

  pthread_mutex_lock(&mutex);
  int c = cauta(lista,user,pass);
  if (c == 1) {
    medie = (medie*n+f)/(n+1);
    f = medie;
    n++;
  }
  else if (c == -1) {
    f = -2;
  }
  else if (c == 0) {
    f = -1;
  }
  pthread_mutex_unlock(&mutex);
  printf("Value %f \n",f);
  if (send(sClient,(char*)&f,sizeof(f),0) < 0) {
    printf("Eroare la trimitere numar \n");
  }
  close(sClient);
}

int main(int argc, char *argv[]) {
  int sListen, sClient;
  struct sockaddr_in rp,client;
  int iPort = 28345;
  sListen = socket(AF_INET,SOCK_STREAM,0);

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
  
  lista = (elem*)malloc(sizeof(elem));
  strcpy(lista->user, "rolisz");
  strcpy(lista->parola, "password");
  lista->folosit = 0;
  lista->next = NULL;

  adauga_lista(lista,"srir1180","parola");
  adauga_lista(lista,"username","pw");
  adauga_lista(lista,"utilizator","jelszo");

  if (signal(SIGINT,sighandler) == SIG_ERR) {
    printf("Eroare la signal handling");
  }

  i = 0;
  int s;
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
