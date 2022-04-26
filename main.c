#include <stdio.h>
#include <pthread.h>
struct info {
	size_t values[7];
};
struct info read(){
	FILE * f = fopen("/proc/stat","r");
	char b[255];
	struct info inf;
	fscanf(f,"%s %ld %ld %ld %ld %ld %ld %ld",b,inf.values,inf.values+1,inf.values+2,inf.values+3,inf.values+4,inf.values+5,inf.values+6);
	fclose(f);
	return inf;
}
void print(int idle) {
	printf("idle: %ld\n", idle);
}
int analyze(struct info inf) {
	int user = inf.values[0];
	int nice = inf.values[1];
	int system = inf.values[2];
	int idle = inf.values[3];
	int iowait = inf.values[4];
	int irq = inf.values[5];
	int softirq = inf.values[6];
	print((idle*100) / (user + nice + system + idle + iowait + irq + softirq));
}
struct info infoVector[100]; int infoVectorSize = 0;
pthread_mutex_t infos = PTHREAD_MUTEX_INITIALIZER;
void *readerThread(){
	for(;;){
		if(infoVectorSize<100) {
			pthread_mutex_lock(&infos);
			infoVector[infoVectorSize++] = read();
			pthread_mutex_unlock(&infos);
		}
	}
}
void *analyzerThread(){
	for(;;){
		if(infoVectorSize>0) {
			pthread_mutex_lock(&infos);
			analyze(infoVector[infoVectorSize--]);
			pthread_mutex_unlock(&infos);
		}
	}
}
int main(){
pthread_t reader; pthread_create(&reader,NULL,readerThread,NULL);
pthread_t analyzer; pthread_create(&analyzer,NULL,analyzerThread,NULL);
pthread_join(reader,NULL);
pthread_join(analyzer,NULL);
}
