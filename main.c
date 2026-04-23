#include "include/arrays.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3030
#define BUF_SIZE 1024

#define MSGS_PATH "db/messages.txt"
#define SEND_RESPONSE "HTTP/1.1 200 OK\r\n\r\nsending"

typedef struct {
  char *method;
  char *route;
} req_info;

const char *getContentType(const char *path) {
  if (strstr(path, ".css"))
    return "text/css";
  if (strstr(path, ".js"))
    return "application/javascript";
  if (strstr(path, ".ico"))
    return "image/x-icon";
  if (strstr(path, ".png"))
    return "image/png";
  if (strstr(path, ".jpg"))
    return "image/jpeg";
  if (strstr(path, ".json"))
    return "application/json";
  if (strstr(path, ".html"))
    return "text/html";

  return "text/plain";
}

req_info tokenize(char *recv) {
  req_info req;
  char *firstline = strtok(recv, "\r\n");
  req.method = strtok(firstline, " ");
  req.route = strtok(NULL, " ");
  return req;
}

char *route(const char *route, const char *method) {
  if (strcmp(method, "GET") == 0) {
    if (strcmp(route, "/") == 0) {
      return "static/index.html";
    }
    if (strcmp(route, "/jacks") == 0) {
      return "static/jacks.html";
    }
    static char path[256];
    snprintf(path, sizeof(path), "static%s", route);
    return path;
  }
  if (strcmp(method, "POST") == 0) {
    if (strcmp(route, "/send") == 0) {
      return SEND_RESPONSE;
    }
  }
  return "HTTP/1.1 404 Not Found";
}
void updateMessages();

void sendData(int *sock, char *method, const char *file) {
  if (strcmp(method, "GET") == 0) {
    if (file == NULL)
      return;

    FILE *html = fopen(file, "rb");
    if (!html) {
      perror("failed to open file");
      send(*sock, "HTTP/1.1 404 Not Found", strlen("HTTP/1.1 404 Not Found"),
           0);
      return;
    }

    fseek(html, 0, SEEK_END);
    long fileSize = ftell(html);
    rewind(html);

    char buffer[BUF_SIZE] = {0};
    size_t read = 0;

    char header[256];
    snprintf(
        header, sizeof(header),
        "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n",
        getContentType(file), fileSize);
    send(*sock, header, strlen(header), 0);

    while ((read = fread(buffer, sizeof(buffer[0]), BUF_SIZE, html)) > 0) {
      send(*sock, buffer, read, 0);
    }
    fclose(html);
  }
}

void *handleClient(void *sock) {
  int *clientSocket = (int *)sock;

  char recvBuf[BUF_SIZE] = {0};

  recv(*clientSocket, recvBuf, BUF_SIZE, 0);
  char *last_line = strrchr(recvBuf, '\n') + 1;

  req_info req = tokenize(recvBuf);

  printf("%s, %s\n", req.method, req.route);

  char *token = req.route;

  char *response = route(req.route, req.method);

  if (strcmp(response, SEND_RESPONSE) == 0) {
    FILE *db = fopen(MSGS_PATH, "a");
    fprintf(db, "%s\n", last_line);
    fclose(db);
  }

  sendData(clientSocket, req.method, response);

  close(*clientSocket);
  free(clientSocket);

  return NULL;
}

int main() {
  int serverSock;
  if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("couldnt initialize server socket");
    return -1;
  }

  int opt = 1;
  setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in serverAddr;

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);

  if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    perror("cant bind");
    return -1;
  }

  if (listen(serverSock, 5) < 0) {
    perror("cant listen man");
    return -1;
  }

  printf("listening on port  %d\n", PORT);

  while (1) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int *clientSocket = malloc(sizeof(int));

    if ((*clientSocket = accept(serverSock, (struct sockaddr *)&clientAddr,
                                &clientLen)) < 0) {
      perror("couldnt accept client connection");
      continue;
    }
    pthread_t thread;
    pthread_create(&thread, NULL, handleClient, clientSocket);
    pthread_detach(thread);
  }
  close(serverSock);
  return 0;
}
