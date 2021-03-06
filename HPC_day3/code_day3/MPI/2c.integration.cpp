/*==================================================
 * Purpose: A program to test MPI_Ssend and MPI_Recv
 * 
 * Integrate f(x)
 * 
 *==================================================*/

#include <stdio.h>
#include <iostream>
#include <mpi.h>
#include <math.h>
#include <stdio.h>

using namespace std;

float integral(float ai, float h, int n);

int main(int argc, char **argv)
{
      int n, p, myid, tag, proc, ierr;
      float h, integral_sum, a, b, ai, pi, my_int;
      int master = 0;  /* processor performing total sum */
      MPI_Comm comm;
      MPI_Status status;

      comm = MPI_COMM_WORLD;      
      ierr = MPI_Init(&argc,&argv);         
      MPI_Comm_rank(comm, &myid);         
      MPI_Comm_size(comm, &p);            

      pi = acos(-1.0);  /* = 3.14159... */
      a = 0.;           /* lower limit of integration */
      b = pi*1./2.;     /* upper limit of integration */
      n = 500;          /* number of increment within each process */
      tag = 123;        /* set the tag to identify this particular job */
      h = (b-a)/n/p;    /* length of increment */

      ai = a + myid*n*h;  /* lower limit of integration for partition myid */
      my_int = integral(ai, h, n);   /* 0<=myid<=p-1 */

      cout << "Process " << myid <<" has the partial integral of " << my_int << endl;

      MPI_Send(        /* Send my_int from myid to master */
              &my_int, 1, MPI_FLOAT, 
              master,        /* message destination */
              tag,          /* message tag */
              comm);

      if(myid == master) {
        integral_sum = 0.0;
        for (proc=0;proc< p; proc++) 
	
	{   /* for-loop serializes receives */
          MPI_Recv(         /* Receive my_int from proc to master  */
                &my_int, 1, MPI_FLOAT, 
                proc,        /* message source */
                tag,         /* message tag */
                comm, & status);     /* status reports source, tag */
          integral_sum += my_int;
        }
        cout << "The Integral = " << integral_sum << endl; /* sum of my_int */
      }
      MPI_Finalize();                        /* let MPI finish up ... */
}


float integral(float ai, float h, int n)
{
      int j;
      float aij, integ;

      integ = 0.0;                 /* initialize */
      for (j=0;j<n-1;j++) {            /* sum integrals */
        aij = ai + (j+0.5)*h;      /* mid-point */
        integ += cos(aij)*h;
      }
      return integ;
}
