# Tema 2 algoritmi parareli si distribuiti - Map Reduce

334CA Tudorache Bogdan Mihai

Tema contine implementarea unui **pipeline map reduce** pentru rezolvarea unei probleme de **procesare concurenta de fisiere**.

Pararelizarea programului s-a realizat prin urmatoarele:

1. Construirea unor task-uri de tip **map** si pasarea lor unui pool de thread-uri pentru rezolvare
2. Separarea celor doua grupe de task-uri pentru a asigura ca etapa de map se intampla inainte de reduce
3. Construirea unor task-uri de tip **reduce** si pasarea lor unui pool de thread-uri pentru rezolvare

O parte importanta a problemie a fost tratarea situatiei in care dimensiunile de start si de finish de prelucrare a unui 
fisier intr-un task de tip map se gasesc in mijlocul unor cuvinte.

## Descrierea metodelor de pararelizare

1. Task-urile de tipul map au fost construite si pasate catre un executor service pentru rezolvare. Toate aceste task-uri
pot fi rezolvate individual, asa ca nu a fost nevoie de alte elemente de sincronizare, in afara de blocarea executiei pana 
   cand toate task-urile au fost rezolvate. De asemenea, am folosit o lista sincronizata pentru a grupa toate rezultatele 
   task-urilor la un loc si evitarea conditiilor de cursa.

2. Dupa ce toate task-urile map au terminat, vom prelucra din nou rezultatele in thread-ul principal pentru a putea fi pasate
catre task-urile reduce. Aceasta etapa tine doar de metoda ```awaitTermination``` expusa de ```ExecutorService```, care 
   functioneaza ca o bariera pentru cele doua tipuri de task-uri.

3. Task-urile de tipul reduce au fost construite pe baza rezultatelor de la map si pasate tot unui executor service. Aici
s-au combinat si prelucrat argumentele conform cerintei. Pe baza rezultatelor din aceasta etapa, putem construi datele finale
   ale problemei.

## Tratarea situatiilor speciale din task-urile map

Pentru a vedea daca dimensiunile de start si de finish ale unui map task se suprapun cu un cuvant, am ales sa citesc din fisier
mai mult decat dimenisunea specificata. Am citit cu o litera inainte de index-ul de start, si cu un numar de circa 15 litere
peste finish. 

In cazul in care la indexul start - 1 avem o litera si start se afla de asemenea pe o litera, inseamna ca
trebuie sa inaintam index-ul de start pentru a evita cuvantul.

Asemanator pentru index-ul de end, daca avem litera la end si end + 1, trebuie sa inaintam index-ul de end pentru a include
si cuvantul curent in map task.