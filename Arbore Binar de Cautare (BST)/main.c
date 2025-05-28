#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct Examen Examen;
typedef struct Nod Nod;

struct Examen {
	int id;
	char* materie;
	float nota;
};

struct Nod {
	Examen info;
	Nod* stanga;
	Nod* dreapta;
};

// pentru conversie la lista dubla
typedef struct LDI LDI;

struct LDI {
	Examen info;
	LDI* prev;
	LDI* next;
};

// pentru conversie la lista simpla
typedef struct NodSimplu NodSimplu;

struct NodSimplu {
	Examen info;
	NodSimplu* next;
}

Examen initExamen(int id, char* materie, float nota) {
	Examen examen;
	examen.id = id;
	examen.nota = nota;
	examen.materie = (char*)malloc(sizeof(char) * (strlen(materie) + 1));
	strcpy(examen.materie, materie);

	return examen;
}

// CITIRE DIN FISIER
Examen citireExamen(FILE* file) {
	Examen examen;
	char buffer[100];

	fgets(buffer, sizeof(buffer), file);
	examen.id = atoi(buffer);

	fgets(buffer, sizeof(buffer), file);
	char* materie = strtok(buffer, "\n");
	examen.materie = (char*)malloc(sizeof(char) * (strlen(materie) + 1));
	strcpy(examen.materie, materie);

	fgets(buffer, sizeof(buffer), file);
	examen.nota = atof(buffer);

	return examen;
}

void citireVectorExamene(FILE* file, Examen** vectorExamene, int* nrExamene) {
	if (!file) {
		return;
	}

	while (!feof(file)) {
		*vectorExamene = (Examen*)realloc(*vectorExamene, sizeof(Examen) * ((*nrExamene) + 1));
		(*vectorExamene)[*nrExamene] = citireExamen(file);
		(*nrExamene)++;
	}
}

void afisareExamen(Examen examen) {
	printf("\nExamenul la materia %s, cu id-ul %d, are nota de trecere %.2f", examen.materie, examen.id, examen.nota);
}

// daca nu exista radacina, o creeaza
// apoi in stanga cu id mai mic, in dreapta cu id mai mare
void inserareBST(Nod** radacina, Examen examen) {
	if (*radacina == NULL) {
		Nod* nou = (Nod*)malloc(sizeof(Nod));
		nou->info = examen;
		nou->stanga = NULL;
		nou->dreapta = NULL;
		*radacina = nou;
	}
	else if (examen.id < (*radacina)->info.id) {
		inserareBST(&(*radacina)->stanga, examen);
	}
	else {
		inserareBST(&(*radacina)->dreapta, examen);
	}
}

// pentru parcurgeri in ordine crescatoare sau descrescatoare (schimba stanga cu dreapta si invers)
void parcurgereInordineArbore(Nod* radacina) {
	if (radacina) {
		parcurgereInordineArbore(radacina->stanga);
		afisareExamen(radacina->info);
		parcurgereInordineArbore(radacina->dreapta);
	}
}

//viziteaza intai subarborii, apoi radacina
void parcurgerePostordineArbore(Nod* radacina) {
	if (radacina) {
		parcurgerePostordineArbore(radacina->stanga);
		parcurgerePostordineArbore(radacina->dreapta);
		afisareExamen(radacina->info);
	}
}

// daca id == radacina, inseamna ca este chiar radacina
// daca id < radacina, inseamna ca trebuie cautat in stanga
// daca id > radacina, inseamna ca trebuie cautat in dreapta
Examen cautareById(Nod* radacina, int id) {
	if (radacina == NULL) {
		return initExamen(-1, "", 0.0);
	}
	else if (id == radacina->info.id) {
		return initExamen(radacina->info.id, radacina->info.materie, radacina->info.nota);
	}
	else if (id > radacina->info.id) {
		return cautareById(radacina->dreapta, id);
	}
	else {
		return cautareById(radacina->stanga, id);
	}
}

