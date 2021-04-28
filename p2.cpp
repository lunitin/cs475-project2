#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// print debugging messages?
#ifndef DEBUG
#define DEBUG	false
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	1
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	0.70

float Height( int, int, int);


// main program:
int
main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	int NUMT = 4;
	int NUMNODES = 5500;

	if (argc >= 2)
		NUMT = atoi(argv[1] );
	if (argc >= 3)
		NUMNODES = atoi(argv[2]);

	omp_set_num_threads( NUMT );	// set the number of threads to use in parallelizing the for-loop:`


	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:
	//
	double maxPerformance = 0.;	// must be declared outside the NUMTRIES loop
	double volume = 0.;


	// looking for the maximum performance:
	for( int tries = 0; tries < NUMTRIES; tries++ )
	{
		double time0 = omp_get_wtime( );

		#pragma omp parallel for default(none), shared(NUMNODES,fullTileArea,stderr), reduction(+:volume)
		for( int i = 0; i < NUMNODES*NUMNODES; i++ )
		{
			int iu = i % NUMNODES;
			int iv = i / NUMNODES;

			float area = fullTileArea;
			float z = Height( iu, iv, NUMNODES );

			// Modify area based on location
			
			// If this tile is on an edge
			if (iu == 0 || iv == 0 || iu == NUMNODES - 1 || iv == NUMNODES - 1) {
				area /= 2;
				fprintf(stderr, "Reducing value for an edge\n");
			}

			// If this tile is a corner 
			if ( (iu == 0 && iv == 0) ||
			     (iu == 0 && iv == NUMNODES -1) ||
			     (iu == NUMNODES -1 && iv == 0) ||
			     (iu == NUMNODES -1 && iv == NUMNODES -1) ) {
				area /= 2;
				fprintf(stderr, "Reducing value for an corner\n");
			}
			   

			volume +=  area* z;
			
		}

		double time1 = omp_get_wtime( );
		double megaVolsPerSecond = (double)NUMNODES*NUMNODES / ( time1 - time0 ) / 1000000.;
		if( megaVolsPerSecond > maxPerformance )
			maxPerformance = megaVolsPerSecond;

	}

	volume *= 2;

	fprintf(stderr, "%2d threads : %8d nodes ; volume = %6.5lf%% ; megavols/sec = %6.2lf\n",
		NUMT, NUMNODES, volume, maxPerformance);

}



float Height( int iu, int iv, int NUMNODES )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(float)N );
	return height;
}

