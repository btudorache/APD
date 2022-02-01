# Tema 1 algoritmi parareli si distribuiti - Pararelizarea unui algoritm genetic

334CA Tudorache Bogdan Mihai

Tema contine pararelizarea unui **algoritm genetic** de rezolvare a **problemei rucsacului** scris in limbajul C prin diferite primitive de pararelizare oferite de biblioteca **Pthreads**.

Pararelizarea programului s-a realizat prin urmatoarele: 

1. distribuirea muncii uniform pe fiecare thread in parte
2. folosirea barierelor pentru a sincroniza thread-urile
3. implementarea unui merge-sort pararel

O optimizare care a imbunatatit foarte mult viteza de executie nu a tinut de pararelizare, 
ci de evitarea de a calcula acelasi lucru de 2 ori prin **caching**.

## Descrierea metodelor de pararelizare

1. Distribuirea muncii s-a realizat prin **impartiri de tipul start/end pe fiecare thread
 in parte**. Astfel, fiecare thread a putut sa isi efectueze calculele izolat, fara ca
  aceste operatii sa se intercaleze cu munca altor thread-uri

2. In momente cheie din algoritm, toate thread-urile sunt silite sa astepte celelalte
 thread-uri inainte de a continua executia, pentru a asigura corectitudinea algoritmului. 
 Acest lucru s-a implementat prin **folosirea barierelor** in cod. Momente din algoritm 
 in care s-au folosit bariere:
    * La inceputul iteratiei unei noi generatii;
    * Dupa apelarea functiei ```compute_fitness_function```;
    * Dupa aplicarea algoritmului de merge sort de catre fiecare thread in parte;
    * Dupa ce primul thread apeleaza ```merge_sections_of_array``` pentru a *imbina* partile sortate de fiecare thread in parte.

3. Pentru a folosi un algoritm de sortare eficient, am implementat un **algoritm de merge-sort**.
 Implementarea a fost inspirata de la [Aceasta adresa](https://malithjayaweera.com/2019/02/parallel-merge-sort/), insa cu modificarile necesare pentru a functiona in situatia noastra. 
 Algoritmul functioneaza in felul urmator:
   practic, fiecare thread face merge sort pe "bucata lui" (impartirile facute prin indicii start/end) prin functiile ```merge``` si ```merge_sort```. La final, unul dintre thread-uri "imbina" munca facuta de toate thread-urile, apeland functia ```merge_sections_of_array```.

## Optimizare prin caching

Pentru a evita **ingreunarea algoritmului de sortare prin functia de compare ineficienta**,
 putem optimiza ```cmpfunc``` prin folosirea rezultatului anterior calculat de ```compute_fitness_function```. 

Practic, ne dorim sa calculam "numarul de 1" din fiecare individ dintr-o generatie, si putem face acest lucru o singura data. Vom adauga de asemenea un camp *num_items* de tipul int pentru a cache-ui acest calcul, si pentru a-l folosi din nou in ```cmpfunc```.