# Tema 3 algoritmi parareli si distribuiti - Calcule colaborative in sisteme distribuite

334CA Tudorache Bogdan Mihai

Tema contine coordonarea unui **program distribuit** cu o topologie formata din 3 clustere. 
Aceste clustere trebuie sa comunice eficient in scopul dublarii elementelor unui vector.

Tema este formata din doua etape: 

1. Stabilirea topologiei
2. Realizarea calculelor

## Descrierea etapelor

1. Stabilirea topologiei se realizeaza prin coordonarea **proceselor coordonator** si **proceselor worker**:
    * Procesele coordonator citesc fisierul ce contine datele de intrare pentru clusterul respectiv. 
      Apoi, fiecare dintre procesele coordonator isi trimit unul celuilalt datele pe care le-a obtinut in pasul anterior. 
      Astfel, fiecare proces coordonator cunoaste actualmente intreaga topologie. Mai ramane ca datele despre topologie sa fie trimise spre workeri. 
    * Procesele worker afla prima data care este procesul parinte print-un mesaj primit de la coordonatorul clusterului, apoi primesc intreaga topologie de la coordonatorul lor.

2. Realizarea calculelor se face prin coordonarea **procesului root**, **celelalte procese coordonator** si **procelese worker**. 
Stabilirea a "ce parte din array" sa prelucreze fiecare worker se face printr-un procedeu de **start/end** asemanator cu cel de la programele pararele:
    * Procesul root calculeaza vectorul in starea lui initiala, trimite acest vector la ceilalti coordonatori si trimite fiecare parte din array asociata fiecarui worker din proriul cluster. 
    In final, asteapta raspunsul cu fiecare bucata din array de la fiecare coordonator/worker in parte, si afiseaza rezultatul.
    * Celelalte doua procese coordonator primesc array-ul de la procesul root, dupa care trimit la fiecare worker din propriul cluster fiecare bucata asociata si asteapta raspunsul, iar in final trimite acest rezultat inapoi la root.
    * Procesele worker primesc de la coordonatorul lor fiecare bucata necesara pentru prelucrare, o dubleaza, dupa care o trimit inapoi de unde a venit.

