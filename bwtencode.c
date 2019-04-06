#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
//#define DEBUGtime 0
#define DEBUG 0
int *array;

int cmp(const void *a, const void *b){
    int ia = *(int *)a;
    int ib = *(int *)b;
    return array[ia] < array[ib]? -1: array[ia] > array[ib];
}

char d;
struct SuffixArray
{
        unsigned ip;            /* index point */
        char *suffix;
};

static int compare(const void *l, const void *r)
{
    struct SuffixArray *lp = (struct SuffixArray *)l;
    struct SuffixArray *rp = (struct SuffixArray *)r;
	
	char c1,c2;
	int i = 0;
	do{
		c1=(*lp).suffix[i];
		c2=(*rp).suffix[i];
		if(!c1 || !c2) return c1-c2;
		if(c1 == d && c2 == d) return lp->ip-rp->ip;			
		i++;
	}while(c1==c2);
	if(c1==d && c2 !=d) return -1;
	if(c2==d && c1 != d) return 1;	
	return c1-c2;
}


int encode(char* delimiter, char* path1, char* path2,char* path3){
    FILE *f1 = fopen(path2,"r");
    fseek(f1,0,SEEK_END);
    unsigned int length = ftell(f1);
    fseek(f1,0,SEEK_SET);
    char *string = malloc((length+1)*sizeof(char));//string has the whole string from file
    printf("length: %d\n",length);
    int i=0,j;
	fread(string,length,sizeof(char),f1);
	string[length]='\0';
	//printf("%s\n",string);
	fclose(f1);

	char de;
	if(strlen(delimiter)==2 && delimiter[1]=='n') de = '\n';
	else if(strlen(delimiter)==2 && delimiter[1]=='t') de = '\t';
	else de = delimiter[0];

    size_t blocksize;
	blocksize = sizeof(struct SuffixArray);
	//printf("blocksize:%d\n",blocksize);
    struct SuffixArray *iindex;

	char* p1 = malloc(sizeof(char)*(strlen(path1)+8));
	strcpy(p1,path1);
	strcat(p1,"/d1.txt\0");
	//printf("%s\n%s\n",p1,path3);
	FILE* d1 = fopen(p1,"wb");//dummyindex for aux
	FILE* f2 = fopen(path3,"wb");//bwt
	
	
	int len,x;
	char bwtchar;
	int p;
	clock_t beginqsort;
	clock_t endqsort;
	double time_spent_qsort;
	//search in a loop to find suffix starting with character lexicologically in order
	
	for(i=0;i<127;i++){	
		len=0;
		iindex = (struct SuffixArray*)calloc(1, blocksize);
		//printf("%d",i);
		if(!i){
			for (j = 0; j < length; j++) {
				if(string[j]==de){
					//printf("[%02d] %s\n", j, &string[j]);
					iindex[len].ip = j;
					iindex[len++].suffix = &string[j];
					iindex = (struct SuffixArray*)realloc(iindex,(len+1)* blocksize);//one more than len space allocation 
				}
			}	
		}
		else{
			if((char)i==de)continue;		
			else{
				for (j = 0; j < length; j++) {
					if(string[j]==(char)i){
						//printf("[%02d] %s\n", j, &string[j]);
						iindex[len].ip = j;
						iindex[len++].suffix = &string[j];
						iindex = (struct SuffixArray*)realloc(iindex,(len+1)* blocksize);//one more than len space allocation 
					}
				}
			}
		}	
		#ifdef DEBUGtime
		if(!len){
			printf("skip[%d], no time report\n",i);
			continue;
		}
		#endif
		beginqsort = clock();
		d = de;
		qsort(iindex, len, blocksize, compare);
		endqsort = clock();
		time_spent_qsort = (double)(endqsort - beginqsort) / CLOCKS_PER_SEC;
		#ifdef DEBUGtime 
		printf("time[%d]: %f\n",i,time_spent_qsort);
		#endif
		for (x = 0; x < len; x++){
			//printf("[%02d] %s\n", iindex[x].ip, iindex[x].suffix);
			//fwrite(iindex[x].ip,sizeof(int),1,d1);
			if(iindex[x].ip-1==-1)bwtchar=string[length-1];
			else bwtchar = string[iindex[x].ip-1];
			//printf("\n-------------bwt------------\n");
			//printf("%c ",bwtchar);
			//printf("\n-------------aux------------\n");
			if(bwtchar == de){ 
				if(iindex[x].ip-1==-1)p=length-1;
				else p=iindex[x].ip-1;
				//printf("%d ",p);
				printf("bwt pos: %d\n",ftell(f2));
				
				fwrite(&p,sizeof(int),1,d1);
			}
			fwrite(&bwtchar,sizeof(char),1,f2);
		}
		free(iindex);
	}
	fclose(d1);
	
    fclose(f2);
	printf("\n");
	FILE*f = fopen(p1,"rb");
	fseek(f,0,SEEK_END);
	int d1len = ftell(f)/sizeof(int);
	//printf("d1len: %d\n",d1len);
	fseek(f,0,SEEK_SET);
    int *d1string = malloc(d1len*sizeof(int));
	fread(d1string,d1len,sizeof(int),f);
	for(i=0;i<d1len;i++){
		printf("%d ",d1string[i]);
	}
	
	
	int *index = malloc(d1len*sizeof(int));
	for(i=0;i<d1len;i++){
		index[i] = i;
    }
#ifdef DEBUG
	printf("\n----------index(aux)-----------\n");
	for(i=0;i<d1len;i++){
		printf("%d ",index[i]);
	}
#endif
	array = d1string;
	qsort(index, d1len, sizeof(index[0]), cmp);
#ifdef DEBUG
	printf("\n----------index after sort-----------\n");
	for(i=0;i<d1len;i++){
		printf("%d ",index[i]);
	}
#endif	
	//int *index1 = calloc(d1len,sizeof(int));
	for(i=0;i<d1len;i++){
		//printf("%d, %d\n",index[i],i);
		d1string[index[i]]=i;		
	}	
	fclose(f);
#ifdef DEBUG		
	printf("\n----------inverted index-----------\n");
	for(i=0;i<d1len;i++){
		printf("%d ",d1string[i]);
	}
	printf("\n");
#endif
	int pathlen = strlen(path3);
	char* p3 = malloc(sizeof(char)*strlen(path3));
	strcpy(p3,path3);
	p3[pathlen-1]='x';
	p3[pathlen-2]='u';
	p3[pathlen-3]='a';
	FILE * f3 = fopen(p3,"wb");//auxiliary positional file
	fwrite(d1string,sizeof(int),d1len,f3);
	free(index);
	free(d1string);	
	//free(index1);
	fclose(f3);
	remove(p1);
    return 0;
}

int main(int argc, char* argv[])
{	
	clock_t begin = clock();
    encode(argv[1],argv[2],argv[3],argv[4]);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	#ifdef DEBUGtime
	printf("time: %f\n",time_spent);
	#endif
    return 0;
}
