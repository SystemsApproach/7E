/*
 * Socket API calls used by this server:
 *   int socket(int domain, int type, int protocol)
 *   int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
 *   int bind(int sockfd, struct sockaddr *addr, socklen_t addrlen)
 *   int listen(int sockfd, int backlog)
 *   int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
 *   ssize_t recv(int sockfd, void *buf, size_t len, int flags)
 *   int close(int sockfd)
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT 5555
#define MAX_PENDING 5
#define MAX_LINE    4096

int
main(int argc, char *argv[])
{
  struct sockaddr_in sin;
  char buf[MAX_LINE];
  ssize_t buf_len;
  socklen_t addr_len;
  int s, new_s;
  int opt = 1;

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);

  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    exit(1);
  }
  listen(s, MAX_PENDING);

  printf("Server listening on port %d\n", SERVER_PORT);

  /* wait for connection, then receive and print text */
  while (1) {
    addr_len = sizeof(sin);
    if ((new_s = accept(s, (struct sockaddr *)&sin, &addr_len)) < 0) {
      perror("accept");
      exit(1);
    }

    while ((buf_len = recv(new_s, buf, sizeof(buf) - 1, 0)) > 0) {
      buf[buf_len] = '\0';
      fputs(buf, stdout);
      fflush(stdout);
    }

    close(new_s);
  }
}
