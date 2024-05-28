// train.cpp

#include "train.h"
#include "shMem.h"


#define SPHERE_RADIUS 5.0
#define SPHERE_COLOUR 238/255.0, 106/255.0, 20/255.0
#define CAR_COLOUR 2/255.0, 12/255.0, 63/255.0 // train colour
#define CAR_DIMENSIONS vec3( 3.75, 5.0, 10.0 ) // for scaling cube
#define GRAVITY vec3( 0, 0, -9.8 ) // for physics implementation

// Draw the train.
//
// 'flag' is toggled by pressing 'F' and can be used for debugging


void Train::draw( mat4 &WCStoVCS, mat4 &WCStoCCS, vec3 lightDir, bool flag )

{
#if 1

  // YOUR CODE HERE

	float t = spline->paramAtArcLength(pos);

	mat4 M = spline->findLocalTransform(t) * translate(0.0, 2.0, 0.0) * scale(CAR_DIMENSIONS.x, CAR_DIMENSIONS.y, CAR_DIMENSIONS.z); // translate to make train look like its on top of the tracks
	mat4 MV = WCStoVCS * M;
	mat4 MVP = WCStoCCS * M;

	cube->draw(MV, MVP, lightDir, vec3(CAR_COLOUR));

#else

  float t = spline->paramAtArcLength( pos );

  // Draw sphere

  vec3 o, x, y, z;
  spline->findLocalSystem( t, o, x, y, z );

  mat4 M   = translate( o ) * scale( SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS );
  mat4 MV  = WCStoVCS * M;
  mat4 MVP = WCStoCCS * M;

  sphere->draw( MV, MVP, lightDir, vec3( SPHERE_COLOUR ) );

#endif
}


void Train::advance( float elapsedSeconds )

{
#if 1

  // YOUR CODE HERE
	float t = spline->paramAtArcLength(pos);

	vec3 o, x, y, z;
	spline->findLocalSystem(t, o, x, y, z);
    #if 1 // normal gravity of -9.8m/s^2
		velocity = speed * z + elapsedSeconds * (5 * GRAVITY * z) * z;

    #else // increased gravity to better visualize effect of physics on train (doubled)
		velocity = speed * z + elapsedSeconds * (2 * GRAVITY);
    #endif

	speed = velocity.length();
	if (speed < 30.0) // minimum speed so train doesn't get stuck
		speed = 30.0;
	pos = pos + elapsedSeconds * speed;

	if (pos >= spline->totalArcLength()) // so train does not stop after one loop of track
		pos = 0;
#else

  pos += 1;

#endif
}
