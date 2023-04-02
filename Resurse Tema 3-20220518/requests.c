#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_delete_request(char *host, char *url, char *cookie, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    if(!message || !line) {
        error("Eroare la alocarea bufferelor in compute_delete_request.\n");
    }

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if(cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message, line);
    }

    if(token != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message,line);
    }

    compute_message(message, "");
    return message;
}

char *compute_get_request(char *host, char *url, char *query_params, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    if(!message || !line) {
        error("Eroare la alocarea bufferelor in compute_get_request.\n");
    }

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if(cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message, line);
    }

    if(token != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message,line);
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request( char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    if(!message || !line || !body_data_buffer) {
        error("Eroare la alocarea bufferelor in \"compute_post_request\"");
    }

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    char *buff = (char*)calloc(LINELEN, sizeof(char*));
    if(!buff) {
        error("Eroare la alocarea buff in \"compute_post_request\"");
    }
    int i;

    sprintf(buff, "{\"%s\"", body_data[0]);
    strcat(body_data_buffer, buff);
    for (i = 1; i < body_data_fields_count; i++) {
        if (i % 2 == 1) {
            sprintf(buff, ":\"%s\"", body_data[i]);
        } else if (i % 2 == 0) {
            sprintf(buff, ",\"%s\"", body_data[i]);
        }
        strcat(body_data_buffer, buff);
    }
    sprintf(buff, "}");
    strcat(body_data_buffer, buff);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message,line);
    }

    if (token != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message,line);
    }

    compute_message(message, "");
    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);

    return message;
}
