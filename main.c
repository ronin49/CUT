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
int main(){
	struct info inf = read();
	printf("%ld %ld %ld %ld %ld %ld %ld\n",inf.values[0],inf.values[1],inf.values[2],inf.values[3],inf.values[4],inf.values[5],inf.values[6]);
}
