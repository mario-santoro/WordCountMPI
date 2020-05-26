#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/param.h>
#include <time.h>
#define LENGTH 30000 //al massimo 1000 parole
#define CHARLENGTH 20 //non esistono parole più lunghe di 20 caratteri
char PATHNAME[MAXPATHLEN];
//struttura che identifica la singola parola e la sua frequenza (o ricorrenza) 
typedef struct Word
{
    char name[CHARLENGTH]; 
    int frequenza;
} Word;

//funzione utile per aprire i file nella directory "file" e salvare le parole in un array bidimensionale. 
//Nota: i file possono avere qualsiasi nome nella cartella "file" e possono anche essere creati altri, ma non può cambiare il nome e la posizione della cartella "file"
int RiempiArray(char array[LENGTH][CHARLENGTH])
{
    char ch;
    int j = 0;
    FILE *fp;
    int start = 0;
    DIR *dirp = NULL;
    struct dirent *dp;
    char path[MAXPATHLEN];

    //getwd restituisce un percorso file assoluto che rappresenta la directory di lavoro corrente.
    if (!getwd(PATHNAME))
    {
        printf("Error getting path\n");
        exit(0);
    }
    //concateno il percorso della directory corrente con /file per indicare dove recuperare i file
    strcat(PATHNAME, "/file/");

    /*prendo i file dalla cartella e apro i file per l'assegnamento dell'array*/
    dirp = opendir(PATHNAME);
    while (dirp)
    {
        if ((dp = readdir(dirp)) != NULL)
        {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
            {
                //recupero i nomi dei file dalla cartella e li uso per aprire i file
                char *ptr = dp->d_name;
                strcpy(path, PATHNAME);
                strcat(path, ptr);
                if ((fp = fopen(path, "rt")) == NULL)
                {
                    printf("Errore nell'apertura del file'");
                    exit(1);
                }
                /*scorro file e riempio array*/
                while ((ch = fgetc(fp)) != EOF)
                {
                    //se è diverso da \n allora è ancora la stessa parola
                    if (ch != '\n')
                    {
                        array[start][j] = ch;
                        j++;
                    }
                    //se è \n aggiungo il tappo per concludere parola e vado avanti alla riga successiva (nuova parola) azzerando il contatore dei caratteri
                    if (ch == '\n')
                    {
                        array[start][j] = '\0';
                        start++;
                        j = 0;
                    }
                }
            }
        }
        else
        {
            closedir(dirp);
            dirp = NULL;
        }
    }
    fclose(fp);
    closedir(dirp);
    return start;
}

// funzione utile per la stampa dell'array, usata per delle prove
void stampaArray(char array[LENGTH][CHARLENGTH], int size, int rank)
{
    for (int i = 0; i < size; i++)
    {
        printf("Sono %d stampo %s\n", rank, array[i]);
    }
}
//funzione che restituisce il numero di elementi nella struttura
int contaStrutt(Word *words)
{
    int i;
    for (i = 0; i < LENGTH; i++)
    {
        //se la frequenza è 0 allora non ci sono altri elementi e posso uscire dal for
        if (words[i].frequenza == 0)
        {
            break;
        }
    }
    return i;
}

//funzione che calcola la frequenza delle parole in una porzione di array
int calcolaFrequenza(char array[LENGTH][CHARLENGTH], int row, Word words[LENGTH])
{
    //setto flag a 0
    int bool = 0;
    //inserisco primo elemento dell'array nella struttura con frequenza 1
    strcpy(words[0].name, array[0]);
    words[0].frequenza = 1;
    int size = 1;
    //scorro tutto l'array parola per parola
    for (int i = 1; i < row; i++)
    {        
        //scorro la struttura
        for (int j = 0; j < size; j++)
        {        
            //controllo se è gia presente la parola nella struttura, 
            //nel caso aumento la frequenza della parola e setto il flag a 1, infine posso uscire dal ciclo che scorre la struttura
            if (strcmp(array[i], words[j].name) == 0)
            {
                bool = 1;              
                words[j].frequenza++;
                break;
            }
        }
        //se il flag è ancora a 0 non ho trovato l'occorrenza della parola nella struttura
        // quindi si aggiunge una nuova parola con frequenza 1 nella struttura
        if (bool == 0 && strcmp(array[i],"")!=0)
        {
            strcpy(words[size].name, array[i]);
            words[size].frequenza = 1;
            size++;          
        }
        bool = 0;
    }

    return size;
}

//funzione che divide equamente le parole dell'array tra i processi
void partitioning(int taglia, int p, int *partitioning)
{
    //dimensione array modulo numero di processi otteniamo il resto
    int modulo = taglia % p;
      //inizializzo l'array con i valori (iniziali) uguali per tutti
    for (int i = 0; i < p; i++)
    {
        partitioning[i] = taglia / p;
    }
    //se il resto non è 0 
    if (modulo != 0)
    {      
        int temp = 0;
        //finchè il resto non è zero viene smistato il valore tra le varie porzioni
        while (modulo != 0)
        {
            //il tmp deve essere sempre modulo p
            partitioning[temp%p] = partitioning[temp%p] + 1;
            temp++;    
            modulo--;
        }
    }
}


