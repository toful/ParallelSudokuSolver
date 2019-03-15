#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define CERT 1
#define FALS 0

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

void copy_table( int new_table[][9] ){
    for( int i = 0; i < 9; i++){
        for( int j = 0; j < 9; j++){
            new_table[i][j]=taula[i][j];
        }
    }
}

/*Comprova que en la posició (x,y) del sudoku es pugi ficar l'element z*/
int puc_posar(int x, int y, int z)
{
    int i,j,l,pi,pj;
    // Quadrat
    pi = ( x / 3 ) * 3; //truncament
    pj = y - y % 3; //truncament

    //#pragma omp parallel
    {
        //#pragma omp for
        for ( l=0; l<9; l++ ){
            if ( taula[x][l] == z || taula[l][y] == z ) return(FALS); // Comprovem que z no estigui en la fila ni columna x 
        }
        //#pragma omp for
        for ( i = 0; i < 3; i++) 
            for ( j = 0; j < 3; j++) 
                if ( taula[ pi+i ][ pj+i ] == z ) return(FALS);
    }
    return(CERT);
    
}

////////////////////////////////////////////////////////////////////
int recorrer( int i, int j )
{
    int k;
    long int s = 0;

    if (taula[i][j]) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( recorrer( i, j+1 ) );
        else if ( i<8 ) return( recorrer( i+1, 0 ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    { 
        for ( k=1; k < 10; k++ )
            if ( puc_posar( i, j, k ) ) 
            {
                taula[i][j] = k; 
                if (j<8) s += recorrer( i, j+1 );
                else if (i<8) s += recorrer( i+1, 0 );
                else s++;
                taula[i][j] = 0;
            }
    }
    return(s);
}

////////////////////////////////////////////////////////////////////
int recorrer_v2( int i, int j, int table[][9] )
{
    int k;
    long int s = 0;

    if (table[i][j]) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( recorrer_v2( i, j+1, table ) );
        else if ( i<8 ) return( recorrer_v2( i+1, 0, table ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    { 
        for ( k=1; k < 10; k++ )
            if ( puc_posar( i, j, k ) ) 
            {
                taula[i][j] = k; 
                if (j<8) s += recorrer_v2( i, j+1, table );
                else if (i<8) s += recorrer_v2( i+1, 0, table );
                else s++;
                taula[i][j] = 0;
            }
    }
    return(s);
}

////////////////////////////////////////////////////////////////////
int First_recorrer( int i, int j )
{
    int k;
    long int s = 0;

    if (taula[i][j]) //Valor fixe no s'ha d'iterar
    {
        if ( j<8 ) return( First_recorrer( i, j+1 ) );
        else if ( i<8 ) return( First_recorrer( i+1, 0 ) );
        else return( 1 ); // Final de la taula
    }
    else // hi ha un 0 hem de provar
    {
        //#pragma omp parallel for default(none) private(taula, k) shared(i, j) reduction(+:s)
        #pragma omp parallel for reduction(+:s) 
        for ( k=1; k < 10; k++ )
            if ( puc_posar( i, j, k ) ) 
            {
                int new_table[9][9];
                copy_table( new_table );
                new_table[i][j] = k; 
                if (j<8) s += recorrer_v2( i, j+1, new_table );
                else if (i<8) s += recorrer_v2( i+1, 0, new_table );
                else s++;
                //taula[i][j] = 0;
            }
    }
    return(s);
}
////////////////////////////////////////////////////////////////////
int main()
{
    //int i,j,k;
    long int nsol;

    //nsol = recorrer( 0, 0 );
    nsol = First_recorrer( 0, 0 );
    printf( "numero solucions : %ld\n", nsol );
    exit( 0 );
}