void stergereDinArbore(Nod** radacina, int id) {
	if (!radacina || !*radacina) return;

	if (id < (*radacina)->info.id) {
		stergereDinArbore(&(*radacina)->stanga, id);
	}
	else if (id > (*radacina)->info.id) {
		stergereDinArbore(&(*radacina)->dreapta, id);
	}
	// a fost gasit nodul pe care vrem sa il stergem
	else {
		// daca este nod frunză
		if (!(*radacina)->stanga && !(*radacina)->dreapta) {
			free((*radacina)->info.materie);
			free(*radacina);
			*radacina = NULL;
		}
		// daca are un singur copil
		else if (!(*radacina)->stanga || !(*radacina)->dreapta) {
			Nod* copil = (*radacina)->stanga ? (*radacina)->stanga : (*radacina)->dreapta;
			free((*radacina)->info.materie);
			free(*radacina);
			*radacina = copil;
		}
		// daca are doi copii → găsesc succesorul
		else {
			// parcurg spre stânga cât e nevoie, pornind din fiul drept
			Nod* parinte = *radacina;
			Nod* fiu = (*radacina)->dreapta;

			while (fiu->stanga) {
				parinte = fiu;
				fiu = fiu->stanga;
			}

			// copiez datele fiului în nodul curent
			free((*radacina)->info.materie);
			(*radacina)->info.id = fiu->info.id;
			(*radacina)->info.nota = fiu->info.nota;
			(*radacina)->info.materie = malloc(strlen(fiu->info.materie) + 1);
			strcpy((*radacina)->info.materie, fiu->info.materie);

			// apoi elimin fiul din arbore
			// copilul lui "fiu" poate avea doar subarbore drept
			Nod* subarb = fiu->dreapta;
			// legături de părinte
			if (parinte->stanga == fiu)
				parinte->stanga = subarb;
			else
				parinte->dreapta = subarb;

			// eliberez memoria fiu
			free(fiu->info.materie);
			free(fiu);
		}
	}
}

void stergeNodRadacina(Nod** radacina) {

	if (radacina) {

		Nod* parent = *radacina;
		Nod* fiu = parent->dreapta;

		while (fiu->stanga) {
			parent = fiu;
			fiu = fiu->stanga;
		}

		free((*radacina)->info.materie);
		(*radacina)->info.id = fiu->info.id;
		(*radacina)->info.nota = fiu->info.nota;
		(*radacina)->info.materie = (char*)malloc(sizeof(char) * (strlen(fiu->info.materie) + 1));
		strcpy((*radacina)->info.materie, fiu->info.materie);

		Nod* subarb = fiu->dreapta;

		if (parent->stanga == fiu) {
			parent->stanga = subarb;
		}
		else {
			parent->dreapta = subarb;
		}

		free(fiu->info.materie);
		free(fiu);

		parcurgerePostordineArbore(*radacina);
	}
}

Examen extragereDinArbore(Nod** radacina, int id) {
	// 1) întâi găsesc examenul
	Examen examen = cautareById(*radacina, id);
	// 2) dacă există, îl şterg
	if (examen.id != -1) {
		stergereDinArbore(&(*radacina), id);
	}
	return examen;
}

void updateInBST(Nod** radacina, float criteriu) {

	if (*radacina) {
		updateInBST(&(*radacina)->stanga, criteriu);
		updateInBST(&(*radacina)->dreapta, criteriu);

		if ((*radacina)->info.nota < criteriu) {
			(*radacina)->info.nota = 6.66;
		}
	}
	
}

void mediaNotelorLaAceeasiMaterie(Nod* radacina, char* materie, float* sum, int* count) {

	if (!radacina) return;

	if (radacina) {
		mediaNotelorLaAceeasiMaterie(radacina->stanga, materie, sum, count);
		mediaNotelorLaAceeasiMaterie(radacina->dreapta, materie, sum, count);

		if (strcmp(radacina->info.materie, materie) == 0) {
			*count += 1;
			*sum += radacina->info.nota;
		}
	}
	
}

void functieExameneAdmise(Nod* radacina, Examen** vectorExamen, float notaCriteriu, int* nr) {
	
	if (!radacina) return;

	if (radacina) {

		functieExameneAdmise(radacina->stanga, vectorExamen, notaCriteriu, nr);
		functieExameneAdmise(radacina->dreapta, vectorExamen, notaCriteriu, nr);

		if (radacina->info.nota >= notaCriteriu) {
			*vectorExamen = (Examen*)realloc(*vectorExamen, sizeof(Examen) * ((*nr) + 1));
			(*vectorExamen)[*nr] = radacina->info;
			(*nr) += 1;
		}
	}
}

