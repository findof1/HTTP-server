// gcc main.c -o main.exe -lws2_32

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

int main()
{
  WSADATA wsaData;
  int port = 8080;

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
    return 1;
  }

  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET)
  {
    printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  struct sockaddr_in saServer;

  saServer.sin_family = AF_INET;
  saServer.sin_addr.s_addr = INADDR_ANY;
  saServer.sin_port = htons(port);

  bind(s, (struct sockaddr *)&saServer, sizeof(saServer));

  printf("Listening on port %d\n", port);

  listen(s, 10);

  while (1)
  {
    SOCKET client = accept(s, NULL, NULL);
    if (client == INVALID_SOCKET)
    {
      printf("Accept failed. Error Code: %d\n", WSAGetLastError());
      closesocket(s);
      WSACleanup();
      return 1;
    }

    char request[256] = {0};

    recv(client, request, sizeof(request), 0);

    if (memcmp(request, "GET / ", 6) == 0)
    {
      FILE *f = fopen("index.html", "r");
      if (f)
      {
        char buffer[1024] = {0};
        fread(buffer, 1, sizeof(buffer), f);
        fclose(f);

        char httpHeader[] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Connection: close\r\n\r\n";
        send(client, httpHeader, sizeof(httpHeader) - 1, 0);

        send(client, buffer, strlen(buffer), 0);
      }
    }

    if (memcmp(request, "GET /home ", 10) == 0)
    {
      FILE *f = fopen("home.html", "r");
      if (f)
      {
        char buffer[1024] = {0};
        fread(buffer, 1, sizeof(buffer), f);
        fclose(f);

        char httpHeader[] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Connection: close\r\n\r\n";
        send(client, httpHeader, sizeof(httpHeader) - 1, 0);

        send(client, buffer, strlen(buffer), 0);
      }
    }

    closesocket(client);
  }

  closesocket(s);
  WSACleanup();

  return 0;
}
