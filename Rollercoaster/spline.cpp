/* spline.cpp
 */


#include "spline.h"
#include "shMem.h"


#define DIVS_PER_SEG 20         // number of samples of on each spline segment (for arc length parameterization)

float Spline::M[][4][4] = {

  { { 0, 0, 0, 0},              // Linear
    { 0, 0, 0, 0},
    { 0,-1, 1, 0},
    { 0, 1, 0, 0}, },

  { { -0.5,  1.5, -1.5,  0.5 }, // Catmull-Rom
    {  1.0, -2.5,  2.0, -0.5 },
    { -0.5,  0.0,  0.5,  0.0 },
    {  0.0,  1.0,  0.0,  0.0 } },

  { { -0.1667, 0.5,   -0.5,    0.1667 }, // B-spline
    {  0.5,   -1.0,    0.5,    0.0    },
    { -0.5,    0.0,    0.5,    0.0    },
    {  0.1667, 0.6667, 0.1667, 0.0    } }

};


const char * Spline::MName[] = {
  "linear",
  "Catmull-Rom",
  "B-spline",
  ""
};


// Evaluate the spline at parameter 't'.  Return the value, tangent
// (i.e. first derivative), or binormal, depending on the
// 'returnDerivative' parameter.
//
// The spline is continuous, so the first data point appears again
// after the last data point.  t=0 at the first data point and t=n-1
// at the n^th data point.  For t outside this range, use 't modulo n'.
//
// Use the change-of-basis matrix in M[currSpline].


vec3 Spline::eval( float t, evalType type )

{
  // YOUR CODE HERE

  // for t outside [0,data.size()), move t into range
    if (t >= data.size())
        t = t - data.size();
    if (t < 0)
        t = t + data.size();
    float u = t - floor(t);

  // Find the 4 control points for this t, and the u value in [0,1] for this interval.
    vec3 qim1;
    if (floor(t) == 0)
        qim1 = data[data.size() - 1];
    else
        qim1 = data[floor(t) - 1];
    vec3 qi = data[floor(t)];
    vec3 qip1 = data[(int(floor(t)) + 1) % data.size()];
    vec3 qip2 = data[(int(floor(t)) + 2) % data.size()];

  // Compute Mv
    mat4 v;
    v.rows[0] = vec4(qim1, 0);
    v.rows[1] = vec4(qi, 0);
    v.rows[2] = vec4(qip1, 0);
    v.rows[3] = vec4(qip2, 0);

    mat4 M2;
    M2.rows[0] = vec4(M[currSpline][0][0], M[currSpline][0][1], M[currSpline][0][2], M[currSpline][0][3]);
    M2.rows[1] = vec4(M[currSpline][1][0], M[currSpline][1][1], M[currSpline][1][2], M[currSpline][1][3]);
    M2.rows[2] = vec4(M[currSpline][2][0], M[currSpline][2][1], M[currSpline][2][2], M[currSpline][2][3]);
    M2.rows[3] = vec4(M[currSpline][3][0], M[currSpline][3][1], M[currSpline][3][2], M[currSpline][3][3]);

    mat4 Mv = M2 * v;
    vec4 T = vec4(pow(u, 3), pow(u, 2), u, 1);
    vec4 Tprime = vec4(3 * pow(u, 2), 2 * u, 1, 0);
    vec3 result;

  // If type == VALUE, return the value = T (Mv)
    if (type == VALUE) {
        result.x = T.x * Mv[0][0] + T.y * Mv[1][0] + T.z * Mv[2][0] + T.w * Mv[3][0];
        result.y = T.x * Mv[0][1] + T.y * Mv[1][1] + T.z * Mv[2][1] + T.w * Mv[3][1];
        result.z = T.x * Mv[0][2] + T.y * Mv[1][2] + T.z * Mv[2][2] + T.w * Mv[3][2];
    }

  // If type == TANGENT, return the value T' (Mv)
    if (type == TANGENT) {
        result.x = Tprime.x * Mv[0][0] + Tprime.y * Mv[1][0] + Tprime.z * Mv[2][0] + Tprime.w * Mv[3][0];
        result.y = Tprime.x * Mv[0][1] + Tprime.y * Mv[1][1] + Tprime.z * Mv[2][1] + Tprime.w * Mv[3][1];
        result.z = Tprime.x * Mv[0][2] + Tprime.y * Mv[1][2] + Tprime.z * Mv[2][2] + Tprime.w * Mv[3][2];
    }

  //return vec3(result.x, result.y, result.z);
    return result;
}


// Find a local coordinate system at t.  Return the axes x,y,z.  y
// should point as much up as possible and z should point in the
// direction of increasing position on the curve.


void Spline::findLocalSystem(float t, vec3& o, vec3& x, vec3& y, vec3& z)

{
#if 1

    // YOUR CODE HERE
    o = value(t);
    z = tangent(t).normalize();
    float yz = -(z.x * z.x + z.y * z.y) / z.z;
    if (yz < 0)
        y = vec3(-z.x, -z.y, -yz).normalize();
    else
        y = vec3(z.x, z.y, yz).normalize();
    x = (z ^ y).normalize();


#else

  o = vec3(0,0,0);
  x = vec3(1,0,0);
  y = vec3(0,1,0);
  z = vec3(0,0,1);

#endif
}


mat4 Spline::findLocalTransform( float t )

