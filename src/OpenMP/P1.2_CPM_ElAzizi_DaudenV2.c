#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <omp.h>

#define CERT 1
#define FALS 0

typedef struct {
    int taula[9][9];
    int i;
    int j;
    int nelem;
} data;

data * taules;

int possible_vaules[9];
int num_possible_vaules = 0;


int max_num_treads;
int num_trees = 0;

// Ha de ser inicialment correcta !!
int taula[9][9] = \
        {1,2,3, 4,5,6,  7,8,9,  \
         9,8,7, 3,2,1,  6,5,4,  \
         6,5,4, 7,8,9,  1,2,3,  \
\
         7,9,8, 1,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
\
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0,  \
         0,0,0, 0,0,0,  0,0,0};


void print_table( int table[][9] ){
    int i, j;
    for( i = 0; i < 9; i++){
        for( j = 0; j < 9; j++){
            printf( "%d   ", table[i][j] );
        }
        printf("\n");
    }
    printf("\n");
}

void copy_table( int new_table[][9], int last_table[][9]  ){
    int i, j;
    for( i = 0; i < 9; i++){
        for( j = 0; j < 9; j++){
            new_table[i][j] = last_table[i][j];
        }
    }
}

/*Comprova que en la posició (x,y) del sudoku es pugi ficar l'element z*/
int puc_posar(int x, int y, int z, int thread )
{
    int i,j,pi,pj;
    //int thread = omp_get_thread_num();

    for ( i=0; i<9; i++ ){
        if ( taules[thread].taula[x][i] == z ) return(FALS);
        if ( taules[thread].taula[i][y] == z ) return(FALS);
    }
    // Quadrat
    pi = ( x / 3 ) * 3; //truncament
    pj = y - y % 3; //truncament
    for ( i = 0; i < 3; i++) 
        for ( j = 0; j < 3; j++) 
            if ( taules[thread].taula[ pi+i ][ pj+j ] == z ) return(FALS);
    return(CERT);
}

