#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#define IP_SERVER "34.241.4.235"
#define PAYLOAD_TYPE "application/json"

int sock;
char *c;
bool logged_in, access_to_library;
char *cookie;
char *token_jwt;

void free_memory(char *m, char *r, char **d, int nr) {
    int i;
    if(d != NULL) {
        for(i = 0; i < nr; i++)
            if(d[i] != NULL)
                free(d[i]);
        free(d);
    }
    if(m != NULL) free(m);
    if(r != NULL) free(r);
}

void extract_cookie(char *r) {
    int i;
    char *p, *cp;
    p = strstr(r, "connect.sid");
    cp = p;
    if(p != NULL) {
        i = 0;
        while(p[i] != ';') {
            ++i;
        }
        p[i] = 0;
    }
    strcpy(cookie, cp);
}

void extract_token(char *r) {
    char *p1 = strstr(r, "{\"token\":");
    p1 += 10;
    char *p2 = p1;
    while(*p2 != '"') {
        ++p2;
    }
    *p2 = 0;
    strcpy(token_jwt, p1);
}

char **allocate_data_matrix(int nr) {
    int i;
    char **d = (char **) malloc(nr * sizeof(char *));
    if(d == NULL) { error("Eroare la alocarea campurilor de date."); }

    for(i = 0; i < nr; i++) {
        d[i] = (char *) malloc(sizeof(char));
        if(d[i] == NULL) { error("Eroare la alocarea unui camp de date."); }
    }

    return d;
}

void read_credentials(char **d) {
    strcpy(d[0], "username");
    strcpy(d[2], "password");
    
    while(1) {
        printf("username = "); scanf("%s", d[1]);
        if(strchr(d[1], ' ') != NULL) {
            printf("Username - ul nu poate contine spatii. Va rugam introduceti username-ul din nou.\n");
        } else if(strchr("0123456789", d[1][0]) != NULL) {
            printf("Username-ul nu poate incepe cu o cifra. Va rugam introduceti username-ul din nou.\n");
        } else {
            break;
        }
    }

    while(1) {
        printf("password = "); scanf("%s", d[3]);
        if(strchr(d[1], ' ') != NULL) {
            printf("Parola nu poate contine spatii. Va rugam introduceti parola din nou.\n");
        } else {
            break;
        }
    }
}

