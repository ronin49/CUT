#include <stdio.h>
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
int main(){
	struct info inf = read();
	printf("%ld %ld %ld %ld %ld %ld %ld\n",inf.values[0],inf.values[1],inf.values[2],inf.values[3],inf.values[4],inf.values[5],inf.values[6]);
	printf("idle: %ld", analyze(inf));
}
