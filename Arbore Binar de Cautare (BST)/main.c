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

	dezalocare(&radacina);
}