void read_book_details(char **d) {
    strcpy(d[0], "title");
    strcpy(d[2], "author");
    strcpy(d[4], "genre");
    strcpy(d[6], "page_count");
    strcpy(d[8], "publisher");

    //Citeste '\n' ramas de la ultimul scanf.
    fgets(d[1], LINELEN, stdin);

    while(1) {
        printf("title = "); fgets(d[1], LINELEN, stdin); d[1][strlen(d[1]) - 1] = 0;
        if(d[1][0] == ' ') {
            printf("Titlul nu poate incepe cu spatiu. Va rugam reintroduceti titlul.\n");
        } else break;
    }

    while(1) {
        printf("author = "); fgets(d[3], LINELEN, stdin); d[3][strlen(d[3]) - 1] = 0;
        char *p = d[3];
        bool ok = false;
        //Numele nu are voie sa inceapa cu spatiu.
        if(*p == ' ') {
            printf("Numele autorului nu poate incepe cu spatiu. Va rugam reintroduceti numele.\n");
            continue;
        }
        while(*p != '\0') {
            //Daca nu este litera sau spatiu se cere introducerea din nou a autorului.
            if( !( (*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122) || *p == ' ' ) ) {
                printf("Numele autorului nu poate contine decat litere sau spatii. Va rugam reintroduceti numele.\n");
                ok = true;
                break;
            }
            ++p;
        }
        if(!ok) break;
    }

    while(1) {
        printf("genre = "); fgets(d[5], LINELEN, stdin); d[5][strlen(d[5]) - 1] = 0;
        char *p = d[5];
        bool ok = false;
        if(d[5][0] == ' ') {
            printf("Genul nu poate incepe cu spatiu. Va rugam reintroduceti numele genului.\n");
            continue;
        }
        while(*p != '\0') {
            //Daca nu este litera sau spatiu se cere introducerea din nou a genului.
            if( !( (*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122) || *p == ' ' ) ) {
                printf("Numele autorului nu poate contine decat litere sau spatii. Va rugam reintroduceti numele.\n");
                ok = true;
                break;
            }
            ++p;
        }
        if(!ok) break;
    }

    while(1) {
        printf("page_count = "); fgets(d[7], LINELEN, stdin); d[7][strlen(d[7]) - 1] = 0;
        char *p = d[7];
        bool ok = false;
        while(*p != '\0') {
            //Daca un caracter nu este cifra atunci page_count este invalid.
            if( strchr("0123456789", *p) == NULL ) {
                printf("page_count invalid. Toate caracterele trebuie sa fie cifre. Va rugam reintroduceti numarul de pagini.\n");
                ok = true;
                break;
            }
            ++p;
        }
        if(!ok) break;
    }

    while(1) {
        printf("publisher = "); fgets(d[9], LINELEN, stdin); d[9][strlen(d[9]) - 1] = 0;
        char *p = d[9];
        bool ok = false;
        if(d[9][0] == ' ') {
            printf("Numele editurii nu poate incepe cu spatiu. Va rugam reintroduceti editura.\n");
        }
        while(*p != '\0') {
            //Daca nu este litera sau spatiu se cere introducerea din nou a editurii.
            if( !( (*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122) || *p == ' ' || *p == '.' ) ) {
                printf("Numele editurii nu poate contine decat litere, spatii sau punct. Va rugam reintroduceti editura.\n");
                ok = true;
                break;
            }
            ++p;
        }
        if(!ok) break;
    }
}

void read_book_id(char **d) {
    strcpy(d[0], "id");

    //Citeste '\n' ramas de la ultimul scanf.
    fgets(d[1], LINELEN, stdin);

    while(1) {
        printf("id = "); fgets(d[1], LINELEN, stdin); d[1][strlen(d[1]) - 1] = 0;
        char *p = d[1];
        bool ok = false;
        if(p[0] == '0') {
            printf("Prima cifra a id-ului nu poate fi 0. Introduceti alt id.\n");
            continue;
        }
        while(*p != '\0') {
            //Daca un caracter nu este cifra atunci id este invalid.
            if( strchr("0123456789", *p) == NULL ) {
                printf("id invalid. Toate caracterele trebuie sa fie cifre. Va rugam reintroduceti id-ul.\n");
                ok = true;
                break;
            }
            ++p;
        }
        if(!ok) break;
    }
}

void read_data_matrix_rows(char **d, char *command) {
    if(!strcmp(command, "register") || !strcmp(command, "login")) {
        read_credentials(d);
    } else if(!strcmp(command, "add_book")) {
        read_book_details(d);
    } else if(!strcmp(command, "get_book") || !strcmp(command, "delete_book")) {
        read_book_id(d);
    }
}

void show_error_from_server(char *c, char *r) {
    char *error_message = strstr(r, "{");

    if(!strcmp(c, "register")) {
        if(error_message != NULL) {
		    printf("%s -> Username-ul este deja folosit de catre cineva.\n", error_message);
        } else {
            error("Raspunsul serverului nu poate fi parsat.\n");
        }
    } else if(!strcmp(c, "login")) {
        if(error_message != NULL) {
	        printf("%s -> Credentialele nu se potrivesc (parola sau username-ul gresite.)\n", error_message);
        } else {
            error("Raspunsul serverului nu poate fi parsat.\n");
        }
    }  else if(!strcmp(c, "logout") || !strcmp(c, "enter_library")) {
        if(error_message != NULL) {
		    printf("%s -> Trebuie sa demonstrati ca sunteti autentificati (cookie lipsa) !!!\n", error_message);
        } else {
            error("Raspunsul serverului nu poate fi parsat.\n");
        }
    } else if(!strcmp(c, "add_book") || !strcmp(c, "get_book") || !strcmp(c, "get_books")) {
        if(error_message != NULL) {
		    printf("%s\n", error_message);
        } else {
            error("Raspunsul serverului nu poate fi parsat.\n");
        }
    }
}

