#include "shMem.h"
#include "drawSegs.h"

int windowWidth  = 800;
int windowHeight = 600;

std::shared_ptr<Segs> segs;
std::shared_ptr<Cube> cube;
std::shared_ptr<Axes> axes;
std::shared_ptr<Sphere> sphere;
std::shared_ptr<Cylinder> cylinder;

void initSharedObjects() {
    segs = std::shared_ptr<Segs>(new Segs());
    cube = std::shared_ptr<Cube>(new Cube());
    axes = std::shared_ptr<Axes>(new Axes());
    sphere = std::shared_ptr<Sphere>(new Sphere(4));
    cylinder = std::shared_ptr<Cylinder>(new Cylinder(200));
}

