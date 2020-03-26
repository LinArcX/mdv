#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <extern.h>

#include <server.h>
#include <util.h>

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

const char style[];
const size_t style_len;

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
        goto END;

    argc -= optind;
    argv += optind;

    if (argc > 0) {
        fname = argv[0];
        if (NULL == (file = fopen(fname, "r")))
            err(EXIT_FAILURE, "%s", fname);
    } else {
        fprintf(stdout, "Please pass your markdown file as first argument: mdv <foo.md>");
        exit(1);
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

    run_server(&ab);
    abFree(&ab);

    return (EXIT_SUCCESS);
END:
    return (EXIT_FAILURE);
}
