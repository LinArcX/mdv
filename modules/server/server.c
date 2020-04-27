#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "server.h"
#include <extern.h>
#include <util.h>

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

int port = 6422;
const char style[];
const size_t style_len;
//char response[] = "";

struct opts {
    size_t iunit;
    size_t ounit;
    int toc_level;
    hoedown_html_flags html_flags;
    hoedown_extensions extensions;
    size_t max_nesting;
};

void update_html()
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(1);
    }
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    ////////////////////////

    ////int client_fd = accept(sockfd, (struct sockaddr*)&cli_addr, &sin_len);

    //time_t t = time(NULL);
    //struct tm tm = *localtime(&t);
    //char* month = get_month_name(tm.tm_mon + 1);
    //printf("127.0.0.1:%d - - [ %d-%s-%02d %02d:%02d:%02d] 'GET'\n", port, tm.tm_mday, month, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    ////if (client_fd == -1) {
    ////    perror("Can't accept");
    ////    //continue;
    ////}

    //FILE* file;
    //struct opts data;
    //hoedown_buffer *ib, *ob;
    //hoedown_renderer* renderer = NULL;
    //hoedown_document* document;

    //memset(&data, 0, sizeof(struct opts));

    //data.iunit = DEF_IUNIT;
    //data.ounit = DEF_OUNIT;
    //data.max_nesting = DEF_MAX_NESTING;
    //data.html_flags = HOEDOWN_HTML_USE_XHTML;

    //ib = hoedown_buffer_new(data.iunit);

    //if (NULL == (file = fopen(fname, "r")))
    //    err(EXIT_FAILURE, "%s", fname);

    //if (hoedown_buffer_putf(ib, file))
    //    err(EXIT_FAILURE, "!");

    ////if (file != stdin)
    //fclose(file);
    //printf("has");

    //renderer = hoedown_html_renderer_new(data.html_flags, data.toc_level);

    //ob = hoedown_buffer_new(data.ounit);
    //document = hoedown_document_new(renderer, data.extensions, data.max_nesting);
    //hoedown_document_render(document, ob, ib->data, ib->size);

    //hoedown_buffer_free(ib);
    //hoedown_document_free(document);
    //hoedown_html_renderer_free(renderer);

    //struct abuf ab = ABUF_INIT;
    //abAppend(&ab, "HTTP/1.1 200 OK\r\n");
    //abAppend(&ab, "Content-Type: text/html; charset=UTF-8\r\n\r\n");

    //abAppend(&ab, "<!DOCTYPE html><html><head>");
    //abAppend(&ab, "<meta charset=\"utf-8\">");
    //abAppend(&ab, "<title>");
    //abAppend(&ab, "title");
    //abAppend(&ab, " - mdv");
    //abAppend(&ab, "</title>");

    //abAppend(&ab, "<script>(function() {var link = document.querySelector(\"link[rel*='icon']\") || document.createElement('link');");
    //abAppend(&ab, "link.type = 'image/x-icon'; link.rel = 'shortcut icon'; link.href = '../assets/favicon.ico';");
    //abAppend(&ab, "document.getElementsByTagName('head')[0].appendChild(link);})();</script>");

    //abAppend(&ab, "<style>\n.markdown-body { min-width: 200px; max-width: 950px; margin: 0 auto; padding: 30px;}\n");
    //abAppend(&ab, style);
    //abAppend(&ab, "</style>\n");
    //abAppend(&ab, "</head>");

    //abAppend(&ab, "<body>\n<article class=\"markdown-body\">\n");
    //abAppend(&ab, ob->data);
    //abAppend(&ab, "</article></body></html>\r\n");
    //hoedown_buffer_free(ob);

    //printf("tam");
    ////write(client_fd, ab.b, ab.len); /*-1:'\0'*/
    //abFree(&ab);
    ////close(client_fd);
}

void run_server()
{
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[2000];

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //Bind
    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        exit(1);
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

gm:
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0) {
        perror("accept failed");
        exit(1);
    }
    puts("Connection accepted");

te:
    //Receive a message from client
    while ((read_size = read(client_sock, client_message, sizeof(client_message))) > 0) {
        //Send the message back to client
        write(client_sock, client_message, strlen(client_message));
        ////write(client_fd, ab.b, ab.len); /*-1:'\0'*/

        //        memset(&client_message, 0, sizeof(client_message));

        srand(time(NULL)); // Initialization, should only be called once.
        int r = rand();
        printf("%d", r);
    }
    if (read_size == 0) {
        goto te;
        //puts("Client disconnected");
        //fflush(stdout);
    } else if (read_size == -1) {
        goto te;
        perror("recv failed");
    } else {
        goto gm;
    }
}

//void _run_server()
//{
//    int one = 1, sockfd, new_socket, valread;
//    char buffer[1024] = { 0 };
//    struct sockaddr_in address;
//    int addrlen = sizeof(address);
//
//    char* server_info = "* Running on http://localhost (Press CTRL+C to quit) \n";
//
//    // create socket
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0) {
//        err(1, "can't open socket");
//    }
//
//    // set socket options
//    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(int)) == -1) {
//        err(1, "can't set options!");
//    }
//
//    // bind to address:port
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(port);
//
//    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) == -1) {
//        close(sockfd);
//        err(1, "Can't bind");
//    }
//
//    // listen
//    if (listen(sockfd, 5) < 0) {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//
//gm:
//    if ((new_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//
//    //////////////
//    //Receive a message from client
//    int read_size;
//    char client_message[2000];
//
//    while ((read_size = read(new_socket, client_message, sizeof(client_message))) > 0) {
//        //Send the message back to client
//        write(new_socket, client_message, strlen(client_message));
//        memset(&client_message, 0, sizeof(client_message));
//
//        srand(time(NULL)); // Initialization, should only be called once.
//        int r = rand();
//        printf("%d", r);
//    }
//    if (read_size == 0) {
//        //puts("Client disconnected");
//        goto gm;
//        //fflush(stdout);
//    } else if (read_size == -1) {
//        perror("recv failed");
//    }
//
//    ///////////////////
//    //valread = read(new_socket, buffer, 1024);
//    //printf("%s\n", buffer);
//    //send(new_socket, server_info, strlen(server_info), 0);
//
//    //printf("* Running on http://localhost:%d/ (Press CTRL+C to quit) \n", port);
//
//    //while (1) {
//    //    update_html();
//    //}
//
//    //struct abuf bf_browser = ABUF_INIT;
//    //abAppend(&bf_browser, "firefox");
//    //abAppend(&bf_browser, " ");
//    //abAppend(&bf_browser, "http://localhost:");
//    //char c[5];
//    //snprintf(c, 5, "%d", port);
//    //abAppend(&bf_browser, c);
//    //system(bf_browser.b);
//    //abFree(&bf_browser);
//}

//char* c = "";
//while (1) {
//}
//
//
//
//    //close(sock);

//int one = 1;
//sock = socket(AF_INET, SOCK_STREAM, 0);
//if (sock < 0)
//    err(1, "can't open socket");

//setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

//address.sin_family = AF_INET;
//address.sin_addr.s_addr = INADDR_ANY;
//address.sin_port = htons(port);

//if (bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
//    close(sock);
//    err(1, "Can't bind");
//}

//listen(sock, 5);
//////////////////
//
//
//
//
////struct abuf ab = ABUF_INIT;

//void create_html_page()
//{
//    //return ab;
//}
