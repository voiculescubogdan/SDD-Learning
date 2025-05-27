#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct Examen Examen;
typedef struct MaxHeap MaxHeap;

struct Examen {
	int nrCredite;
	char* materie;
	float nota;
};

struct MaxHeap {
	Examen* vector;
	int dim;
};

Examen initExamen(int nrCredite, char* materie, float nota) {
	Examen examen;

	examen.nrCredite = nrCredite;
	examen.nota = nota;
	examen.materie = (char*)malloc(sizeof(char*) * (strlen(materie) + 1));
	strcpy(examen.materie, materie);

	return examen;
}

// CITIRE DIN FISIER
Examen citireExamen(FILE* file) {
	Examen examen;
	char buffer[100];

	fgets(buffer, sizeof(buffer), file);
	examen.nrCredite = atoi(buffer);

	fgets(buffer, sizeof(buffer), file);
	char* materie = strtok(buffer, "\n");
	examen.materie = (char*)malloc(sizeof(char) * (strlen(materie) + 1));
	strcpy(examen.materie, materie);

	fgets(buffer, sizeof(buffer), file);
	examen.nota = atof(buffer);

	return examen;
}

void citireVectorExamene(FILE* file, Examen** vectorExamen, int* nrExamene) {
	if (file == NULL) {
		return;
	}

	while (!feof(file)) {
		*vectorExamen = (Examen*)realloc(*vectorExamen, sizeof(Examen) * ((*nrExamene) + 1));
		(*vectorExamen)[*nrExamene] = citireExamen(file);
		(*nrExamene)++;
	}
}

void afisareExamen(Examen examen) {
	printf("\nExamenul la materia %s, avand %d numar de credite, se trece cu nota %.2f", examen.materie, examen.nrCredite, examen.nota);
}

void traversareMaxHeap(MaxHeap heap) {
	for (int i = 0; i < heap.dim; i++) {
		afisareExamen(heap.vector[i]);
	}
}

// aici se face MaxHeap ul, adica filtreaza toate examenele in ordine descrescatoare
void filtrareMaxHeap(MaxHeap heap, int index) {
	if (heap.dim > 0) {
		int pozMax = index;
		int pozSt = 2 * index + 1;
		int pozDr = 2 * index + 2;

		if (pozSt < heap.dim && heap.vector[pozMax].nrCredite < heap.vector[pozSt].nrCredite) {
			pozMax = pozSt;
		}

		if (pozDr < heap.dim && heap.vector[pozMax].nrCredite < heap.vector[pozDr].nrCredite) {
			pozMax = pozDr;
		}

		if (index != pozMax) {
			Examen aux = heap.vector[index];
			heap.vector[index] = heap.vector[pozMax];
			heap.vector[pozMax] = aux;

			if (pozMax <= heap.dim / 2 - 1) {
				filtrareMaxHeap(heap, pozMax);
			}
		}
	}
}

// se scoate din MaxHeap radacina (adica cel mai mare examen)
// se inlocuieste cu ultimul examen pentru a putea fi scos mai usor
// se reduce dimensiunea cu 1
// se filtreaza noul heap
void extragereMaxHeap(MaxHeap* heap, Examen* examen) {
	*examen = initExamen(heap->vector[0].nrCredite, heap->vector[0].materie, heap->vector[0].nota);
	Examen aux = heap->vector[0];
	heap->vector[0] = heap->vector[heap->dim - 1];
	heap->vector[heap->dim - 1] = aux;
	heap->dim--;
	filtrareMaxHeap(*heap, 0);
}

// se face loc pentru un nou examen in heap
// noul examen este adaugat la sfarsit
// dupa ce este adaugat, se filtreaza noul heap
void inserareMaxHeap(MaxHeap* heap, Examen examen) {
	heap->vector = realloc(heap->vector, sizeof(Examen) * (heap->dim + 1));
	heap->vector[heap->dim] = examen;

	int index = heap->dim;
	heap->dim++;

	while (index > 0) {
		int parent = (index - 1) / 2;

		if (heap->vector[parent].nrCredite < heap->vector[index].nrCredite) {
			Examen aux = heap->vector[parent];
			heap->vector[parent] = heap->vector[index];
			heap->vector[index] = aux;

			index = parent;
		}
		else {
			break;
		}
	}
}

