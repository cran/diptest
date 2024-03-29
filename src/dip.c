/*   ALGORITHM AS 217 APPL. STATIST. (1985) VOL.34, NO.3

  @article{HarP85,
     author = {P. M. Hartigan},
     title = {Computation of the Dip Statistic to Test for Unimodality},
     year = 1985,
     journal = {Applied Statistics},
     pages = {320--325},
     volume = 34 }
  @article{HarJH85,
     author = {J. A. Hartigan and P. M. Hartigan},
     title = {The Dip Test of Unimodality},
     year = 1985,
     journal = {Ann. of Statistics},
     pages = {70--84},
     volume = 13 }

  Does the dip calculation for an ordered vector X using the
  greatest convex minorant and the least concave majorant, skipping
  through the data using the change points of these distributions.

  It returns the dip statistic 'DIP' and the modal interval (XL, XU).
				===			     ======

   dip.f -- translated by f2c (version of 22 July 1992	22:54:52).

   Pretty-Edited and extended (debug argument)
   by Martin Maechler <maechler@stat.math.ethz.ch>
      ETH Seminar fuer Statistik
      8092 Zurich	 SWITZERLAND

---------------

   Two Bug Fixes:
       =========

   1)	July 30 1994 : For unimodal data, gave "infinite loop"  (end of code)
   2)	Oct  31 2003 : Yong Lu <lyongu+@cs.cmu.edu> : ")" typo in Fortran
                       gave wrong result (larger dip than possible) in some cases
*/

#include <R.h>

// for the "registration part":
#include <Rinternals.h>

#include <R_ext/Rdynload.h>


