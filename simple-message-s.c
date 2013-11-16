/** A simple message program
 *  author: Blakely Madden
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "9999"
#define BACKLOG 20

struct addrinfo* getAddrList() {
  struct addrinfo *res;
  struct addrinfo hints;

  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = 0;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;

  int failure = getaddrinfo(NULL, PORT, &hints, &res);
  if (failure)
    exit(1);
  else
    return res;
}

int getSockFD() {
  struct addrinfo* res = getAddrList();
  struct addrinfo* p;
  int sockfd = 0;
  int binded;

  for (p = res; p != NULL; p=p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1)
      continue;
    binded = bind(sockfd, p->ai_addr, p->ai_addrlen);
    if (binded)
      continue;
    break;
  }
  if (p == NULL)
    exit(2);
  freeaddrinfo(res);
  return sockfd;
}

int startListening() {
  int sockfd = getSockFD();

  int failure = listen(sockfd, BACKLOG);
  if (failure)
    exit(3);

  return sockfd;
}

int acceptConn(char *message, int message_size) {
  struct sockaddr_storage client_addr;
  int sockfd = startListening();
  int accepted;
  int val;

  while (sockfd) {
    unsigned int size_storage = sizeof client_addr;
    accepted = accept(sockfd, (struct sockaddr*)&client_addr, &size_storage);
    if (accepted == -1)
      continue;

    if (!fork()) {
      close(sockfd);
      val = send(accepted, message, message_size, 0);
      if (!val || val == -1)
        perror("send");
      printf("Sent -- %s\n", message);
      close(accepted);
      exit(0);
    }
    close(accepted);
  }

  return 0;
}

int main (int argc, char *argv[]) {
  int win;
  int size;
  if (argc != 2)
    exit(4);
  size = strlen(argv[1]);

  win = acceptConn(argv[1], size);
  return win;
}