////////////////////////////////////////////////////////////////////
int recorrer( int i, int j, int thread )
{
    int k;
    long int s = 0;
    //int thread = omp_get_thread_num();

    if ( taules[thread].taula[i][j] ) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( recorrer( i, j+1, thread ) );
        else if ( i<8 ) return( recorrer( i+1, 0, thread ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    { 
        for ( k=1; k < 10; k++ )
            if ( puc_posar( i, j, k, thread ) ) 
            {
                taules[thread].taula[i][j] = k; 
                if (j<8) s += recorrer( i, j+1, thread );
                else if (i<8) s += recorrer( i+1, 0, thread );
                else s++;
                taules[thread].taula[i][j] = 0;
            }
    }
    return(s);
}

/*Comprova que en la posició (x,y) del sudoku es pugi ficar l'element z*/
int init_puc_posar(int x, int y, int z, int taula[][9] )
{
    int i,j,pi,pj;
    //int thread = omp_get_thread_num();

    for ( i=0; i<9; i++ ){
        if ( taula[x][i] == z ) return(FALS);
        if ( taula[i][y] == z ) return(FALS);
    }
    // Quadrat
    pi = ( x / 3 ) * 3; //truncament
    pj = y - y % 3; //truncament
    for ( i = 0; i < 3; i++) 
        for ( j = 0; j < 3; j++) 
            if ( taula[ pi+i ][ pj+j ] == z ) return(FALS);
    return(CERT);
}


data * init_datasetV2( int i, int j, int taula[][9] ){
    int k;
    data * taules_aux = 0;
    int local_num_possible_vaules = 0;
    int possible_vaules[9];

    data ** taules_aux2 = 0;
    data * taules_result = 0;
    int aux = 0, aux2 = 0, aux3 = 0;

    if ( taula[i][j] ) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( init_datasetV2( i, j+1, taula ) );
        else if ( i<8 ) return( init_datasetV2( i+1, 0, taula ) );
        else printf("The table is full of numbers\n");; // Final de la taula
    }
    else // hi ha un 0 hem de provar
    { 
        for ( k=1; k < 10; k++ )
            if ( init_puc_posar( i, j, k, taula ) ) 
            {
                possible_vaules[ local_num_possible_vaules ] = k;
                local_num_possible_vaules ++;
            }
        taules_aux = ( data * ) malloc( sizeof( data ) * local_num_possible_vaules );
        for( k = 0; k < local_num_possible_vaules; k++ ){
            copy_table( taules_aux[k].taula, taula );
            taules_aux[k].taula[i][j] = possible_vaules[ k ];
            taules_aux[k].i = i;
            taules_aux[k].j = j;
            taules_aux[k].nelem = local_num_possible_vaules;
        }


        num_possible_vaules += local_num_possible_vaules;
        if( num_possible_vaules < max_num_treads ){
            taules_aux2 = ( data** ) malloc( sizeof( data* ) * local_num_possible_vaules );
            for( k = 0; k < local_num_possible_vaules; k++ ){
                taules_aux2[k] = init_datasetV2( i, j, taules_aux[k].taula );
                aux += taules_aux2[k][0].nelem;
            }
            taules_result = ( data* ) malloc( sizeof( data ) * aux );
            for( k = 0; k < local_num_possible_vaules; k++ ){
                for( aux2 = 0; aux2 < taules_aux2[k][0].nelem; aux2++ ){
                    taules_result[ aux3 ] = taules_aux2[k][aux2];
                    taules_result[ aux3 ].nelem = aux;
                    aux3++; 
                }
            }
            taules_aux = taules_result;
        }
    }
    return taules_aux;
}


void init_dataset( int i, int j ){
    int k;
    int aux = 0, aux2 = 0, aux3 = 0;
    data ** taules_aux = 0;
    data * taules_result = 0;
    int num_possible_vaules_aux;

    if ( taula[i][j] ) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( init_dataset( i, j+1 ) );
        else if ( i<8 ) return( init_dataset( i+1, 0 ) );
        else printf("The table is full of numbers\n"); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    { 
        for ( k=1; k < 10; k++ )
            if ( init_puc_posar( i, j, k, taula ) ) 
            {
                possible_vaules[ num_possible_vaules ] = k;
                num_possible_vaules ++;
            }
        taules = ( data *) malloc( sizeof( data) * num_possible_vaules );
        for( k = 0; k < num_possible_vaules; k++ ){
            copy_table( taules[k].taula, taula );
            taules[k].taula[i][j] = possible_vaules[ k ];
            taules[k].i = i;
            taules[k].j = j;
        }

        if( num_possible_vaules < max_num_treads ){
            taules_aux = ( data** ) malloc( sizeof( data* ) * num_possible_vaules );
            num_possible_vaules_aux = num_possible_vaules;
            for( k = 0; k < num_possible_vaules_aux; k++ ){
                taules_aux[k] = init_datasetV2( i, j, taules[k].taula );
                aux += taules_aux[k][0].nelem;
            }
            taules_result = ( data* ) malloc( sizeof( data ) * aux );
            for( k = 0; k < num_possible_vaules_aux; k++ ){
                for( aux2 = 0; aux2 < taules_aux[k][0].nelem; aux2++ ){
                    taules_result[ aux3 ] = taules_aux[k][aux2];
                    aux3++; 
                }
            }
            num_possible_vaules = aux;
            taules = taules_result;
        }
    }
}


////////////////////////////////////////////////////////////////////
int main( int nargs, char* args[] )
{
    int i, parts;
    long int nsol = 0;

    assert( nargs == 2 );
    parts = atoi( args[1] );
    if ( parts < 2 ) assert("Han d'haver dues parts com a minim" == 0);
    omp_set_num_threads( parts );
    max_num_treads = parts;

    //max_num_treads = omp_get_max_threads();
    printf("Number of simultaneous threads that are going to be used: %d\n", max_num_treads );

    init_dataset(0, 0);
    /*for( i=0; i<num_possible_vaules; i++ ){
        print_table( taules[i].taula );
    }*/
    #pragma omp parallel for reduction( +:nsol ) schedule( dynamic, 1)
    for( i=0; i<num_possible_vaules; i++ ){
        printf( "Executing part %d by thread %d\n", i, omp_get_thread_num() );
        nsol += recorrer( taules[i].i, taules[i].j, i );
    }
    printf( "numero solucions : %ld\n", nsol );
    exit( 0 );
}
