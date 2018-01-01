/* ------------------------------------------------------

 View Frustum - Lighthouse3D

  -----------------------------------------------------*/

#include "FrustumG.h"
#include <math.h>
#include <SDL_log.h>

#define ANG2RAD 3.14159265358979323846/180.0



FrustumG::FrustumG() {}

FrustumG::~FrustumG() {}

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD) {

	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	tang = (float)tan(angle* ANG2RAD * 0.5) ;
	nh = nearD * tang;
	nw = nh * ratio; 
	fh = farD  * tang;
	fw = fh * ratio;


}


void FrustumG::setCamDef(Vec3 &p, Vec3 &l, Vec3 &u) {

	Vec3 dir,nc,fc,X,Y,Z;

	Z = p - l;
	Z.normalize();

	X = u * Z;
	X.normalize();

	Y = Z * X;

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

    /*
	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
     */
    auto minusz = -Z;
    pl[NEARP].setNormalAndPoint(minusz,nc);
    pl[FARP].setNormalAndPoint(Z,fc);

    Vec3 aux,normal;

    aux = (nc + Y*nh) - p;
    aux.normalize();
    normal = aux * X;
    auto tmp = nc+Y*nh;
    pl[TOP].setNormalAndPoint(normal,tmp);

    aux = (nc - Y*nh) - p;
    aux.normalize();
    normal = X * aux;
    tmp = nc-Y*nh;
    pl[BOTTOM].setNormalAndPoint(normal,tmp);

    aux = (nc - X*nw) - p;
    aux.normalize();
    normal = aux * Y;
    tmp = nc-X*nw;
    pl[LEFT].setNormalAndPoint(normal,tmp);

    aux = (nc + X*nw) - p;
    aux.normalize();
    normal = Y * aux;
    tmp = nc+X*nw;
    pl[RIGHT].setNormalAndPoint(normal,tmp);
}


int FrustumG::pointInFrustum(Vec3 &p) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {
        auto dist = pl[i].distance(p);
        //SDL_Log("Pos %.2f,%.2f,%.2f distance %.2f to plane %d", p.x, p.y, p.z, dist, i);
		if (pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return(result);

}


int FrustumG::sphereInFrustum(Vec3 &p, float raio) {

	int result = INSIDE;
	float distance;

	for(int i=0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -raio)
			return OUTSIDE;
		else if (distance < raio)
			result =  INTERSECT;
	}
	return(result);

}


int FrustumG::boxInFrustum(AABox &b) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

        auto vp = b.getVertexP(pl[i].normal);
        auto vn = b.getVertexN(pl[i].normal);
		if (pl[i].distance(vp) < 0)
			return OUTSIDE;
		else if (pl[i].distance(vn) < 0)
			result =  INTERSECT;
	}
	return(result);

 }
