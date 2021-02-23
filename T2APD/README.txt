#Sulimovici Raoul-Renatto 331CB

Pentru implementarea temei am sters interfata Intersection si am creat o clasa Intersection
care va contine detalii pentru toate tipurile de intersectie din tema. Initializez doar variabilele
de care am nevoie la fiecare task.

In intersectionFactory am instantiat clasa Intersection pentru fiecare tip de intersectie.
In ReaderHandlerFactory am asociat variabila intersection din main cu fiecare tip de intersectie si
am setat campurile de care am nevoie din intersection sau din main.

In IntersectionHandlerFactory am realizat implementarea propriu-zisa in fiecare functie handle.

#task1
Am afisat mesajul specific ajungerii la semafor pentru fiecare thread dupa care am dat sleep cu valoarea
timpului pentru fiecare masina si am afisat ca masina a iesit din interesctie.

#task2
Am afisat mesajele ajungerii in intersectie si am pus o bariera pentru ca rezolvarea sa inceapa dupa ce au ajuns
deja toate.

Am folosit un semafor pentru a ma asigura ca tot timpul sunt doar n masini in sensul giratoriu.
Acest semafor face acquire inainte de intrarea masinii, sta in giratoriu timpul dat in fisier, 
dupa care face release si afiseaza mesajul iesirii.

#task3

Acest task este asemanator cu cel de mai sus, doar ca pentru a ma asigura ca 
intra o singura masina din fiecare sens am folosit un semafor pentru fiecare sens din care vin masinile.

#task4
Acest task este asemanator cu cel de mai sus, doar ca pentru a ma asigura ca intra x masini din fiecare sens
am folosit un semafor pentru fiecare sens din care vin masini, semafor cu cate x locuri posibile. (x fiind citit din fisier)

Am mai folosit, de asemenea, o bariera cu valoarea numarul de masini care pot intra maxim in intersectie (nr_masini_per_sens * sensuri) pentru a ma asigura
ca aceste x * sensuri masini vor ajunge inainte ca acestea sa intre, respectiv ca vor iesi toate inainte de terminarea task-ului.


#task5
Pentru acest task am folosit notiunile de la cel anterior, dar am scos bariera mentionata, astfel neasteptand masinile sa intre/iasa toate, vor intra partial sau complet insa niciodata nu vor depasi acel numar.

#task6
Pentru acest task am salvat numarul de masini cu prioritate si fara, am initializat un semafor pentru masinile cu prioritate si
o coada unde voi retine ce masini fara prioritate incearca sa intre atunci cand intersectia este ocupata.

Am verificat daca masina care incearca sa intre e prioritara sau nu:
	-> daca este, o voi introduce in intersectie, fac acquire la semafor, dau sleep cele 2 secunde dupa care o scot si verific daca exista masini neprioritare care asteapta.
		-> daca exista si nu mai este nicio masina prioritara in intersectie, le voi introduce pe toate, in ordinea in care au venit
	-> daca nu este, voi afisa ca aceasta incearca sa intre. Verific daca sunt masini prioritare in intersectie.
		-> daca da, o adaug pe cea neprioritara in queue.
		-> daca nu, masina va intra in intersectie
#task7
Pentru acest task, am initializat pietonii cu datele din fisier si am salvat o lista de liste de string-uri pentru istoricul culorii semaforului.
Cat timp nu se termina executia programului (semaforului) (isFinished):
	->Daca pietonii trec in acest moment, masinile vor avea rosu (Voi afisa mesaj si salva in istoric doar daca masina a avut verde anterior)
	->Daca pietonii nu trec, Masinile vor avea verde (la fel ca mai sus)

Deoarece la sfarsitul while-ului, nu apuca toate masinile sa treaca, ma voi asigura ca trec toate verificand istoricul masinilor si asigurandu-ma ca toate au verde la final (acestea vor pleca).

#task8
Am afisat masinile care ajung in intersectie si am folosit o bariera sa ajunga toate odata. Am salvat masinile care au venit intr-o coada si dupa am parcurs coada verificand directia.
Cand au trecut nr_max de masini am schimbat sensul.

la #task9 nu am scris nimic iar la #task10 nu este complet (am sincronizat ajungerea la calea ferata cu o bariera, am afisat mesajul si am lasat masinile sa treaca in ordine).

