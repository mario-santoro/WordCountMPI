#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/param.h>
#include<time.h>
char PATHNAME[MAXPATHLEN];
typedef struct Word
{
  char name[50];
  int frequenza;  
}Word;

Word * calcolaFrequenza(char** array, int row){  
  Word * words;
  words =(Word *)malloc( (row) * sizeof(Word) );
  int bool; 
   if(array!=NULL){               
        strcpy(words[0].name, array[0]);       
        words[0].frequenza=1;      
         int size= contaStrutt(words);                     
       for(int i=1;i<row;i++){       
          int j;
          int size= contaStrutt(words);
         // printf("%d\n",size);
          for(int j=0;j<size;j++){          
            if( strcmp(array[i], words[j].name)== 0){
                bool=1;
              words[j].frequenza++;
              //printf("nome: %s, frequenza %d\n",words[j].name,words[j].frequenza );            
              break;
            }                
          }
        if(bool==0){
            strcpy(words[size].name, array[i]);
            words[size].frequenza=1;
           // printf("nome: %s, frequenza %d\n",words[size].name,words[size].frequenza );
        }
        bool=0;
      }      
    }
    int size= contaStrutt(words);
    
  words= realloc( words , size * sizeof(Word) );
 return words;
}


char ** ArrayDinamico (char ** array){
    char ch;
    int j =0;
  	FILE *fp;
    int start=0;
    char buffer[50];
    DIR *dirp = NULL;
    struct dirent *dp;
    char path[MAXPATHLEN];
 //Recuperare pathname della cartella con i file
    if(!getwd(PATHNAME)){
        printf("Error getting path\n");
        exit(0);
    }
    strcat(PATHNAME, "/file/");
   
/*prendo i file dalla cartella e apro i file per l'allocazione e il riempimento dinamico dell'array*/
             
    dirp=opendir(PATHNAME);
    while (dirp){
        if ((dp = readdir(dirp)) != NULL){
            if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0 ){ 
              //recupero i nomi dei file dalla cartella e li uso per aprire i file 
              char *ptr= dp->d_name;              
              strcpy(path,PATHNAME);              
              strcat(path,ptr );                
	            if((fp=fopen(path, "rt"))==NULL) {
		             printf("Errore nell'apertura del file'");
	  	          exit(1);
  	           }	
              /*scorro file e riempio array in maniera dinamica*/
              while((ch = fgetc(fp)) != EOF){      
                if(ch!='\n'){         
                 buffer[j]=ch;             
                 j++;
                 }         
                 if(ch=='\n'){
                    buffer[j]='\0';           
                    array[start] =(char *) malloc( (j) * sizeof(char) );
                    strcpy(array[start], buffer);                  
                    strcpy(buffer, "");                                        
                    start++;
                    if(start>=30000){
                        array = realloc(array ,(start+1) * sizeof(char *) );
                    }
                    j=0;
                  }       
              }    
            }
        }else{
            closedir(dirp);
            dirp = NULL;
        }        
    }   
    array = realloc( array , start * sizeof(char *) );
    fclose(fp); 
    closedir(dirp);  
    return array;
}

int contaRighe(char** array){ 
  int i;
  for( i=0;i<300000;i++){
      if(array[i]==NULL){        
          break;
      }
  }
return i;
}


int contaStrutt(Word * words){ 
  int i;
  for( i=0;i<30000;i++){
     
      if(words[i].frequenza==0){        
          break;
      }
  }
return i;
}


void stampaStruttura(Word * words){
      int i;
int size= contaStrutt(words);
  for( i=0;i<size;i++){
     
       printf("parola: %s, frequenza %d\n",words[i].name, words[i].frequenza);      
       
  }

}
void stampaArray(char ** array, int row){
    for(int i=0;i<row;i++){
             printf("%s\n",array[i]);
    }

}
/*

void partitioning(int taglia, int p, int * partitioning){
    int modulo=taglia%p;    
 
    if(modulo!=0){
        for(int i=0;i<p;i++){
            partitioning[i]= taglia/p;
        }
        int temp=0;
        while(modulo!=0){
            if(temp<p){
                partitioning[temp]= partitioning[temp]+1;
                temp++;
            }else{
                temp=0;
                partitioning[temp]=partitioning[temp]+1;
                temp++;
            }
            modulo--;
        }
    }else{
        for(int i=0;i<p;i++){
            partitioning[i]= taglia/p;
        }

    }
}

void reduce(double * array, MPI_Datatype tipo, int root, MPI_Comm communicator){
   // double array[N]={10,2,13,42,5,6,1,18,9,4,7,3,2,9};
	int myRank,p;
	MPI_Comm_size(communicator, &p);
	MPI_Comm_rank(communicator, &myRank);
	MPI_Status status;
    int part[p];
    partitioning(N, p, part);
	int tag=0;
        int count;
	if(myRank==root){
            	int start= part[0];
				double m= averege(array, part[0]);
				printf("sono %d, la media parziale è %f\n", myRank, m);
				for(int i=1; i<p; i++){
					MPI_Send(&array[start], part[i], tipo, i, tag, communicator);
					printf("sono %d, invio array con elemento %f in posizione %d\n",myRank,array[start],start);
                    start =start +part[i];
				}		
			 double array2[p];
				array2[0]=m;
                for(int i=1; i<p; i++){					
                    MPI_Recv(&array2[i], 1, tipo, i, tag, communicator, &status);
                    printf("sono %d, ricevo dal processo %d la media %f\n",myRank, i, array2[i]);
                }			
                 double res = averege(array2,p);
                printf("sono %d, ecco la media finale: %f\n",myRank, res);
	}else{
              
                MPI_Recv(&array, N, tipo, root, tag, communicator, &status);
                MPI_Get_count(&status,MPI_DOUBLE,&count);
				double mi= averege(array,count);
                printf("sono %d, la mia media è %f\n",myRank,mi);
				MPI_Send(&mi, 1, tipo, root, tag, communicator);
	}
	MPI_Finalize();
}
*/


int main(int argc, char *argv[]){
	int myRank,p;
	//MPI_Status status;
	//MPI_Init(&argc, &argv);
	//MPI_Comm_size(MPI_COMM_WORLD, &p);
	//MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int tag=0;	
  char**vocab;
  clock_t begin = clock();
  vocab=(char **)malloc( (30000) * sizeof(char*) );
  vocab = ArrayDinamico(vocab); 
  int row=contaRighe(vocab);
  //stampaArray(vocab, row);
  Word * words= calcolaFrequenza(vocab, row);    
  stampaStruttura(words);
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("execution time = %lf\n",time_spent);
	//MPI_Finalize();
	return 0;
}