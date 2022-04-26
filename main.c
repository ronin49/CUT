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
int analyze(struct info inf) {
	int user = inf.values[0];
	int nice = inf.values[1];
	int system = inf.values[2];
	int idle = inf.values[3];
	int iowait = inf.values[4];
	int irq = inf.values[5];
	int softirq = inf.values[6];
	return (idle*100) / (user + nice + system + idle + iowait + irq + softirq);
}
void print(int idle) {
	printf("idle: %d\n", idle);
}
/*there are 3 threads
  reading thread
  analyzing thread
  printing thread

  reading thread reads infos to buffer which uses analyzing thread

  analyzing thread takes from buffer which reading thread is writing to
  and puts percentages to buffer which uses printing thread

  printing thread prints from buffer to which analyzing thread writes to
 */
/*reader -> analyzer communication using array of info*/
struct info infoVector[100]; int infoVectorSize = 0;
pthread_mutex_t infos = PTHREAD_MUTEX_INITIALIZER;
/*analyzer -> printer communication using array of percentages*/
int percentVector[100]; int percentVectorSize = 0;
pthread_mutex_t percents = PTHREAD_MUTEX_INITIALIZER;
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
		if(infoVectorSize>0 && percentVectorSize < 100) {
			pthread_mutex_lock(&infos);
			pthread_mutex_lock(&percents);
			percentVector[percentVectorSize++] = analyze(infoVector[infoVectorSize--]);
			pthread_mutex_unlock(&percents);
			pthread_mutex_unlock(&infos);
		}
	}
}
void *printerThread(){
	for(;;){
		if(percentVectorSize>0) {
			pthread_mutex_lock(&percents);
			print(percentVector[percentVectorSize--]);
			pthread_mutex_unlock(&percents);
		}
	}
}
int main(){
	pthread_t reader; pthread_create(&reader,NULL,readerThread,NULL);
	pthread_t analyzer; pthread_create(&analyzer,NULL,analyzerThread,NULL);
	pthread_t printer; pthread_create(&printer,NULL,printerThread,NULL);
	pthread_join(printer,NULL);
	pthread_join(reader,NULL);
	pthread_join(analyzer,NULL);
}