/* Subroutine */
void diptst(const double x[], const int *n_,
	    double *dip, int *lo_hi, int *ifault,
	    int *gcm, int *lcm, int *mn, int *mj,
	    const int *min_is_0, const int *debug)
{
#define low   lo_hi[0]
#define high  lo_hi[1]
#define l_gcm lo_hi[2]
#define l_lcm lo_hi[3]

    const int n = *n_;
    int mnj, mnmnj, mjk, mjmjk, ig, ih, iv, ix,  i, j, k;
    double dip_l, dip_u, dipnew;

    /* Parameter adjustments, so I can do "as with index 1" : x[1]..x[n] */
    --mj;    --mn;
    --lcm;   --gcm;
    --x;

/*-------- Function Body ------------------------------ */

    *ifault = 1;    if (n <= 0) return;
    *ifault = 0;

/* Check that X is sorted --- if not, return with  ifault = 2*/

    *ifault = 2;    for (k = 2; k <= n; ++k) if (x[k] < x[k - 1]) return;
    *ifault = 0;

/* Check for all values of X identical, */
/*     and for 1 <= n < 4. */

/* LOW contains the index of the current estimate  of the lower end
   of the modal interval, HIGH contains the index for the upper end.
*/
    low = 1;	high = n; /*-- IDEA:  *xl = x[low];    *xu = x[high]; --*/

/* M.Maechler -- speedup: it saves many divisions by n when we just work with
 * (2n * dip) everywhere but the very end! */
    *dip = (*min_is_0) ? 0. : 1.;
    if (n < 2 || x[n] == x[1])		goto L_END;

    if(*debug)
	Rprintf("dip() in C: n = %d; starting with  2N*dip = %g.\n",
		n, *dip);

/* Establish the indices   mn[1..n]  over which combination is necessary
   for the convex MINORANT (GCM) fit.
*/
    mn[1] = 1;
    for (j = 2; j <= n; ++j) {
	mn[j] = j - 1;
	while(1) {
	  mnj = mn[j];
	  mnmnj = mn[mnj];
	  if (mnj == 1 ||
	      ( x[j]  - x[mnj]) * (mnj - mnmnj) <
	      (x[mnj] - x[mnmnj]) * (j - mnj)) break;
	  mn[j] = mnmnj;
	}
    }

/* Establish the indices   mj[1..n]  over which combination is necessary
   for the concave MAJORANT (LCM) fit.
*/
    mj[n] = n;
    for (k = n - 1; k >= 1; k--) {
	mj[k] = k + 1;
	while(1) {
	  mjk = mj[k];
	  mjmjk = mj[mjk];
	  if (mjk == n ||
	      ( x[k]  - x[mjk]) * (mjk - mjmjk) <
	      (x[mjk] - x[mjmjk]) * (k - mjk)) break;
	  mj[k] = mjmjk;
	}
    }

/* ----------------------- Start the cycling. ------------------------------- */
LOOP_Start:

    /* Collect the change points for the GCM from HIGH to LOW. */
    gcm[1] = high;
    for(i = 1; gcm[i] > low; i++)
	gcm[i+1] = mn[gcm[i]];
    ig = l_gcm = i; // l_gcm == relevant_length(GCM)
    ix = ig - 1; //  ix, ig  are counters for the convex minorant.

    /* Collect the change points for the LCM from LOW to HIGH. */
    lcm[1] = low;
    for(i = 1; lcm[i] < high; i++)
	lcm[i+1] = mj[lcm[i]];
    ih = l_lcm = i; // l_lcm == relevant_length(LCM)
    iv = 2; //  iv, ih  are counters for the concave majorant.

    if(*debug) {
	Rprintf("'dip': LOOP-BEGIN: 2n*D= %-8.5g  [low,high] = [%3d,%3d]", *dip, low,high);
	if(*debug >= 3) {
	    Rprintf(" :\n gcm[1:%d] = ", l_gcm);
	    for(i = 1; i <= l_gcm; i++) Rprintf("%d%s", gcm[i], (i < l_gcm)? ", " : "\n");
	    Rprintf(" lcm[1:%d] = ", l_lcm);
	    for(i = 1; i <= l_lcm; i++) Rprintf("%d%s", lcm[i], (i < l_lcm)? ", " : "\n");
	} else { // debug <= 2 :
	    Rprintf("; l_lcm/gcm = (%2d,%2d)\n", l_lcm,l_gcm);
	}
    }

/*	Find the largest distance greater than 'DIP' between the GCM and
 *	the LCM from LOW to HIGH. */

    // FIXME: <Rconfig.h>  should provide LDOUBLE or something like it
    long double d = 0.;// <<-- see if this makes 32-bit/64-bit difference go..
    if (l_gcm != 2 || l_lcm != 2) {
	if(*debug) Rprintf("  while(gcm[ix] != lcm[iv]) :%s",
			   (*debug >= 2) ? "\n" : " ");
      do { /* gcm[ix] != lcm[iv]  (after first loop) */
	  long double dx;
	  int gcmix = gcm[ix],
	      lcmiv = lcm[iv];
	  if (gcmix > lcmiv) {
	      /* If the next point of either the GCM or LCM is from the LCM,
	       * calculate the distance here. */
	      int gcmi1 = gcm[ix + 1];
	      dx = (lcmiv - gcmi1 + 1) -
		  ((long double) x[lcmiv] - x[gcmi1]) * (gcmix - gcmi1)/(x[gcmix] - x[gcmi1]);
	      ++iv;
	      if (dx >= d) {
		  d = dx;
		  ig = ix + 1;
		  ih = iv - 1;
		  if(*debug >= 2) Rprintf(" L(%d,%d)", ig,ih);
	      }
	  }
	  else {
	      /* If the next point of either the GCM or LCM is from the GCM,
	       * calculate the distance here. */
	      int lcmiv1 = lcm[iv - 1];
/* Fix by Yong Lu {symmetric to above!}; original Fortran: only ")" misplaced! :*/
	      dx = ((long double)x[gcmix] - x[lcmiv1]) * (lcmiv - lcmiv1) /
		  (x[lcmiv] - x[lcmiv1])- (gcmix - lcmiv1 - 1);
	      --ix;
	      if (dx >= d) {
		  d = dx;
		  ig = ix + 1;
		  ih = iv;
		  if(*debug >= 2) Rprintf(" G(%d,%d)", ig,ih);
	      }
	  }
	  if (ix < 1)	ix = 1;
	  if (iv > l_lcm)	iv = l_lcm;
	  if(*debug) {
	      if(*debug >= 2) Rprintf(" --> ix = %d, iv = %d\n", ix,iv);
	      else Rprintf(".");
	  }
      } while (gcm[ix] != lcm[iv]);
      if(*debug && *debug < 2) Rprintf("\n");
    }
    else { /* l_gcm or l_lcm == 2 */
	d = (*min_is_0) ? 0. : 1.;
	if(*debug)
	    Rprintf("  ** (l_lcm,l_gcm) = (%d,%d) ==> d := %g\n", l_lcm, l_gcm, (double)d);
    }

    if (d < *dip)	goto L_END;

/*     Calculate the DIPs for the current LOW and HIGH. */
    if(*debug) Rprintf("  calculating dip ..");

    int j_best, j_l = -1, j_u = -1;

    /* The DIP for the convex minorant. */
    dip_l = 0.;
    for (j = ig; j < l_gcm; ++j) {
	double max_t = 1.;
	int j_ = -1, jb = gcm[j + 1], je = gcm[j];
	if (je - jb > 1 && x[je] != x[jb]) {
	  double C = (je - jb) / (x[je] - x[jb]);
	  for (int jj = jb; jj <= je; ++jj) {
	    double t = (jj - jb + 1) - (x[jj] - x[jb]) * C;
	    if (max_t < t) {
		max_t = t; j_ = jj;
	    }
	  }
	}
	if (dip_l < max_t) {
	    dip_l = max_t; j_l = j_;
	}
    }

    /* The DIP for the concave majorant. */
    dip_u = 0.;
    for (j = ih; j < l_lcm; ++j) {
	double max_t = 1.;
	int j_ = -1, jb = lcm[j], je = lcm[j + 1];
	if (je - jb > 1 && x[je] != x[jb]) {
	  double C = (je - jb) / (x[je] - x[jb]);
	  for (int jj = jb; jj <= je; ++jj) {
	    double t = (x[jj] - x[jb]) * C - (jj - jb - 1);
	    if (max_t < t) {
		max_t = t; j_ = jj;
	    }
	  }
	}
	if (dip_u < max_t) {
	    dip_u = max_t; j_u = j_;
	}
    }

    if(*debug) Rprintf(" (dip_l, dip_u) = (%g, %g)", dip_l, dip_u);

    /* Determine the current maximum. */
    if(dip_u > dip_l) {
	dipnew = dip_u; j_best = j_u;
    } else {
	dipnew = dip_l; j_best = j_l;
    }
    if (*dip < dipnew) {
	*dip = dipnew;
	if(*debug)
	    Rprintf(" -> new larger dip %g (j_best = %d)\n", dipnew, j_best);
    }
    else if(*debug) Rprintf("\n");

    /*--- The following if-clause is NECESSARY  (may loop infinitely otherwise)!
      --- Martin Maechler, Statistics, ETH Zurich, July 30 1994 ---------- */
    if (low == gcm[ig] && high == lcm[ih]) {
      if(*debug)
	Rprintf("No improvement in  low = %d  nor  high = %d --> END\n",
		low, high);
    } else {
	low  = gcm[ig];
	high = lcm[ih];	   goto LOOP_Start; /* Recycle */
    }
/*---------------------------------------------------------------------------*/

L_END:
    /* do this in the caller :
     *   *xl = x[low];  *xu = x[high];
     * rather return the (low, high) indices -- automagically via lo_hi[]  */
    *dip /= (2*n);
    return;
} /* diptst */
#undef low
#undef high

