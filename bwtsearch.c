#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define DEBUGctable 0
#define DEBUGaux 0
#define DEBUGlf 0
#define DEBUGocc 0
#define DEBUGocctable 0

int *ctable;
int *count;
int cmp (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

int occ(char c, int start, int end, FILE* f){
	fseek(f,start,SEEK_SET);
	int i,count =0;
	char fc;
	for(i=0;i<=end-start;i++){
		fread(&fc,sizeof(char),1,f);
		if(fc==c)count++;//strncmp(&l[i],&c,1)
	}
	return count;
}

int occfunc(char c,int pos, FILE* d,FILE* f){
	fseek(f,0,SEEK_END);
	int flen = ftell(f)/sizeof(char);
	int num = flen/(127*4);
	int interval = flen/(num+1);
	if(!d || pos<interval){
		return occ(c,0,pos,f);
	}
	//printf("flen:%d",flen);
	int quo = pos / interval;
	if(quo>num){
		if(pos == flen)return count[(int)c];
		else return count[(int)c]-occ(c,pos+1,flen,f);
	}
	int result=0;
	int re = pos % interval;
	fseek(d,sizeof(int)*((quo-1)*127+(int)c),SEEK_SET);
	fread(&result,sizeof(int),1,d);
	if(!re){		
		return result;
	}
	else{
		result += occ(c,quo*interval+1,pos,f);
		return result;
	}
}

int trueid(int id, int decount){
	if(id == decount-1) return 1;
	else return id + 2;  
}

int exist(int j, int* id, int countid){
	//int size = sizeof(id)/sizeof(int);
	//printf("size: %d\n",size);
	int i;
	for(i=1;i<countid;i++){
		if(id[i]==j){
			printf("%d(%d) is exist in id. index[%d]\n",id[i],j,i);
			return 1;
		}
	}
	return 0;
}

int mnasearch(char* delimiter, char* path1, char* path2, char* mna, char* term){
	char de;
	if(strlen(delimiter)==2 && delimiter[1]=='n') de = '\n';
	else if(strlen(delimiter)==2 && delimiter[1]=='t') de = '\t';
	else de = delimiter[0];
	
	FILE* f1=fopen(path1,"r");
	fseek(f1,0,SEEK_END);
	unsigned int length = ftell(f1)/sizeof(char);
	//printf("length: %d\n",length);
	fseek(f1,0,SEEK_SET);
	int i,j;
	int deindex=(int)de;
	char c;
	count = calloc(127,sizeof(int));
	for(i = 0;i<length;i++){
		fread(&c,sizeof(char),1,f1);
		count[(int)c]++;
	}

	ctable = calloc(127,sizeof(int));
	int decount = count[deindex];
	count[0]=decount;
	count[deindex]=0;
#ifdef DEBUGctable
	printf("\n----------count-----------\n");
	for(i = 0;i<127;i++){
		if(count[i])printf("%d,%d\n",i,count[i]);
	}
#endif
	for(i = 0;i<127;i++){
		if(count[i]){for(j=0;j<i;j++)ctable[i]+=count[j];}
	}
	ctable[deindex]=0; 
	
#ifdef DEBUGctable
	printf("\n----------ctable-----------\n");
	for(i = 0;i<127;i++){
		if(ctable[i])printf("%c: %d\n",i,ctable[i]);
	}
#endif
	char* p2 = malloc(sizeof(char)*(strlen(path2)+8));
	sprintf(p2,"%s/%s",path2,"occ.txt");
	FILE* d1 = fopen(p2,"rb");
	int cc;	
	int num = length/(127*4);
	int interval = length/(num+1);
	//printf("\n------------------num:%d\n------------------interval:%d\n",num,interval);
	if(num && !d1){
		int pos;
		FILE* d = fopen(p2,"wb");
		for(i=0;i<num;i++){
			pos = interval*(i+1);
			for(j=0;j<127;j++){
				cc = occ((char)j,0,pos,f1);
				fwrite(&cc,sizeof(int),1,d);
			}
		}
		fclose(d);
		FILE* d1 = fopen(p2,"rb");
	}
	int* occtable = calloc(sizeof(int),num*127);
	fread(occtable,sizeof(int),num*127,d1);
#ifdef DEBUGocctable
	printf("\n----------occ table(from file)-----------\n");
	printf("      ");
	printf("%c|",de);
	for(j=0;j<127;j++){
		if(ctable[j])printf("%c|",j);
	}
	printf("\n");
	for(i=0;i<num;i++){
		for(j=0;j<127;j++){
			if(!j)printf("[%03d] ",interval*(i+1));
			if(j)cc = occtable[i*127+j];
			else cc = occtable[i*127+(int)de];
			if(!j||ctable[j])printf("%d|",cc);
		}
		printf("\n");
	}
#endif
#ifdef DEBUGocc
	printf("\n----------occ-----------\n");
	printf("      ");
	printf("%c|",de);
	for(j=0;j<127;j++){
		if(ctable[j])printf("%c|",j);
	}
	printf("\n");
	for(i=0;i<length;i++){
		if(i == 358 || i == 716){
			for(j=0;j<127;j++){
				if(!j)printf("[%03d] ",i);
				if(j)cc = occ((char)j,0,i,f1);
				else cc = occ(de,0,i,f1);
				if(!j||ctable[j])printf("%d|",cc);
			}
		printf("\n");
		}
	}
#endif
	printf("\n");
	//aux file read once a time
	int pathlen = strlen(path1);
	//printf("\npathlen: %d",pathlen);
	path1[pathlen-1]='x';
	path1[pathlen-2]='u';
	path1[pathlen-3]='a';
	//printf("\n%s\n",path1);
	FILE* f2 =fopen(path1,"rb");
	if(mna[1]=='i'){
		int start = atoi(strtok(term," "));
		int end = atoi(strtok(NULL," "));
		//printf("\nfrom %d to %d\n",start,end);
		//delimiter position buffer to array dp
		int *dp = malloc(decount*sizeof(int));
		j=0;
		fseek(f1,0,SEEK_SET);
		for(i = 0;i<length;i++){
			fread(&c,sizeof(char),1,f1);
			if(c==de){//delimiter[0]			
				dp[j++]=i;
			}
		}
#ifdef DEBUGaux
		printf("\n--------------dp----------------\n");
		for(i=0;i<decount;i++){
			printf("%d ",dp[i]);
		}		

		printf("\n--------------aux----------------\n");
		int* aux = malloc(sizeof(int)*decount);
		fseek(f2,0,SEEK_SET);
		fread(aux,sizeof(int),decount,f2);
		for(i=0;i<decount;i++){
			printf("%d ",aux[i]);
		}	
#endif
		printf("\n");
		int x,y;
		int nextindex;
		char next;
		char *records;
		int slen=0;
		printf("\ndecount: %d\n",decount);
		for(i=start;i<end+1;i++){
			slen = 0;
			records = (char*)malloc(sizeof(char));
			nextindex = dp[i-1];
			fseek(f1,nextindex,SEEK_SET);
			fread(&next,sizeof(char),1,f1);
			printf("\ni: %d,currentindex: %d, current: %c\n",i,nextindex,next);
			do{
				printf("next: %c, nextindex: %d, occfunc: %d, occ: %d\n",next,nextindex,occfunc(next,nextindex,d1,f1),occ(next,0,nextindex,f1));
				nextindex = ctable[(int)next]+occ(next,0,nextindex,f1)-1;
				fseek(f1,nextindex,SEEK_SET);
				fread(&next,sizeof(char),1,f1);
				records[slen++]=next;
				records = realloc(records,(slen+1)*sizeof(char));
			}while(next!=de);
			for(y=slen-2;y>=0;y--){
				printf("%c",records[y]);
			}
			printf("\n");
			free(records);
		}/*
		for(i=start;i<end+1;i++){
			slen = 0;
			records = (char*)malloc(sizeof(char));
			nextindex = dp[i-1];
			fseek(f1,nextindex,SEEK_SET);
			fread(&next,sizeof(char),1,f1);
			printf("\ni: %d, j: %d, currentindex: %d, current: %c\n",i,j,nextindex,next);
			while(1){
				printf("next: %c, nextindex: %d, occfunc: %d, occ: %d\n",next,nextindex,occfunc(next,nextindex,d1,f1),occ(next,0,nextindex,f1));
				nextindex = ctable[(int)next]+occ(next,0,nextindex,f1)-1;//occfunc(next,nextindex,d1,f1)-1;
				fseek(f1,nextindex,SEEK_SET);
				fread(&next,sizeof(char),1,f1);//next = string[nextindex];				
				printf("nextindex: %d, next: %d\n",nextindex,next);
				if(next==de) break;
				records[slen]=next;
				records=realloc(records,(++slen+1)*sizeof(char));
			}
			for(y=slen-1;y>=0;y--){
				printf("%c",records[y]);
			}
			printf("\n");
			free(records);
		}*/
	}
	else{
		//printf("%s\n",term);
		j=strlen(term)-1;
		//printf("j: %d, term[j]: %c\n",j,term[j]);
		char tc = term[j];
		int first,last;
		if(tc == de) first = 0;
		else first = ctable[(int)tc];
		//int n;
		if(tc==de) last = count[0]-1;
		else last = ctable[(int)tc]+count[(int)tc]-1;
		if(!first&&!last){//record(one char) not found 
			printf("0\n");
			return 0;
		} 
		printf("\ntc: %c, first: %d, last: %d\n",tc,first,last);
		int flag = 1;
		while(last>=first && j>=1){
			tc = term[--j];		
#ifdef DEBUGlf
			printf("c: %c first: %d, occ table: %d, occ: %d\nlast: %d,occ table: %d, occ: %d\n",tc,first,occfunc(tc,first-1,d1,f1),occ(tc,0,first-1,f1),last,occfunc(tc,last,d1,f1),occ(tc,0,last,f1));
			//printf("c: %c, first: %d, last: %d\n",tc,first,last);
#endif
			first = ctable[(int)tc]+occ(tc,0,first-1,f1);//occfunc(tc,first-1,d1,f1);
			last = ctable[(int)tc]+occ(tc,0,last,f1)-1;//occfunc(tc,last,d1,f1)-1;
			if(last<first){
				flag=0;
				break;
			}
		}
		printf("flag: %d\n",flag);
		if(!flag){//records(moren than one char not found
			printf("0\n");
			return 0;
		} 
		if(mna[1]=='m'){
			printf("\n---------------new m result----------------\n");
			printf("find %d record(s)\n",last-first+1);return 0;
		} 
		//delimiter bwt position buffer to array dp
		int *dp = malloc(decount*sizeof(int));
		j=0;
		fseek(f1,0,SEEK_SET);
		for(i = 0;i<length;i++){
			fread(&c,sizeof(char),1,f1);
			if(c==de){//delimiter[0]			
				dp[j++]=i;
			}
		}
		printf("\n--------------bwt dp----------------\n");
		for(i=0;i<decount;i++){
			printf("%d ",dp[i]);
		}
		printf("\n");
		int nextindex;
		char next;
		int* in = (int*)calloc(sizeof(int),1);
		//in[0]=-1;
		//int incount=1;
		int incount=0;
		int id;
		for(i=first;i<=last;i++){
			nextindex = i;
			fseek(f1,nextindex,SEEK_SET);
			fread(&next,sizeof(char),1,f1);
			printf("currentindex: %d, current: %c\n",nextindex,next);
			while(next!=de){
				nextindex=ctable[(int)next]+occfunc(next,nextindex,d1,f1)-1;
				fseek(f1,nextindex,SEEK_SET);
				fread(&next,sizeof(char),1,f1);
				printf("nextindex: %d, next(ascii): %d\n",nextindex,next);
			}
			for(j = 0; j< decount; j++){
				if(dp[j]==nextindex){
					fseek(f2,j*sizeof(int),SEEK_SET);
					fread(&id,sizeof(int),1,f2);
					id = trueid(id,decount);
					if(!exist(id,in,decount)){
						in[incount++]=id;
						printf("%d being stored in id array.\n ",id);
						in = (int*)realloc(in,sizeof(int)*(incount+1));
					}
				}
			}
			/*while(1){
				if(next==de){//delimiter[0]
					for(j=0;j<decount;j++){
						if(dp[j]==nextindex){
							fseek(f2,j*sizeof(int),SEEK_SET);
							fread(&id,sizeof(int),1,f2);
							id++;
							if(id==decount)id = 0;
							if(!exist(id,in,incount)){
								printf("%d hasn't existed before.\n ",id);
								in = (int*)realloc(in,sizeof(int)*(incount+1));
								in[incount++]=id;
							}
						}
					}
					break;
				} 
				nextindex=ctable[(int)next]+occfunc(next,nextindex,d1,f1)-1;
				fseek(f1,nextindex,SEEK_SET);
				fread(&next,sizeof(char),1,f1);
				printf("nextindex: %d, next(ascii): %d\n",nextindex,next);
			}*/
		}

		printf("\n--------------in----------------\n");
		for(i = 0;i<incount;i++){
			printf("%d ",in[i]);
		}
		if(mna[1]=='n'){
			printf("\n---------------n result----------------\n");
			printf("%d\n",incount);
		}
		else if(mna[1]=='a'){
			printf("decount:%d\n",decount);
			qsort(in,incount,sizeof(in[0]),cmp);
			printf("\n---------------a result----------------\n");
			for(i = 0;i<incount;i++){
				//if(in[i]==decount)printf("1\n");
				printf("%d\n",in[i]);
			}
		}
	}
	printf("\n");
	fclose(f1);
	fclose(f2);
	if(num && !d1)fclose(d1);
	return 0;
}

int main(int argc, char* argv[])
{
	if(!strncmp(argv[4],"-m",2))mnasearch(argv[1],argv[2],argv[3], argv[4], argv[5]);
	else if(!strncmp(argv[4],"-n",2))mnasearch(argv[1],argv[2],argv[3],argv[4],argv[5]);
	else if(!strncmp(argv[4],"-a",2))mnasearch(argv[1],argv[2],argv[3],argv[4],argv[5]);
	else if(!strncmp(argv[4],"-i",2))mnasearch(argv[1],argv[2],argv[3],argv[4],argv[5]);
	return 0;
}
