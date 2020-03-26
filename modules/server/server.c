#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <util.h>

#include "server.h"
#include <extern.h>

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

const char style[];
const size_t style_len;

int sock;
struct sockaddr_in svr_addr, cli_addr;
socklen_t sin_len = sizeof(cli_addr);

struct opts {
    size_t iunit;
    size_t ounit;
    int toc_level;
    hoedown_html_flags html_flags;
    hoedown_extensions extensions;
    size_t max_nesting;
};

int port = 6422;
char response[] = "";

void create_html_page()
{
    struct opts data;
    hoedown_buffer *ib, *ob;
    hoedown_renderer* renderer = NULL;
    hoedown_document* document;

    memset(&data, 0, sizeof(struct opts));

    data.iunit = DEF_IUNIT;
    data.ounit = DEF_OUNIT;
    data.max_nesting = DEF_MAX_NESTING;
    data.html_flags = HOEDOWN_HTML_USE_XHTML;

    ib = hoedown_buffer_new(data.iunit);

    if (hoedown_buffer_putf(ib, file))
        err(EXIT_FAILURE, "%s", fname);

    if (file != stdin)
        fclose(file);

    renderer = hoedown_html_renderer_new(data.html_flags, data.toc_level);

    ob = hoedown_buffer_new(data.ounit);
    document = hoedown_document_new(renderer, data.extensions, data.max_nesting);
    hoedown_document_render(document, ob, ib->data, ib->size);

    hoedown_buffer_free(ib);
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "HTTP/1.1 200 OK\r\n");
    abAppend(&ab, "Content-Type: text/html; charset=UTF-8\r\n\r\n");

    abAppend(&ab, "<!DOCTYPE html><html><head>");
    abAppend(&ab, "<meta charset=\"utf-8\">");
    abAppend(&ab, "<title>");
    abAppend(&ab, argv[0]);
    abAppend(&ab, " - mdv");
    abAppend(&ab, "</title>");

    abAppend(&ab, "<script>(function() {var link = document.querySelector(\"link[rel*='icon']\") || document.createElement('link');");
    abAppend(&ab, "link.type = 'image/x-icon'; link.rel = 'shortcut icon'; link.href = '../assets/favicon.ico';");
    abAppend(&ab, "document.getElementsByTagName('head')[0].appendChild(link);})();</script>");

    abAppend(&ab, "<style>\n.markdown-body { min-width: 200px; max-width: 950px; margin: 0 auto; padding: 30px;}\n");
    abAppend(&ab, style);
    abAppend(&ab, "</style>\n");
    abAppend(&ab, "</head>");

    abAppend(&ab, "<body>\n<article class=\"markdown-body\">\n");
    abAppend(&ab, ob->data);
    abAppend(&ab, "</article></body></html>\r\n");
    hoedown_buffer_free(ob);

    abFree(&ab);
}

void update_html(struct abuf* ab)
{
    int client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char* month = get_month_name(tm.tm_mon + 1);
    printf("127.0.0.1:%d - - [ %d-%s-%02d %02d:%02d:%02d] 'GET'\n", port, tm.tm_mday, month, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    if (client_fd == -1) {
        perror("Can't accept");
        //continue;
    }

    write(client_fd, ab->b, ab->len); /*-1:'\0'*/
    close(client_fd);
}

void run_server()
{
    int one = 1;
    sock = socket(AF_INET, SOCK_STREAM, 0);
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

    //while (1) {
    //}
}
