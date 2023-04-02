Barbuceanu Constantin - 322CB
Tema3 - Protocoale de comunicatie - Client Web - Comunicatie cu REST API

# 1) OBIECTIV.

Obiectivul temei este implementarea unui client in c/c++ care sa interactioneze
cu un server deja implementat care foloseste un REST API. Tema are in vedere
intelegerea comunicarii prin HTTP si a unor concepte des folosite precum 
json, sesiune, token jwt, cookie etc..

# 2) DESCRIEREA GENERALA A IMPLEMENTARII.

Clientul primeste comenzi de la tastatura. Comenzile posibile sunt: register,
login, enter_libary, get_books, get_book, add_book, delete_book, logout, exit.
In functie de comanda clientul realizeaza un request pe care il trimite catre
server. Acesta poate fi de tip POST, GET sau DELETE. In functie de comanda data,
se vor citi tot de la tastatura date de intrare specifice precum credentialele
unui user, datele unei carti sau id-ul unei carti, toate acestea fiind obligate
sa respecte formatarea impusa in cod. Daca un camp introdus de la tastatura nu
respecta formatarea, programul va cere sa introduceti din nou datele, in continuu
pana la introducerea corecta a acestora. In urma citirii comenzii si a datelor de 
intrare clientul va forma obiectul json sub forma unui string (vector de char)
doar daca este necesar si va trimite cererea catre server.

# 3) OBSERVATII IMPORTANTE.

    ****************
    * Parsarea JSON:
    ****************

Tema am scris-o in C. Nu am folosit nico biblioteca specifica pentru parsarea
mesajelor json (parson in speta), fapt care am vazut ca este permis in enunt.
Prin urmare am implementat singur parsarea mesajelor json acolo unde a fost
necesar. Am ales sa fac asta, intrucat documentatia bibliotecii parson pentru C
mi s-a parut mai subtire spre deosebire de C++ de exemplu. Practic pe moment nu
mi-a placut ideea de a folosi niste functii care nu stiu exact cum sunt
implementate si cum functioneaza. Ca sa inteleg toate dedesubturile ar fi fost
necesar un efort suplimentar de intelegere/exersare/documentare pe care nu am 
reusit sa il depun acum. In plus, consider ca implementarea unei solutii proprii
m-a adus mai aproape de intelegerea acestora pe viitor, intrucat astfel mi s-a
format o intuitie necesara a algoritmilor si ideilor necesare implementarii unor 
functii de parsare json. Locurile unde a trebuit sa aplic parsare json au fost:

    -> extract_token() : pt. a extrage tokenul jwt.
    -> compute_post_request() : pt. a crea un mesaj de format json sub forma de string
                                in care sa pun date introduse de la tastatura(date de
                                autentificare, date despre carte, id carte).

Am mai facut de asemenea si parsare pe headerele din response, cum ar fi ca sa extrag
cookie-ul primit la login.

    ****************
    * Modularizare:
    ****************

Am incercat sa modularizez cat se poate de mult programul. Am scris functii pe care
le reutilizez in mai multe parti diferite din program. Ideea dupa care m-am ghidat a fost
sa scriu functii usor de citit, scurte, usor de updatat si reutilizabile. Asta este motivul
pentru care functiile principale (register, login, enter_library, add_book, delete_book,
get_book, get_books, logout) sunt atat de scurte, intrucat am exportat partial functionalitatea
acestora in alte functii.

    ********************************
    * Logica si ordinea comenzilor:
    ********************************

Comenzile respecta o anumita logica si ordine obligatorie. Astfel este garantat ca pentru a
putea da comenzile (get_book, get_books, add_book, delete_book, logout) utilizatorul trebuie sa fie
logat(cookie de sesiune) si sa aiba acces la biblioteca(token_jwt obtinut in urma enter_library).
Daca nu se respecta ordinea comenzilor se va afisa un mesaj de eroare si se va cere reintroducerea
comenzii. In plus pt. a putea da enter_library trebuie in prealabil login. De asemenea, pt. a putea
da exit utilizatorul este obligat sa dea mai intai logout.

    *********************************************
    * Respectarea formatului datelor de intrare:
    *********************************************

Date de intrare care se cer de la tastatura sunt: username, password, author, title, genre, page_count,
publisher, id. Am impus restrictiile de formatare necesare asupra acestora. Daca o restrictie este
incalcate se cere reintroducerea datelor pana la forma corecta. De exemplu:
    -> id-ul nu poate avea prima cifra 0 sau alte caractere in afara de cifre
    -> username-ul nu poate contine spatii sau incepe cu cifra
    -> parola nu poate contine spatii
    -> titlul, autorul, genul, publisher-ul nu pot incepe cu spatiu
    -> si alte cateva

# 4) STRUCTURA GENERALA A PROGRAMULUI.

In main exista un while din care se iese doar cand se primeste exit. In rest la fiecare comanda
primita se deschide conexiunea, se apeleaza functia specifica si la intoarcere se inchide conexiunea.
De asemenea se verifica variabilele booleene logged_in si access_to_library.

Functiile asociate comenzilor sunt:
    -> enroll (comanda register), enter_library, get_book, add_book, get_books, delete_book, logout.

In fiecare se apeleaza:
    -> allocate_data_matrix (eventual)
    -> read_data_matrix_rows (eventual)
    -> compute_***_request (post, delete, get)
    -> send_to_server
    -> receive_from_server
    -> show_message_from_server
    -> free_memory
    
Acestea sunt functiile principale care asigura scheletul programului. Pe langa acestea mai sunt alte
cateva cu roluri cheie cum ar fi extract_cookie sau extract_token.