void show_books(char *r) {
    char *payload = strstr(r, "[");
    if(payload == NULL) {
        error("Eroare la parsarea payload-ului in show_books.\n");
    }

    char *id, *title, *p, *p_end;
    id = (char *)malloc(20); title = (char *)malloc(100);
    if(id == NULL || title == NULL) {
        error("Eroare la alocarea id-ului/titlului in show_books.\n");
    }

    int nr_books = 0;
    p = payload;
    while(*p != '\0'){
        if(*p == '{') ++nr_books;
        ++p;
    }

    p = payload;
    while(p != NULL) {
        p = strstr(p, "{");
        if(p == NULL) break;
        p = strstr(p, ":") + 1; //Ma plaseaza pe prima cifra a id-ului.
        p_end = p;
        while(*p_end != ',') ++p_end;
        *p_end = 0;
        strcpy(id, p);
        p = p_end + 1;
        p += 9;                 //Ma plaseaza pe prima litera a titlului
        p_end = p;
        while(*p_end != '"') ++p_end;
        *p_end = 0;
        strcpy(title, p);
        p = p_end + 1;
        printf("%s   --->   %s\n", id, title);
    }

    free(id); free(title);
}

void parse_book_fields(char *r) {
    char *payload = strstr(r, "[");
    if(payload == NULL) {
        error("Eroare la parsarea payload-ului in show_books.\n");
    }

    printf("##############################################\n");
    printf("Datele cartii descarcate sunt: \n\n");

    char *p = strtok(payload, ",");
    p += 2;
    printf("%s\n", p);  //title
    p = strtok(NULL, ",");
    printf("%s\n", p);  //author
    p = strtok(NULL, ",");
    printf("%s\n", p);  //publisher
    p = strtok(NULL, ",");
    printf("%s\n", p);  //genre
    p = strtok(NULL, ",");
    p[strlen(p) - 2] = 0;
    printf("%s\n", p);  //page_count
    p = strtok(NULL, ",");
    printf("##############################################\n");
}

void show_message_from_server(char *command, char *r, char **d) {
    char status_code_from_server[15];
    memset(status_code_from_server, 0, 15);

    if(!strcmp(command, "register")) {

        strncpy(status_code_from_server, r + 9, 11);  
        if(!strcmp(status_code_from_server, "201 Created")) { 
            printf("%s : Clientul %s a fost inrolat cu succes.\n", status_code_from_server, d[1]);
        } else {
            show_error_from_server("register", r);
        }

    } else if(!strcmp(command, "login")) {

        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s : Logare acceptata pentru clientul %s.\n", status_code_from_server, d[1]);
        } else {
            show_error_from_server("login", r);
        }

    } else if(!strcmp(command, "logout")) {

        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s : Delogare finalizata.\n", status_code_from_server);
        } else {
            show_error_from_server("logout", r);
        }

    } else if(!strcmp(command, "enter_library")) {
        
        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s : Accesul in biblioteca va este permis. Bine ati venit !\n", status_code_from_server);
        } else {
            show_error_from_server("enter_library", r);
        }

    } else if(!strcmp(command, "add_book")) {

        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s : Cartea cu titlul \"%s\", scrisa de %s a fost introdusa cu succes in biblioteca!\n", status_code_from_server, d[1], d[3]);
        } else {
            show_error_from_server("get_book", r);
        }

    } else if(!strcmp(command, "get_book")) {

        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s : Cartea cu id-ul %s a fost downloadata cu succes.\n", status_code_from_server, d[1]);
            parse_book_fields(r);
        } else {
            show_error_from_server("get_book", r);
        }

    } else if(!strcmp(command, "get_books")) {

        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s -> Lista cartilor din biblioteca este:\n", status_code_from_server);
            printf("##############################################\n");
            show_books(r);
            printf("##############################################\n");
        } else {
            show_error_from_server("get_books", r);
        }

    } else if(!strcmp(command, "delete_book")) {
        
        strncpy(status_code_from_server, r + 9, 6);
        if(!strcmp(status_code_from_server, "200 OK")) { 
            printf("%s -> Cartea cu id-ul %s a fost scoasa din biblioteca.\n", status_code_from_server, d[1]);
        } else {
            show_error_from_server("get_books", r);
        }

    }
}