//----------------- Registration <==> ../NAMESPACE
#define CDEF(name)  {#name, (DL_FUNC) &name, sizeof(name ## _t)/sizeof(name ## _t[0]), name ##_t}

#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}


// void diptst(double *x, int *n_,
// 	    double *dip, int *lo_hi, int *ifault,
// 	    int *gcm, int *lcm, int *mn, int *mj,
// 	    int *min_is_0, int *debug)
static R_NativePrimitiveArgType diptst_t[] = {
    REALSXP, INTSXP, /* dip: */ REALSXP, INTSXP, INTSXP,
    /* gcm: */ INTSXP, INTSXP,  INTSXP, INTSXP,
    /* min_is_0:*/ LGLSXP, INTSXP
};


static const R_CMethodDef CEntries[]  = {
    CDEF(diptst),
    {NULL, NULL, 0}
};

/* static R_CallMethodDef CallEntries[] = { */
/*     CALLDEF(sinc_c, 1), */

/*     {NULL, NULL, 0} */
/* }; */

/**
 * register routines
 * @param dll pointer
 * @return none
 * @author Martin Maechler
 */
void
#ifdef HAVE_VISIBILITY_ATTRIBUTE
__attribute__ ((visibility ("default")))
#endif
R_init_diptest(DllInfo *dll)
{
    R_registerRoutines(dll, CEntries, NULL, NULL, NULL);
    /* R_registerRoutines(dll, CEntries, CallEntries, NULL, NULL); */
    R_useDynamicSymbols(dll, FALSE);
}
