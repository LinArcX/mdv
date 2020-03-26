#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <extern.h>

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

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

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }
struct abuf {
    char* b;
    int len;
};

void abAppend(struct abuf* ab, const char* s)
{
    char* new = realloc(ab->b, ab->len + strlen(s));
    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, strlen(s));
    ab->b = new;
    ab->len += strlen(s);
}

void abFree(struct abuf* ab)
{
    free(ab->b);
}

char* get_month_name(int month_number)
{
    //char* month_name = "";
    switch (month_number) {
    case 1:
        return "Jan";
        break;
    case 2:
        return "Feb";
        break;
    case 3:
        return "Mar";
        break;
    case 4:
        return "Apr";
        break;
    case 5:
        return "May";
        break;
    case 6:
        return "Jun";
        break;
    case 7:
        return "Jul";
        break;
    case 8:
        return "Aug";
        break;
    case 9:
        return "Sep";
        break;
    case 10:
        return "Oct";
        break;
    case 11:
        return "Nov";
        break;
    case 12:
        return "Dec";
        break;
    default:
        return "!";
        break;
    }
}

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

//////////
struct opts {
    size_t iunit;
    size_t ounit;
    int toc_level;
    hoedown_html_flags html_flags;
    hoedown_extensions extensions;
    size_t max_nesting;
};

int main(int argc, char* argv[])
{
    struct opts data;
    FILE* file = stdin;
    const char* fname = "<stdin>";
    hoedown_buffer *ib, *ob;
    hoedown_renderer* renderer = NULL;
    hoedown_document* document;

    memset(&data, 0, sizeof(struct opts));

    data.iunit = DEF_IUNIT;
    data.ounit = DEF_OUNIT;
    data.max_nesting = DEF_MAX_NESTING;
    data.html_flags = HOEDOWN_HTML_USE_XHTML;

    if (-1 != getopt(argc, argv, ""))
        goto usage;

    argc -= optind;
    argv += optind;

    if (argc > 0) {
        fname = argv[0];
        if (NULL == (file = fopen(fname, "r")))
            err(EXIT_FAILURE, "%s", fname);
    }

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

    /////
    FILE* file_pointer;
    char c;

    file_pointer = fopen("./assets/style.css", "r"); //reset the pointer
    struct abuf ab_assets = ABUF_INIT;
    while ((c = getc(file_pointer)) != EOF) {
        abAppend(&ab_assets, &c);
    }
    fclose(file_pointer);
    //printf("%s", ab_assets.b);

    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "HTTP/1.1 200 OK\r\n");
    abAppend(&ab, "Content-Type: text/html; charset=UTF-8\r\n\r\n");

    abAppend(&ab, "<!DOCTYPE html><html><head>");
    abAppend(&ab, "<meta charset=\"utf-8\">");
    abAppend(&ab, "<link rel = \"icon\" href = \"../assets/favicon.ico\">");
    abAppend(&ab, "<title>");
    abAppend(&ab, argv[0]);
    abAppend(&ab, " - mdv");
    abAppend(&ab, "</title>");
    //abAppend(&ab, "<link rel=\"stylesheet\" href=\"../assets/style.css\">");
    abAppend(&ab, "<style>\n.markdown-body { min-width: 200px; max-width: 950px; margin: 0 auto; padding: 30px;}\n");
    abAppend(&ab, ab_assets.b);
    abAppend(&ab, "</style>\n");
    abAppend(&ab, "</head>");

    abAppend(&ab, "<body>\n<article class=\"markdown-body\">\n");
    abAppend(&ab, ob->data);
    abAppend(&ab, "</article></body></html>\r\n");
    //printf("%s", ab.b);

    //file_pointer = fopen("/home/linarcx/completed.txt", "r");
    //printf("----read a line----\n");
    //fgets(buffer, 50, file_pointer);
    //printf("%s\n", buffer);

    //printf("----read and parse data----\n");
    //file_pointer = fopen("./assets/style.css", "r"); //reset the pointer
    //char str1[10], str2[2], str3[20], str4[2];
    //fscanf(file_pointer, "%s %s %s %s", str1, str2, str3, str4);
    //printf("Read String1 |%s|\n", str1);
    //printf("Read String2 |%s|\n", str2);
    //printf("Read String3 |%s|\n", str3);
    //printf("Read String4 |%s|\n", str4);

    //printf("----read the entire file----\n");

    hoedown_buffer_free(ob);

    run_server(&ab);
    abFree(&ab);
    abFree(&ab_assets);

    return (EXIT_SUCCESS);
usage:
    return (EXIT_FAILURE);
}