Examen* vectorExameneAdmise(Nod* radacina, float notaCriteriu, int* nr) {
	Examen* vector = NULL;
	*nr = 0;
	functieExameneAdmise(radacina, &vector, notaCriteriu, nr);
	return vector;
}

// extrage mai multe elemente dupa un criteriu intr-un vector
void extrageElementeById(Nod** radacina, Examen** vectorElemente, int* nrElemente, int idCriteriu) {
	if (!(*radacina)) return;

	if (*radacina) {
		extrageElementeById(&(*radacina)->stanga, vectorElemente, nrElemente, idCriteriu);
		extrageElementeById(&(*radacina)->dreapta, vectorElemente, nrElemente, idCriteriu);

		if ((*radacina)->info.id < idCriteriu) {
			*vectorElemente = (Examen*)realloc(*vectorElemente, sizeof(Examen) * ((*nrElemente) + 1));
			(*vectorElemente)[*nrElemente] = extrageDinBST(radacina, (*radacina)->info.id);
			(*nrElemente)++;
		}
	}
}

Examen* functieExtragereElemente(Nod** radacina, int* nrElemente, int idCriteriu) {
	Examen* vector = NULL;
	*nrElemente = 0;
	extrageElementeById(radacina, &vector, nrElemente, idCriteriu);
	return vector;
}

// conversie la vector
void conversieLaVector(Nod* radacina, Examen** vectorConversie, int* nrElem) {
	if (!radacina) return;

	if (radacina) {
		conversieLaVector(radacina->stanga, vectorConversie, nrElem);
		conversieLaVector(radacina->dreapta, vectorConversie, nrElem);

		*vectorConversie = (Examen*)realloc(*vectorConversie, sizeof(Examen) * ((*nrElem) + 1));
		(*vectorConversie)[*nrElem] = radacina->info;
		(*nrElem)++;
	}
}

Examen* vectorConversie(Nod* radacina, int* nrElem) {
	Examen* vector = NULL;
	*nrElem = 0;
	conversieLaVector(radacina, &vector, nrElem);
	return vector;
}

// conversie la lista dubla
void conversieListaDubla(Nod* radacina, LDI** cap, LDI** coada) {
	if (!radacina) return;

	if (radacina) {
		conversieListaDubla(radacina->stanga, cap, coada);
		conversieListaDubla(radacina->dreapta, cap, coada);

		LDI* nou = (LDI*)malloc(sizeof(LDI));
		nou->info = radacina->info;
		nou->prev = *coada;
		nou->next = NULL;

		if (*cap == NULL) {
			*cap = *coada = nou;
		}
		else {
			(*coada)->next = nou;
			*coada = nou;
		}
	}
}

LDI* listaDublaConversie(Nod* radacina) {
	LDI* cap = NULL;
	LDI* coada = NULL;
	conversieListaDubla(radacina, &cap, &coada);
	return cap;
}

void afisareListaDubla(LDI* cap) {
	while (cap) {
		afisareExamen(cap->info);
		cap = cap->next;
	}
}

// conversie la lista simpla
void conversieListaSimpla(Nod* radacina, NodSimplu** cap) {
	if (!radacina) return;

	if (radacina) {
		conversieListaSimpla(radacina->stanga, cap);
		conversieListaSimpla(radacina->dreapta, cap);

		NodSimplu* nou = (NodSimplu*)malloc(sizeof(NodSimplu));
		nou->info = radacina->info;
		nou->next = *cap;
		*cap = nou;
	}
}

NodSimplu* listaSimplaConversie(Nod* radacina) {
	NodSimplu* cap = NULL;
	conversieListaSimpla(radacina, &cap);
	return cap;
}

void afisareListaSimpla(NodSimplu* cap) {
	while (cap) {
		afisareExamen(cap->info);
		cap = cap->next;
	}
}

