#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <omp.h>

#define CERT 1
#define FALS 0

int max_num_treads;
int max_deep = 3;

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
int puc_posar(int x, int y, int z, int taula[][9] )
{
    int i,j,pi,pj;

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

////////////////////////////////////////////////////////////////////
int recorrer2( int i, int j, int taula[][9] )
{
    int k;
    long int s = 0;

    if ( taula[i][j] ) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( recorrer2( i, j+1, taula ) );
        else if ( i<8 ) return( recorrer2( i+1, 0, taula ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    {
        for ( k=1; k < 10; k++ )
        {
            if ( puc_posar( i, j, k, taula ) ) 
            {
                taula[i][j] = k; 
                if (j<8) s += recorrer2( i, j+1, taula );
                else if (i<8) s += recorrer2( i+1, 0, taula );
                else s++;
                taula[i][j] = 0;
            }
        }
    }
    return(s);
}

////////////////////////////////////////////////////////////////////
int recorrer( int i, int j, int taula[][9], int deep )
{
    int k;
    long int s = 0;

    if ( taula[i][j] ) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( recorrer( i, j+1, taula, deep ) );
        else if ( i<8 ) return( recorrer( i+1, 0, taula, deep ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    {
        if( deep < max_deep ){
            #pragma omp parallel
            {
                #pragma omp for reduction( +:s )
                for ( k=1; k < 10; k++ )
                {
                    if ( puc_posar( i, j, k, taula ) ) 
                    {
                        int new_taula[9][9];
                        copy_table( new_taula, taula );
                        new_taula[i][j] = k; 
                        if (j<8) s += recorrer( i, j+1, new_taula, deep++ );
                        else if (i<8) s += recorrer( i+1, 0, new_taula, deep++ );
                        else s++;
                        new_taula[i][j] = 0;
                    }
                }
            }
        }
        else{
            for ( k=1; k < 10; k++ )
            {
                if ( puc_posar( i, j, k, taula ) ) 
                {
                    taula[i][j] = k; 
                    if (j<8) s += recorrer2( i, j+1, taula );
                    else if (i<8) s += recorrer2( i+1, 0, taula );
                    else s++;
                    taula[i][j] = 0;
                }
            }
        }
    }
    return(s);
}

////////////////////////////////////////////////////////////////////
int main( int nargs, char* args[] )
{
    int threads;
    long int nsol = 0;

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

    assert( nargs == 2 );
    threads = atoi( args[1] );
    if ( threads < 2 ) assert("Han d'haver dues parts com a minim" == 0);

    omp_set_num_threads( threads );
    omp_set_nested( 1 );

    nsol = recorrer( 0, 0, taula, 0 );
    printf( "numero solucions : %ld\n", nsol );
    exit( 0 );
}