void enroll() {
    int nr = 4;

    char **d = allocate_data_matrix(nr);
    read_data_matrix_rows(d, "register");

    char *m = compute_post_request(IP_SERVER, "/api/v1/tema/auth/register", PAYLOAD_TYPE, d, nr, NULL, NULL);
    send_to_server(sock, m);
    char *r = receive_from_server(sock);
    show_message_from_server("register", r, d);

    free_memory(m, r, d, nr);
}

void login() {
    int nr = 4;
    char **d = allocate_data_matrix(nr);
    read_data_matrix_rows(d, "login");

    char *m = compute_post_request(IP_SERVER, "/api/v1/tema/auth/login", PAYLOAD_TYPE, d, nr, NULL, NULL);
    send_to_server(sock, m);
    char *r = receive_from_server(sock);
    show_message_from_server("login", r, d);
    extract_cookie(r);
    /*
        Da segfault daca bag credentialele gresite pentru ca in cazul ala nu mai trebuie sa extrag niciun cookie.
        Deci practic trebuie ca show_message_from_server sa nu mai fie void si sa intoarca valoare de eroare sau
        de validare.
    */
    free_memory(m, r, d, nr);
}

void logout() {
    char *m, *r;

    m = compute_get_request(IP_SERVER, "/api/v1/tema/auth/logout", NULL, cookie, NULL);
	send_to_server(sock, m);
	r = receive_from_server(sock);
    show_message_from_server("logout", r, NULL);
    free_memory(m, r, NULL, 0);
}

void enter_library() {
    char *m, *r;

    m = compute_get_request(IP_SERVER, "/api/v1/tema/library/access", NULL, cookie, NULL);
	send_to_server(sock, m);
	r = receive_from_server(sock);
    show_message_from_server("enter_library", r, NULL);
    extract_token(r);
    free_memory(m, r, NULL, 0);
}

void add_book() {
    int nr = 10;

    char **d = allocate_data_matrix(nr);
    read_data_matrix_rows(d, "add_book");

    char *m = compute_post_request(IP_SERVER, "/api/v1/tema/library/books", PAYLOAD_TYPE, d, nr, cookie, token_jwt);
    send_to_server(sock, m);
    char *r = receive_from_server(sock);
    show_message_from_server("add_book", r, d);
    free_memory(m, r, d, nr);
}

void get_book() {
    int nr = 2;

    char **d = allocate_data_matrix(nr);
    read_data_matrix_rows(d, "get_book");

    char *url = (char *) malloc(100);
    if(url == NULL) {
        error("Eroare la alocarea url-ului in get_book.\n");
    }

    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, d[1]);
    char *m = compute_get_request(IP_SERVER, url, NULL, cookie, token_jwt);
    send_to_server(sock, m);
	char *r = receive_from_server(sock);
    show_message_from_server("get_book", r, d);

    free_memory(m, r, d, nr);
    free(url);
}

void get_books() {
    char *m, *r;

    m = compute_get_request(IP_SERVER, "/api/v1/tema/library/books", NULL, cookie, token_jwt);
	send_to_server(sock, m);
	r = receive_from_server(sock);
    show_message_from_server("get_books", r, NULL);
    free_memory(m, r, NULL, 0);
}

