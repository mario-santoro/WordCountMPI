#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<stdlib.h>
char ** ArrayDinamico (FILE *fp){
    char **array;
    array=(char **)malloc( (10) * sizeof(char*) );
    char ch;
    int i=0; int j =0;
    while((ch = fgetc(fp)) != EOF){
        if(ch!='\n'){         
            j++;
         }         
        if(ch=='\n'){
        //  printf("Stampo carattere numero %d appartantente alla parola %d\n", j,i);
          array[i] =(char *) malloc( (j) * sizeof(char) );
          i++;
          if(i>=10){
            array = realloc( array ,(i+1) * sizeof(char *) );
          }
          j=0;
        }       
    }
    // printf("Stampo carattere numero %d appartantente alla parola %d\n", j,i);
    array[i] = malloc( (j) * sizeof(char) );
    array = realloc( array ,(i+1) * sizeof(char *) );
    return array;
}

int riempioArray(char** array, FILE*fp){
  int i=0; int j = 0;
  char ch;
    while((ch = fgetc(fp)) != EOF){
      if(ch!='\n'){
     // printf("Stampo carattere %c numero %d appartantente alla parola %d\n", ch,j,i);
        array[i][j]=ch;
       //printf("%c",vocab[i][j]);
      }
      j++;
      if(ch=='\n'){
        i++;
        j=0;
      }       
    }

return i+1;

}
void stampaArray(char ** array, int row){
      // printf("%d\n",row);
    for(int i=0;i<row;i++){
        //  printf("%ld\n",strlen(vocab[i]));
         for(int j=0;j<strlen(array[i]);j++){
             printf("%c",array[i][j]);
         }
        printf("\n");
    }

}

int main(int argc, char *argv[]){
	int myRank,p;
	//MPI_Status status;
	//MPI_Init(&argc, &argv);
	//MPI_Comm_size(MPI_COMM_WORLD, &p);
	//MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int tag=0;	
	FILE *fp;
	char  ch;	
	if((fp=fopen("file/text2.txt", "rt"))==NULL) {
		printf("Errore nell'apertura del file'");
		exit(1);
	}	
	char ** vocab = ArrayDinamico(fp);      
  rewind(fp);
  int row= riempioArray(vocab, fp);
	fclose(fp);
  stampaArray(vocab, row);
	//MPI_Finalize();
	return 0;
}