//completa e unifica i risultati di due struttura in un unica struttura: w2
int unisciResult(Word w1[LENGTH], int sizeW1, Word w2[LENGTH])
{
    int bool = 0;
    int sizeW2 = contaStrutt(w2);  
    for (int i = 0; i < sizeW1; i++)
    {        
        for (int j = 0; j < sizeW2; j++)
        {
            //se la parola della prima struttura è già presente nella seconda 
            //allora viene soltanto fatta l'addizione delle 2 frequenze
            if (strcmp(w1[i].name, w2[j].name) == 0)
            {
                w2[j].frequenza = w2[j].frequenza + w1[i].frequenza;            
                bool = 1;
            }
        }
        //se il flag è 0 (e la parola non deve essere vuota) la parola non è presente nella seconda struttura
        //quindi viene aggiunta la parola e la frequenza trovata
        if (bool == 0 &&  strcmp(w1[i].name, "")!=0)
        {
            strcpy(w2[sizeW2].name, w1[i].name);
            w2[sizeW2].frequenza = w1[i].frequenza;           
            sizeW2++;
        }
        bool = 0;
    }
    return sizeW2;
}

//funzione di prova per stampare la struttura
void stampaStruttura(Word *words, int myRank)
{
    int i;
    int size = contaStrutt(words);
    for (i = 0; i < size; i++)
    {
        printf("sono processo %d, ho parola: %s, con frequenza %d\n", myRank, words[i].name, words[i].frequenza);
    }
}

//funzione che crea il file csv con i risultati
void creaCSV(Word w[LENGTH], int size){
    //creo file csv per salvare i risultati
    FILE *fpcsv;
    fpcsv = fopen("risultati.csv", "w+");
    fprintf(fpcsv, "OCCORRENZA,PAROLA");
    for (int t = 0; t < size; t++)
    {
        fprintf(fpcsv, "\n%s,%d", w[t].name, w[t].frequenza);
    }
    fclose(fpcsv);
}

int main(int argc, char *argv[])
{
    int myRank, p;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    int tag = 0;
    int row;
    char vocab[LENGTH][CHARLENGTH];
    int size;
    MPI_Request req; 
    MPI_Datatype type;
    //definisce lunghezza colonne cioè la lunghezza della parola di invio può essere fino a 20 invece che singolo carattere
    //utile per passare l'array bidimensionale con send e recive
    MPI_Type_contiguous(20, MPI_CHAR, &type); //20 perchè stabilisce la lunghezza in colonne dell'array bidiemnsionale
    MPI_Type_commit(&type);
    //Creo la struttura Word come nuovo tipo MPI per passarlo con send e recive 
    MPI_Datatype st, oldtypes[2]; // required variables
    int blockcounts[2];
    MPI_Aint offsets[2], lb, extent;
    //definisco i valori della struttura
    offsets[0] = 0;
    oldtypes[0] = MPI_CHAR;
    blockcounts[0] = 20;

    MPI_Type_get_extent(MPI_CHAR, &lb, &extent);

    offsets[1] = 4 * extent;
    oldtypes[1] = MPI_INT;
    blockcounts[1] = 1;

    //per inviare la struttura come nuovo tipo
    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &st);
    MPI_Type_commit(&st);
    //avvio calcolo tempo
    clock_t begin = clock();
    if (myRank == 0)
    {
        size = RiempiArray(vocab);       
        Word w[LENGTH];
        Word tmp[LENGTH];
        int part[p]; //array di interi che conterrà l'ultimo elemento dell'array 
        //da inviare ad ogni processo dopo la funzione partitioning
        partitioning(size, p, part);
        int start = part[0];
        for (int i = 1; i < p; i++)
        {            
            //invio agli altri processi la loro porzione da computare
            MPI_Isend(&vocab[start], part[i], type, i, tag, MPI_COMM_WORLD,&req);           
            start = start + part[i];
        }
        //rank 0 computa la sua parte, ottenendo la struttura con parole e frequenze parziale
        int size2 = calcolaFrequenza(vocab, part[0], w);          
        //ricevo da tutti i processi la loro parte di computazione
        for (int i = 1; i < p; i++)
        {
            //ricevo la struttura calcolata da ciuascun processo
            MPI_Recv(&tmp, LENGTH, st, i, tag, MPI_COMM_WORLD, &status); 
            int count;
            count=contaStrutt(tmp);
            //unisco i risultati mandati dal processo i con i risultati globali contenuti nella struttura del processo 0          
            unisciResult(tmp, count, w);
        }
        size2=contaStrutt(w);
       //creao file csv
       creaCSV(w, size2);
       
        //calcolo e stampo il tempo di computazione
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("execution time = %lf\n",time_spent);
    }
    else
    {
        //il processo riceve la sua porzione di array
        MPI_Recv(&vocab, LENGTH, type, 0, tag, MPI_COMM_WORLD, &status);        
        int count;
        MPI_Get_count(&status, type, &count);     
        Word words[LENGTH];
        //calcola la frequenza delle parole contenute nella porzione di array inviatogli
        int size2 = calcolaFrequenza(vocab, count, words);           
        MPI_Send(&words, count, st, 0, tag, MPI_COMM_WORLD);
    }
    //libero la memoria e chiudo i processi
    MPI_Type_free(&st);
    MPI_Type_free(&type);
    MPI_Finalize();
   
    return 0;
}