void delete_book() {
    char *m, *r;
    int nr = 2;

    char **d = allocate_data_matrix(nr);
    read_data_matrix_rows(d, "delete_book");

    char *url = (char *) malloc(100);
    if(url == NULL) {
        error("Eroare la alocarea url-ului in get_book.\n");
    }

    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, d[1]);
    m = compute_delete_request(IP_SERVER, url, cookie, token_jwt);
    send_to_server(sock, m);
	r = receive_from_server(sock);
    show_message_from_server("delete_book", r, d);

    free_memory(m, r, d, nr);
    free(url);
}

int main() {   
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    c = (char *) malloc(MAX_C_LEN);
    if (c == NULL) { error("Eroare la alocarea spatiului de memorie pentru comanda.\n"); }

    cookie = (char *)malloc(LINELEN);
    if(!cookie) { error("Eroare la alocarea spatiului de memorie pentru cookie.\n"); }

    token_jwt = (char *)malloc(LINELEN);
    if(!token_jwt) { error("Eroare la alocarea spatiului de memorie pentru cookie.\n"); }

    logged_in = false;
    access_to_library = false;
    while (1) {
        scanf("%s", c);

        if (!strcmp(c, "register")) {

            if(!logged_in) {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                enroll();
                close(sock);
            } else {
                printf("User-ul trebuie sa se delogheze pentru a putea fi inregistrat un nou client.\n");
            }

        } else if (!strcmp(c, "login")) {

            if(!logged_in) {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                login();
                if(cookie != NULL) {
                    logged_in = true;
                }
                close(sock);
            } else {
                printf("User-ul trebuie sa se delogheze pentru a se loga din nou.\n");
            }

        } else if (!strcmp(c, "enter_library")) {
            
            if(!logged_in) {
                printf("Comanda invalida. Clientul trebuie sa fie logat pentru a accesa biblioteca.\n");
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                enter_library();
                access_to_library = true;
                close(sock);
            }

        } else if (!strcmp(c, "get_books")) {
            
            if(!logged_in) {
                printf("Comanda invalida. Clientul nu este logat.\n");
            } else if(!access_to_library) {
                printf("Comanda invalida. Clientul nu are acces la biblioteca. Apasati enter_library.\n");
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                get_books();
                close(sock);
            }

        } else if (!strcmp(c, "get_book")) {
            
            if(!logged_in) {
                printf("Comanda invalida. Clientul nu este logat.\n");
            } else if(!access_to_library) {
                printf("Comanda invalida. Clientul nu are acces la biblioteca. Apasati enter_library.\n");
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                get_book();
                close(sock);
            }

        } else if (!strcmp(c, "add_book")) {

            if(!logged_in) {
                printf("Comanda invalida. Clientul nu este logat.\n");
            } else if(!access_to_library) {
                printf("Comanda invalida. Clientul nu are acces la biblioteca. Apasati enter_library.\n");
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                add_book();
                close(sock);
            }
            
        } else if (!strcmp(c, "delete_book")) {
            
            if(!logged_in) {
                printf("Comanda invalida. Clientul nu este logat.\n");
            } else if(!access_to_library) {
                printf("Comanda invalida. Clientul nu are acces la biblioteca. Apasati enter_library.\n");
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                delete_book();
                close(sock);
            }

        } else if (!strcmp(c, "logout")) {

            if(!logged_in) {
                printf("Comanda invalida. User-ul nu este logat.\n");
                continue;
            } else {
                sock = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                logout();
                close(sock);
            }
        
            logged_in = false;
            access_to_library = false;

        } else if (!strcmp(c, "exit")) {

            if(logged_in) {
                printf("Pentru a opri sesiunea, clientul trebuie mai intai sa dea logout.\n");
                continue;
            }
            break;
            
        } else {
            printf("Comanda introdusa este invalida.\n");
        }
    }

    close(sock);
    free(c);
    free(cookie);
    free(token_jwt);

    return 0;
}