{
  vec3 o, x, y, z;

  findLocalSystem( t, o, x, y, z );

  mat4 M;
  M.rows[0] = vec4( x.x, y.x, z.x, o.x );
  M.rows[1] = vec4( x.y, y.y, z.y, o.y );
  M.rows[2] = vec4( x.z, y.z, z.z, o.z );
  M.rows[3] = vec4( 0, 0, 0, 1 );

  return M;
}


// Draw a point and a local coordinate system for parameter t


void Spline::drawLocalSystem( float t, mat4 &MVP )

{
  vec3 o, x, y, z;

  findLocalSystem( t, o, x, y, z );

  mat4 M;
  M.rows[0] = vec4( x.x, y.x, z.x, o.x );
  M.rows[1] = vec4( x.y, y.y, z.y, o.y );
  M.rows[2] = vec4( x.z, y.z, z.z, o.z );
  M.rows[3] = vec4( 0, 0, 0, 1 );

  M = MVP * M * scale(6,6,6);
  axes->draw(M);
}


// Draw the spline with even parameter spacing


void Spline::draw( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawIntervals )

{
  // Draw the spline

  vec3 *points = new vec3[ data.size()*DIVS_PER_SEG + 1 ];
  vec3 *colours = new vec3[ data.size()*DIVS_PER_SEG + 1 ];

  int i = 0;
  for (float t=0; t<data.size(); t+=1/(float)DIVS_PER_SEG) {
    points[i] = value( t );
    colours[i] = SPLINE_COLOUR;
    i++;
  }

  segs->drawSegs( GL_LINE_LOOP, points, colours, i, MV, MVP, lightDir );

  // Draw points evenly spaced in the parameter

  if (drawIntervals)
    for (float t=0; t<data.size(); t+=1/(float)DIVS_PER_SEG)
      drawLocalSystem( t, MVP );

  delete[] points;
  delete[] colours;
}


// Draw the spline with even arc-length spacing


void Spline::drawWithArcLength( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawIntervals )

{
  // Draw the spline

  vec3 *points = new vec3[ data.size()*DIVS_PER_SEG + 1 ];
  vec3 *colours =  new vec3[ data.size()*DIVS_PER_SEG + 1 ];

  int i = 0;
  for (float t=0; t<data.size(); t+=1/(float)DIVS_PER_SEG) {
    points[i] = value( t );
    colours[i] = SPLINE_COLOUR;
    i++;
  }

  segs->drawSegs( GL_LINE_LOOP, points, colours, i, MV, MVP, lightDir );

  // Draw points evenly spaced in arc length

  if (drawIntervals) {

    float totalLength = totalArcLength();

    for (float s=0; s<totalLength; s+=totalLength/(float)(data.size()*DIVS_PER_SEG)) {
      float t = paramAtArcLength( s );
      drawLocalSystem( t, MVP );
    }
  }

  delete[] points;
  delete[] colours;
}


// Fill in an arcLength array such that arcLength[i] is the estimated
// arc length up to the i^th sample (using DIVS_PER_SEG samples per
// spline segment).


void Spline::computeArcLengthParameterization()

{
  if (data.size() == 0)
    return;

  if (arcLength != NULL)
    delete [] arcLength;

  arcLength = new float[ data.size() * DIVS_PER_SEG + 1 ];

  // first sample at length 0

  arcLength[0] = 0;
  vec3 prev = value(0);

  maxHeight = prev.z;

  // Compute intermediate lengths

  int k = 1;

  for (int i=0; i<data.size(); i++)
    for (int j=(i>0?0:1); j<DIVS_PER_SEG; j++) {

      vec3 next = value( i + j/(float)DIVS_PER_SEG );

      arcLength[k] = arcLength[k-1] + (next-prev).length();
      prev = next;
      k++;

      if (next.z > maxHeight)
        maxHeight = next.z;
    }

  // last sample at full length

  vec3 next = value( data.size() );
  arcLength[k] = arcLength[k-1] + (next-prev).length();

  if (next.z > maxHeight)
    maxHeight = next.z;

  mustRecomputeArcLength = false;
}


// Find the spline parameter at a particular arc length, s.


float Spline::paramAtArcLength( float s )

{
  if (mustRecomputeArcLength)
    computeArcLengthParameterization();

  // Do binary search on arc lengths to find l such that
  //
  //        arcLength[l] <= s < arcLength[l+1].

  if (s < 0)
    s += arcLength[ data.size() * DIVS_PER_SEG ];

  int l = 0;
  int r = data.size()*DIVS_PER_SEG;

  while (r-l > 1) {
    int m = (l+r)/2;
    if (arcLength[m] <= s)
      l = m;
    else
      r = m;
  }

  if (arcLength[l] > s || arcLength[l+1] <= s)
    return (l + 0.5) / (float) DIVS_PER_SEG;

  // Do linear interpolation in arcLength[l] ... arcLength[l+1] to
  // find position of s.

  float p = (s - arcLength[l]) / (arcLength[l+1] - arcLength[l]);

  // Return the curve parameter at s

  return (l+p) / (float) DIVS_PER_SEG;
}



float Spline::totalArcLength()

{
  if (data.size() == 0)
    return 0;

  if (mustRecomputeArcLength)
    computeArcLengthParameterization();

  return arcLength[ data.size() * DIVS_PER_SEG ];
}
