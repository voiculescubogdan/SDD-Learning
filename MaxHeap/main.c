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

void afisareExamen(Examen examen) {
	printf("\nExamenul la materia %s, avand %d numar de credite, se trece cu nota %.2f", examen.materie, examen.nrCredite, examen.nota);
}

void traversareMaxHeap(MaxHeap heap) {
	for (int i = 0; i < heap.dim; i++) {
		afisareExamen(heap.vector[i]);
	}
}

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

void extragereMaxHeap(MaxHeap* heap, Examen* examen) {
	*examen = initExamen(heap->vector[0].nrCredite, heap->vector[0].materie, heap->vector[0].nota);
	Examen aux = heap->vector[0];
	heap->vector[0] = heap->vector[heap->dim - 1];
	heap->vector[heap->dim - 1] = aux;
	heap->dim--;
	filtrareMaxHeap(*heap, 0);
}

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

void afisareFrunze(MaxHeap heap) {
	int primaFrunza = heap.dim / 2;

	for (int i = primaFrunza; i < heap.dim; i++) {
		afisareExamen(heap.vector[i]);
	}
}

// stergem toate elementele care au nota < 5
void stergeDinMaxHeap(MaxHeap* mHeap) {
	Examen* temp = (Examen*)malloc(sizeof(Examen) * mHeap->dim);
	int newDim = 0;

	for (int i = 0; i < mHeap->dim; i++) {
		if (mHeap->vector[i].nota < 5.0f) {
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


void dezalocareMaxHeap(MaxHeap heap) {
	for (int i = 0; i < heap.dim; i++) {
		free(heap.vector[i].materie);
	}

	free(heap.vector);
}

void main() {

	// DE FACUT SI PENTRU CITIRE DIN FISIERE TEXT

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

	printf("\n\nAfisare frunze: ");
	afisareFrunze(heap);

	printf("\n\nDupa stergerea examenelor cu nota < 5:");

	stergeDinMaxHeap(&heap);
	traversareMaxHeap(heap);

	dezalocareMaxHeap(heap);
}