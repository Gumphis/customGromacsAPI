/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsCoordinate.h
 * Author: kulkem
 *
 * Created on July 31, 2017, 1:02 PM
 */

#pragma once

#include "gromacs/math/vectypes.h"
#include "gromacs/utility/basedefinitions.h"

namespace gmx
{

class tsVector 
{
public:
    union
    {
        struct {real x, y, z;};
        real v[3];
        rvec r;
    };
    
    tsVector();
    tsVector(const tsVector& orig);
    tsVector(real _x, real _y, real _z);
    tsVector(rvec _r);
    ~tsVector();
    
    operator rvec& () const;
    operator real* () const;
    real operator () (const int i) const;
    real& operator () (const int i);

    tsVector& operator  = (const tsVector& v);
    tsVector& operator += (const tsVector& v);
    tsVector& operator -= (const tsVector& v);
    tsVector& operator *= (const tsVector& v);
    tsVector& operator *= (const real f);
    tsVector& operator /= (const tsVector& v);
    tsVector& operator /= (const real f);
};

tsVector operator + (const tsVector& v1, const tsVector& v2);
tsVector operator - (const tsVector& v1, const tsVector& v2);
tsVector operator * (const tsVector& v1, const tsVector& v2);
tsVector operator * (const tsVector& v1, const real f);
tsVector operator * (const real f, const tsVector& v1);
tsVector operator / (const tsVector& v1, const tsVector& v2);
tsVector operator / (const tsVector& v1, const real f);
tsVector operator - (const tsVector& v1);                      

gmx_bool operator == (const tsVector& v1, const tsVector& v2);
gmx_bool operator != (const tsVector& v1, const tsVector& v2);

real		tsVectorLength(const tsVector& v);						
real		tsVectorLengthSq(const tsVector& v);						
tsVector	tsVectorNormalize(const tsVector& v);					
tsVector	tsVectorNormalizeEx(const tsVector& v);					
tsVector	tsVectorCross(const tsVector& v1, const tsVector& v2);			
real		tsVectorDot(const tsVector& v1, const tsVector& v2);			
real		tsVectorAngle(const tsVector& v1, const tsVector& v2);			
tsVector	tsVectorInterpolateCoords(const tsVector& v1, const tsVector& v2, const real p);
tsVector	tsVectorInterpolateNormal(const tsVector& v1, const tsVector& v2, const real p);

// ******************************************************************
// Die Matrixklasse
class tsBox
{
public:
    union
    {
            struct
            {
                real m11, m12, m13,
                      m21, m22, m23,
                      m31, m32, m33;
            };
            real		m[3][3];
            real		n[9];
            matrix          b;
    };

    tsBox();
    tsBox(const tsBox& m);
    tsBox(real _m11, real _m12, real _m13,
             real _m21, real _m22, real _m23,
             real _m31, real _m32, real _m33);
    tsBox(real a);

    operator real* () const;
    operator matrix& () const;
    real& operator () (int iRow, int iColumn);
    real operator () (int iRow, int iColumn) const;

    tsBox& operator = (const tsBox& m);
    tsBox& operator = (const matrix& _m);
    tsBox& operator += (const tsBox& m);
    tsBox& operator -= (const tsBox& m);
    tsBox& operator *= (const tsBox& m);
    tsBox& operator *= (const real f);
    tsBox& operator /= (const real f);
};

tsBox operator + (const tsBox& a, const tsBox& b);
tsBox operator - (const tsBox& a, const tsBox& b);
tsBox operator - (const tsBox& m);
tsBox operator * (const tsBox& a, const tsBox& b);
tsBox operator * (const tsBox& m, const real f);
tsBox operator * (const real f, const tsBox& m);
tsBox operator / (const tsBox& m, const real f);

gmx_bool operator == (const tsBox& a, const tsBox& b);
gmx_bool operator != (const tsBox& a, const tsBox& b);

} // gmx namespace