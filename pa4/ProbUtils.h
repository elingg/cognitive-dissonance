#ifndef _PROBUTILS_H
#define _PROBUTILS_H

/* =================================================================== */
/*                          Probability Utils                          */
/* =================================================================== */

/* GetCND */
/* ------ */
/* This gets us the cumulative normal distribution using the erf and
 * erfc functions.
 */
inline double GetCND(double x, double mean, double stddev)
{
  double z = (x - mean) / stddev;
  if (z >= 0.0)
    return 0.5 + 0.5 * erf(z / sqrt(2.0));
  else
    return 0.5 * erfc(-z / sqrt(2.0));
}

/* GetProbND */
/* --------- */
/* Gets us the value of the normal distribution at x.
 */
inline double GetProbND(double x, double mean, double stddev)
{
  double denom = sqrt(2.0 * M_PI) * stddev * exp((x-mean)*(x-mean) / (2.0 * stddev*stddev));
  return 1.0 / denom;
}

#endif