// verifica daca are copil in stanga si in dreapta
// daca nu are, inseamna ca este frunza
// daca are, se reia procesul
void afisareFrunze(Nod* radacina) {
	if (!radacina) return;

	if (!radacina->stanga && !radacina->dreapta) {
		afisareExamen(radacina->info);
	}
	else {
		afisareFrunze(radacina->stanga);
		afisareFrunze(radacina->dreapta);
	}
}

void dezalocare(Nod** radacina) {
	if (*radacina) {
		dezalocare(&(*radacina)->stanga);
		dezalocare(&(*radacina)->dreapta);
		free((*radacina)->info.materie);
		free(*radacina);
	}
}

void main() {
	// Nod* radacina = NULL;

	// inserareBST(&radacina, initExamen(7, "SDD", 5));
	// inserareBST(&radacina, initExamen(4, "PAW", 5));
	// inserareBST(&radacina, initExamen(9, "PEAG", 5));
	// inserareBST(&radacina, initExamen(3, "JAVA", 5));
	// inserareBST(&radacina, initExamen(5, "MACRO", 5));
	// inserareBST(&radacina, initExamen(8, "POO", 5));
	// inserareBST(&radacina, initExamen(2, "ATP", 5));
	// inserareBST(&radacina, initExamen(1, "SGBD", 5));

	FILE* file = fopen("arbore.txt", "r");
	Examen* vectorExamene = NULL;
	int nrExamene = 0;
	citireVectorExamene(file, &vectorExamene, &nrExamene);
	
	Nod* radacina = NULL;
	
	for (int i = 0; i < nrExamene; i++) {
		inserareBST(&radacina, vectorExamene[i]);
	}

	printf("\n\nParcurgere inordine arbore: ");
	parcurgereInordineArbore(radacina);

	printf("\n\nParcurgere postordine arbore: ");
	parcurgerePostordineArbore(radacina);

	printf("\n\nCautare nod cu id 8");

	Examen examen = cautareById(radacina, 8);
	afisareExamen(examen);

	printf("\n\nAfisare frunze: ");
	afisareFrunze(radacina);

	printf("\n\nStergere nod: ");
	stergereDinArbore(&radacina, 7);
	parcurgerePostordineArbore(radacina);

	printf("\n\nExtragere nod: ");
	Examen examenExtras;
	examenExtras = extragereDinArbore(&radacina, 4);
	afisareExamen(examenExtras);

	printf("\n\nDupa update pentru notele mai mici decat 5");
	updateInBST(&radacina, 5.0f);
	parcurgerePostordineArbore(radacina);

	printf("\n\nMedia notelor cu aceeasi materie: ");
	float sum = 0.0;
	int count = 0;
	mediaNotelorLaAceeasiMaterie(radacina, "SDD", &sum, &count);
	printf("\n\nMedia notelor: %.2f", sum/count);

	printf("\n\nSterge nod radacina: ");
	stergeNodRadacina(&radacina);

	printf("\n\nVector examene cu nota peste 5: ");
	int nr = 0;
	Examen* vectorExamen = vectorExameneAdmise(radacina, 5.0f, &nr);
	for (int i = 0; i < nr; i++) {
		afisareExamen(vectorExamen[i]);
	}

	printf("\n\nExtragere elemente cu id < 4 intr-un vector: ");
	Examen* vectorElemente = NULL;
	int nrElemente = 0;
	vectorElemente = functieExtragereElemente(&radacina, &nrElemente, 4);
	for (int i = 0; i < nrElemente; i++) {
		afisareExamen(vectorElemente[i]);
	}

	printf("\n\nConversie la vector: ");
	Examen* vector = NULL;
	int nrElem = 0;
	vector = vectorConversie(radacina, &nrElem);
	for (int i = 0; i < nrElem; i++) {
		afisareExamen(vector[i]);
	}

	printf("\n\nConversie la lista dubla: ");
	LDI* cap = NULL;
	cap = listaDublaConversie(radacina);
	afisareListaDubla(cap);

	printf("\n\nConversie la lista simpla: ");
	NodSimplu* cap = NULL;
	cap = listaSimplaConversie(radacina);
	afisareListaSimpla(cap);

	dezalocare(&radacina);
}
