/*
 * Socket API calls used by this client:
 *   int socket(int domain, int type, int protocol)
 *   int connect(int sockfd, struct sockaddr *addr, socklen_t addrlen)
 *   ssize_t send(int sockfd, const void *buf, size_t len, int flags)
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
#define MAX_LINE    4096

int
main(int argc, char *argv[])
{
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE];
  int s;
  size_t len;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <server-name>\n", argv[0]);
    exit(1);
  }
  host = argv[1];

  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
    exit(1);
  }

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  /* active open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("connect");
    close(s);
    exit(1);
  }

  /* main loop: read from stdin and send to server */
  while (fgets(buf, sizeof(buf), stdin)) {
    len = strlen(buf);
    send(s, buf, len, 0);
  }

  close(s);
  return 0;
}
