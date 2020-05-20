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
  char name[20];
  int frequenza;  
}Word;


//funzione utile per aprire i file nella directory e salvare le parole in un array bidimensionale
int ArrayDinamico (char array[1000][20]){
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
   
/*prendo i file dalla cartella e apro i file per l'assegnamento dell'array*/
             
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
              /*scorro file e riempio array*/
              while((ch = fgetc(fp)) != EOF){      
                if(ch!='\n'){         
                    array[start][j]=ch;     
                   
                    j++;
                 }         
                 if(ch=='\n'){
                     array[start][j]='\0';                                                                                               
                    start++;                
                    j=0;
                  }       
              }    
            }
        }else{
            closedir(dirp);
            dirp = NULL;
        }        
    }   
 
    fclose(fp); 
    closedir(dirp);  
    return start;
}

// funzione utile per la stampa dell'array, usata per delle prove
void stampaArray(char array[1000][20], int size, int rank){
    for(int i=0;i<size;i++){
             printf("Sono %d stampo %s\n",rank, array[i]);
    }

}

int contaStrutt(Word * words){ 
  int i;
  for( i=0;i<1000;i++){
     
      if(words[i].frequenza==0){        
          break;
      }
  }
return i;
}

//funzione che calcola la frequenza delle parole in una porzione di array
int calcolaFrequenza(char array[1000][20], int row, Word words[100]){  

  int bool; 
                 
        strcpy(words[0].name, array[0]);       
        words[0].frequenza=1;      
            int size;                
       for(int i=1;i<row;i++){       
          int j;
           size= contaStrutt(words);
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
 
     size= contaStrutt(words);
    
 
 return size;
}


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

//completa e unifica la struttura w2
int unisciResult(Word w1[1000], int sizeW1, Word w2[1000], int sizeW2){
    int bool=0;
    for(int i=0; i<sizeW1; i++ ){
        for(int j=0; j<sizeW2; j++){
            
            if(strcmp(w1[i].name, w2[j].name)==0){
                w2[j].frequenza++;
                printf("%s frequenza %d\n",w2[j].name,w2[j].frequenza);
                bool=1;
            }

        }
        if(bool==0){
            
            strcpy(w2[sizeW2].name, w1[i].name);
            w2[sizeW2].frequenza=1;
            printf("aggiungo %s con frequenza %d\n",w2[sizeW2].name,w2[sizeW2].frequenza);
            sizeW2++;
        }
    bool=0;
    }

    return sizeW2;
}

int main(int argc, char *argv[]){
	int myRank,p;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int tag=0;
    int row;
    char vocab[1000][20];
    int size;
    MPI_Datatype type;
    MPI_Type_contiguous(20, MPI_CHAR, &type ); //definisce lunghezza colonne cioè la lunghezza della parola di invio può essere fino a 20 invece che singolo carattere
    MPI_Type_commit(&type);
    MPI_Datatype st, oldtypes[2];   // required variables
    int blockcounts[2];

   // MPI_Aint type used to be consistent with syntax of
   // MPI_Type_extent routine
    MPI_Aint offsets[2], lb, extent; 
  

   offsets[0] =0;
   oldtypes[0] = MPI_CHAR;
   blockcounts[0] = 20;

   MPI_Type_get_extent(MPI_CHAR, &lb, &extent);

   offsets[1] = 4*extent;
   oldtypes[1] = MPI_INT;
   blockcounts[1] = 1;

    //per inviare la struttura come nuovo tipo
    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &st);
    MPI_Type_commit(&st);
    
      //array3 =(char *) malloc( (19) * sizeof(char) );
  clock_t begin = clock();
  if(myRank==0){
    size = ArrayDinamico(vocab); 
    printf("array lungo %d\n",size);
    Word  w[100];
     // stampaArray(vocab, size, 0);
    int part[p];
    partitioning(size, p, part);
    int start= part[0];
   
    for(int i=1; i<p; i++){

				MPI_Send(&vocab[start], part[i], type, i, tag, MPI_COMM_WORLD); //lunghezza righe da passare
				printf("sono %d, invio array con elemento %s in posizione %d fino a %d\n",myRank,vocab[start], start,start+part[i]);
                start =start +part[i];
  		
          }	
       
       int size2= calcolaFrequenza(vocab, part[0], w); 
        int size3;
         printf("sono processo con rank %d e la lunghezza della struttura è %d\n",myRank, size2);
        Word tmp[100];
        for(int i=1; i<p; i++){

			MPI_Recv(&tmp, 100, st, i, tag, MPI_COMM_WORLD, &status); //lunghezza righe da passare
			printf("sono %d, ricevo array con elemento %s in posizione %d\n",myRank,tmp[i].name, i);
            int count;
            MPI_Get_count(&status,st,&count);

           size3=unisciResult(tmp, count, w, size2);
             printf("dopo l'unione con %d la lunghezza è: %d\n",i ,size3);
          }	

        //creo file csv
        FILE *fpcsv;
        fpcsv=fopen("risultati.csv","w+");
        fprintf(fpcsv,"OCCORRENZA,PAROLA");
        for(int t=0;t<size3;t++){
                fprintf(fpcsv,"\n%s,%d",w[t].name, w[t].frequenza);
        }
        fclose(fpcsv);
       
   }else{
              
      MPI_Recv(&vocab, 1000, type, 0, tag, MPI_COMM_WORLD, &status);
       // printf("sono %d, ricevo array con elemento %s\n",myRank,vocab[0]);
        // printf("sono %d, ricevo array con elemento %s\n",myRank,vocab[3]);
    int count;
     MPI_Get_count(&status,type,&count);
    	printf("%d\n", count);
       stampaArray(vocab, count, myRank);
     Word words[100];
     
     int size2= calcolaFrequenza(vocab, count,words); 

       
     //printf("sono processo con rank %d e la lunghezza della struttura è %d\n",myRank, size2);
      MPI_Send(&words, size2, st, 0, tag, MPI_COMM_WORLD);
  
	} 


      MPI_Type_free(&st);
        MPI_Type_free(&type);
	MPI_Finalize();
return 0;
}