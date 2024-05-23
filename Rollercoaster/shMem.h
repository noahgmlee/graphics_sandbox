#include "sphere.h"
#include "cylinder.h"
#include "axes.h"
#include "drawSegs.h"
#include "cube.h"
#include "cubeMap.h" // to use cubemap class

extern int windowWidth;
extern int windowHeight;

// extern CubeMap  *cubemap; // cubemap
extern std::shared_ptr<Cube> cube; // cube
extern std::shared_ptr<Segs> segs;
extern std::shared_ptr<Axes> axes;
extern std::shared_ptr<Sphere> sphere;
extern std::shared_ptr<Cylinder> cylinder;

void initSharedObjects();
