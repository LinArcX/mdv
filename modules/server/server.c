#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "server.h"

int port = 6422;

char response[] = "";

void run_server(struct abuf* ab)
{
    //printf(response);
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    printf("* Running on http://localhost:%d/ (Press CTRL+C to quit) \n", port);

    struct abuf bf_browser = ABUF_INIT;
    abAppend(&bf_browser, "firefox");
    abAppend(&bf_browser, " ");
    abAppend(&bf_browser, "http://localhost:");
    //char* c = "";
    char c[5];
    snprintf(c, 5, "%d", port);
    abAppend(&bf_browser, c);
    system(bf_browser.b);
    abFree(&bf_browser);

    while (1) {
        client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char* month = get_month_name(tm.tm_mon + 1);
        printf("127.0.0.1:%d - - [ %d-%s-%02d %02d:%02d:%02d] 'GET'\n", port, tm.tm_mday, month, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        write(client_fd, ab->b, ab->len); /*-1:'\0'*/
        close(client_fd);
    }
}