// stergem toate elementele care au nota < 5
// se face un nou heap temporar care va tine doar examenele cu nota mai mare decat 5
// dupa se modifica heap ul initial cu acesta temporar
// la final se filtreaza noul heap
void stergeDinMaxHeap(MaxHeap* mHeap, float notaCriteriu) {
	Examen* temp = (Examen*)malloc(sizeof(Examen) * mHeap->dim);
	int newDim = 0;

	for (int i = 0; i < mHeap->dim; i++) {
		if (mHeap->vector[i].nota < notaCriteriu) {
			free(mHeap->vector[i].materie);
		}
		else {
			temp[newDim++] = mHeap->vector[i];
		}
	}

	free(mHeap->vector);
	mHeap->vector = (Examen*)realloc(temp, sizeof(Examen) * newDim);
	mHeap->dim = newDim;

	for (int i = mHeap->dim / 2 - 1; i >= 0; i--) {
		filtrareMaxHeap(*mHeap, i);
	}
}

void updateInMaxHeap(MaxHeap* heap, char* materie) {
	for (int i = 0; i < heap->dim; i++) {

		if (strcmp(heap->vector[i].materie, materie) == 0) {
			heap->vector[i].nota = 6.6f;
		}
	}
}

void cautareInMaxHeap(MaxHeap heap, float criteriu) {
	for (int i = 0; i < heap.dim; i++) {
		if (heap.vector[i].nota < criteriu) {
			afisareExamen(heap.vector[i]);
		}
	}
}

void afisareFrunze(MaxHeap heap) {
	int primaFrunza = heap.dim / 2;

	for (int i = primaFrunza; i < heap.dim; i++) {
		afisareExamen(heap.vector[i]);
	}
}

void dezalocareMaxHeap(MaxHeap heap) {
	for (int i = 0; i < heap.dim; i++) {
		free(heap.vector[i].materie);
	}

	free(heap.vector);
}

void main() {

	// CITIRE DIN FISIER
	FILE* file = fopen("fisier.txt", "r");
	Examen* vectorExamene = NULL;
	int nrExamene = 0;
	citireVectorExamene(file, &vectorExamene, &nrExamene);
	
	MaxHeap heap;
	heap.dim = nrExamene;
	heap.vector = (Examen*)malloc(sizeof(Examen) * heap.dim);
	
	for (int i = 0; i < nrExamene; i++) {
		heap.vector[i] = vectorExamene[i];
	}
	
	fclose(file);

	MaxHeap heap;
	heap.dim = 6;

	heap.vector = (Examen*)malloc(sizeof(Examen) * heap.dim);

	heap.vector[0] = initExamen(1, "POO", 3.5);
	heap.vector[1] = initExamen(2, "ATP", 6.5);
	heap.vector[2] = initExamen(3, "SDD", 4);
	heap.vector[3] = initExamen(4, "Micro", 8.2);
	heap.vector[4] = initExamen(5, "SGBD", 3.7);
	heap.vector[5] = initExamen(6, "PAW", 2.3);

	printf("Traversare heap:");

	traversareMaxHeap(heap);

	printf("\n\nFiltrare heap:");

	for (int i = heap.dim / 2 - 1; i >= 0; i--) {
		filtrareMaxHeap(heap, i);
	}

	traversareMaxHeap(heap);

	printf("\n\nInserare in heap: ");

	inserareMaxHeap(&heap, initExamen(7, "CTS", 7.5));

	traversareMaxHeap(heap);

	printf("\n\nExtragere din heap:"); // extragen max

	Examen examen;
	extragereMaxHeap(&heap, &examen);
	afisareExamen(examen);

	printf("\n\nDupa stergerea examenelor cu nota < 5:");

	stergeDinMaxHeap(&heap, 5.0f);
	traversareMaxHeap(heap);

	printf("\n\nAfisare frunze: ");
	afisareFrunze(heap);

	printf("\n\nUpdate in MaxHeap: ");
	updateInMaxHeap(&heap, "SDD");
	traversareMaxHeap(heap);
	
	printf("\n\nCautare in MaxHeap: ");
	cautareInMaxHeap(heap, 7.0f);

	dezalocareMaxHeap(heap);
}
