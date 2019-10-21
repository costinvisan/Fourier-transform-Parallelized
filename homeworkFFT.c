#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <pthread.h>

struct args {
	int step;
	cplx buf[];
	cplx out[];
};

int n;
int P;

typedef double complex cplx;

cplx *arr_input;

void _fft(cplx buf[], cplx out[], int step) {
	if (step < n) {

		if (step < P) {
			pthread_t tid[P];
			int thread_id[P];

			args arguments;
			arguments.step = step * 2;
			arguments.buf = buf;
			arguments.out = out;

			for(int i = 0; i < 2; i++)
				thread_id[i] = i;

			for(int i = 0; i < P; i++) {
				pthread_create(&(tid[i]), NULL, thread_function, &(arguments));
			}

			for(int i = 0; i < P; i++) {
				pthread_join(tid[i], NULL);
			}
		}

		_fft(out, buf, step * 2);
		_fft(out + step, buf + step, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * M_PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}

void *thread_function(void *var) {
	args arguments = *(args *) var;

	int step = arguments.step;
	cplx buf[] = arguments.buf;
	cplx out[] = arguments.out;

	_fft(out, buf, n, step * 2);
	_fft(out + step, buf + step, n, step * 2);
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
 
int main(int argc, char *argv[]) {
	FILE *fd_input = fopen(argv[1], "r");
	FILE *fd_output = fopen(argv[2], "w");
	P = atoi(argv[3]);

	fscanf(fd_input, "%d", &n);
	
	arr_input = (cplx *) calloc(n, sizeof(cplx));
	for (int i = 0; i < n; ++i) {
		fscanf(fd_input, "%lf", &arr_input[i]);
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