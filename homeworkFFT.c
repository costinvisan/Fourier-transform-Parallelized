#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <pthread.h>

typedef double complex cplx;

typedef struct{
	int step;
	cplx *buf;
	cplx *out;
} args;

int n;
int P;

cplx *arr_input;

void _fft(cplx buf[], cplx out[], int step);

void *thread_function(void *var);

void fft(cplx buf[], int n);
 
void show(const char * s, cplx buf[]);
 

int main(int argc, char *argv[]) {
	FILE *fd_input = fopen(argv[1], "r");
	FILE *fd_output = fopen(argv[2], "w");
	P = atoi(argv[3]);

	fscanf(fd_input, "%d", &n);
	
	arr_input = (cplx *) calloc(n, sizeof(cplx));
	for (int i = 0; i < n; ++i) {
		double aux;
		fscanf(fd_input, "%lf", &aux);
		arr_input[i] = aux;
	}	
 
 	fft(arr_input, n);

	fprintf(fd_output, "%d\n", n);
	for (int i = 0; i < n; ++i) {
		fprintf(fd_output, "%lf %lf\n", creal(arr_input[i]), cimag(arr_input[i]));
	}

	free(arr_input);
	fclose(fd_input);
	fclose(fd_output);

	return 0;
}

void _fft(cplx buf[], cplx out[], int step) {
	if (step < n) {

		if (step < P) {
			pthread_t tid1;
			pthread_t tid2;

			args arguments1, arguments2;
			arguments1.step = step * 2;
			arguments1.buf = buf;
			arguments1.out = out;

			arguments2.step = step * 2;
			arguments2.buf = buf + step;
			arguments2.out = out + step;

			pthread_create(&(tid1), NULL, thread_function, &(arguments1));
			pthread_create(&(tid2), NULL, thread_function, &(arguments2));

			pthread_join(tid1, NULL);
			pthread_join(tid2, NULL);

		} else {
			_fft(out, buf, step * 2);
			_fft(out + step, buf + step, step * 2);
	 	}
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * M_PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}

void *thread_function(void *var) {
	args arguments = *(args*) var;

	int step = arguments.step;
	cplx *buf = arguments.buf;
	cplx *out = arguments.out;

	_fft(out, buf, step);
}

void fft(cplx buf[], int n) {
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, 1);
}
 
 
void show(const char * s, cplx buf[]) {
	printf("%s", s);
	for (int i = 0; i < 8; i++)
		if (!cimag(buf[i]))
			printf("%g ", creal(buf[i]));
		else
			printf("(%g, %g) ", creal(buf[i]), cimag(buf[i]));
}
