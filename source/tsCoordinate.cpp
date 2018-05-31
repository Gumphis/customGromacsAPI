/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsCoordinate.cpp
 * Author: kulkem
 * 
 * Created on July 31, 2017, 1:02 PM
 */

#include <math.h>
#include "tsCoordinate.h"

namespace gmx
{

tsVector::tsVector()
    :x(0), y(0), z(0)
{
}

tsVector::tsVector(const tsVector& orig)
    : x(orig.x), y(orig.y), z(orig.z)
{
}

tsVector::tsVector(real _x, real _y, real _z)
    : x(_x), y(_y), z(_z)
{
}

tsVector::tsVector(rvec _r)
    : x(_r[0]), y(_r[1]), z(_r[2])
{
}

tsVector::~tsVector() {}

tsVector::operator rvec& () const                      {return (rvec&)(r);}
tsVector::operator real* () const                      {return (real*)(v);}
real tsVector::operator () (const int i) const         {if(i==1) return x; else if(i==2) return y; else return z;}
real& tsVector::operator () (const int i)              {if(i==1) return x; else if(i==2) return y; else return z;}

tsVector& tsVector::operator  = (const tsVector& v)	{x=v.x; y=v.y;z=v.z; return *this;}
tsVector& tsVector::operator += (const tsVector& v)	{x+=v.x; y+=v.y;z+=v.z; return *this;}
tsVector& tsVector::operator -= (const tsVector& v)	{x-=v.x; y-=v.y;z-=v.z; return *this;}
tsVector& tsVector::operator *= (const tsVector& v)	{x*=v.x; y*=v.y;z*=v.z; return *this;}
tsVector& tsVector::operator *= (const real f)         {x*=f; y*=f;z*=f; return *this;}
tsVector& tsVector::operator /= (const tsVector& v)	{x/=v.x; y/=v.y;z/=v.z; return *this;}
tsVector& tsVector::operator /= (const real f)         {x/=f; y/=f;z/=f; return *this;}

tsVector operator + (const tsVector& v1, const tsVector& v2)  {return tsVector(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}
tsVector operator - (const tsVector& v1, const tsVector& v2)  {return tsVector(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}
tsVector operator * (const tsVector& v1, const tsVector& v2)  {return tsVector(v1.x*v2.x,v1.y*v2.y,v1.z*v2.z);}
tsVector operator * (const tsVector& v1, const real f)       {return tsVector(v1.x*f,v1.y*f,v1.z*f);}
tsVector operator * (const real f, const tsVector& v1)       {return tsVector(v1.x*f,v1.y*f,v1.z*f);}
tsVector operator / (const tsVector& v1, const tsVector& v2)  {return tsVector(v1.x/v2.x,v1.y/v2.y,v1.z/v2.z);}
tsVector operator / (const tsVector& v1, const real f)       {return tsVector(v1.x/f,v1.y/f,v1.z/f);}
tsVector operator - (const tsVector& v1)                      {return tsVector(-v1.x,-v1.y,-v1.z);}

gmx_bool operator == (const tsVector& v1, const tsVector& v2)       {if(v1.x!=v2.x) return false; if(v1.y!=v2.y) return false; return v1.z==v2.z;}
gmx_bool operator != (const tsVector& v1, const tsVector& v2)       {if(v1.x!=v2.x) return true; if(v1.y!=v2.y) return true; return v1.z!=v2.z;}

real		tsVectorLength(const tsVector& v)							{return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);}
real		tsVectorLengthSq(const tsVector& v)							{return v.x * v.x + v.y * v.y + v.z * v.z;}
tsVector	tsVectorNormalize(const tsVector& v)                                                    {return v / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);}
tsVector	tsVectorNormalizeEx(const tsVector& v)                                                  {return v / (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z) + 0.0001f);}
tsVector	tsVectorCross(const tsVector& v1, const tsVector& v2)                                   {return tsVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
real		tsVectorDot(const tsVector& v1, const tsVector& v2)                                     {return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
real		tsVectorAngle(const tsVector& v1, const tsVector& v2)                                   {return acosf((v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) / sqrtf((v1.x * v1.x + v1.y * v1.y + v1.z * v1.z) * (v2.x * v2.x + v2.y * v2.y + v2.z * v2.z)));}
tsVector	tsVectorInterpolateCoords(const tsVector& v1, const tsVector& v2, const real p)	{return v1 + p * (v2 - v1);}
tsVector	tsVectorInterpolateNormal(const tsVector& v1, const tsVector& v2, const real p)	{return tsVectorNormalizeEx(v1 + p * (v2 - v1));}

// ******************************************************************

tsBox::tsBox()
    : m11(0), m12(0), m13(0),
      m21(0), m22(0), m23(0),
      m31(0), m32(0), m33(0)
{
}

tsBox::tsBox(const tsBox& m) 
    : m11(m.m11), m12(m.m12), m13(m.m13),
      m21(m.m21), m22(m.m22), m23(m.m23),
      m31(m.m31), m32(m.m32), m33(m.m33)
{
}

tsBox::tsBox(real _m11, real _m12, real _m13,
                   real _m21, real _m22, real _m23,
                   real _m31, real _m32, real _m33) 
    : m11(_m11), m12(_m12), m13(_m13),
      m21(_m21), m22(_m22), m23(_m23),
      m31(_m31), m32(_m32), m33(_m33)
{
}

tsBox::tsBox(real a)
    : m11(a), m12(a), m13(a),
      m21(a), m22(a), m23(a),
      m31(a), m32(a), m33(a)
{
}

tsBox::operator real* () const			 {return (real*)(n);}
tsBox::operator matrix& () const                      {return (matrix&)(b);}
real& tsBox::operator () (int iRow, int iColumn)      {return m[iRow - 1][iColumn - 1];}
real tsBox::operator () (int iRow, int iColumn) const {return m[iRow - 1][iColumn - 1];}

tsBox& tsBox::operator = (const tsBox& m) 
{
    for(int i = 0; i < 9; ++i)
        n[i] = m.n[i];
    return *this;
}

tsBox& tsBox::operator = (const matrix& _m)
{
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            m[i][j] = _m[i][j];
    return *this;
}

tsBox& tsBox::operator += (const tsBox& m)
{
    for(int i = 0; i < 9; ++i)
        n[i] += m.n[i];
    return *this;
}

tsBox& tsBox::operator -= (const tsBox& m)
{
    for(int i = 0; i < 9; ++i)
        n[i] -= m.n[i];
    return *this;
}

tsBox& tsBox::operator *= (const tsBox& m)
{
    return *this = tsBox(m.m11 * m11 + m.m21 * m12 + m.m31 * m13,
                            m.m12 * m11 + m.m22 * m12 + m.m32 * m13,
                            m.m13 * m11 + m.m23 * m12 + m.m33 * m13,
                            m.m11 * m21 + m.m21 * m22 + m.m31 * m23,
                            m.m12 * m21 + m.m22 * m22 + m.m32 * m23,
                            m.m13 * m21 + m.m23 * m22 + m.m33 * m23,
                            m.m11 * m31 + m.m21 * m32 + m.m31 * m33,
                            m.m12 * m31 + m.m22 * m32 + m.m32 * m33,
                            m.m13 * m31 + m.m23 * m32 + m.m33 * m33);
}

tsBox& tsBox::operator *= (const real f)
{
    for(int i = 0; i < 9; ++i)
        n[i] *= f;
    return *this;
}

tsBox& tsBox::operator /= (const real f)
{
    for(int i = 0; i < 9; ++i)
        n[i] /= f;
    return *this;
}

tsBox operator + (const tsBox& a, const tsBox& b)
{
    return tsBox(a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13,
                    a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23,
                    a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33);
}

tsBox operator - (const tsBox& a, const tsBox& b)
{
    return tsBox(a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13,
                    a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, 
                    a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33);
}

tsBox operator - (const tsBox& m)
{
    return tsBox(-m.m11, -m.m12, -m.m13,
                    -m.m21, -m.m22, -m.m23, 
                    -m.m31, -m.m32, -m.m33);
}

tsBox operator * (const tsBox& a, const tsBox& b)
{
    return tsBox(b.m11 * a.m11 + b.m21 * a.m12 + b.m31 * a.m13,
                    b.m12 * a.m11 + b.m22 * a.m12 + b.m32 * a.m13,
                    b.m13 * a.m11 + b.m23 * a.m12 + b.m33 * a.m13,
                    b.m11 * a.m21 + b.m21 * a.m22 + b.m31 * a.m23,
                    b.m12 * a.m21 + b.m22 * a.m22 + b.m32 * a.m23,
                    b.m13 * a.m21 + b.m23 * a.m22 + b.m33 * a.m23,
                    b.m11 * a.m31 + b.m21 * a.m32 + b.m31 * a.m33,
                    b.m12 * a.m31 + b.m22 * a.m32 + b.m32 * a.m33,
                    b.m13 * a.m31 + b.m23 * a.m32 + b.m33 * a.m33);
}

tsBox operator * (const tsBox& m, const real f)
{
    return tsBox(m.m11 * f, m.m12 * f, m.m13 * f,
                    m.m21 * f, m.m22 * f, m.m23 * f,
                    m.m31 * f, m.m32 * f, m.m33 * f);
}

tsBox operator * (const real f, const tsBox& m)
{
    return tsBox(m.m11 * f, m.m12 * f, m.m13 * f,
                    m.m21 * f, m.m22 * f, m.m23 * f,
                    m.m31 * f, m.m32 * f, m.m33 * f);
}

tsBox operator / (const tsBox& m, const real f)
{
    return tsBox(m.m11 / f, m.m12 / f, m.m13 / f,
                    m.m21 / f, m.m22 / f, m.m23 / f,
                    m.m31 / f, m.m32 / f, m.m33 / f);
}

gmx_bool operator == (const tsBox& a, const tsBox& b)
{
	if(a.m11 != b.m11) return false;
	if(a.m12 != b.m12) return false;
	if(a.m13 != b.m13) return false;
	if(a.m21 != b.m21) return false;
	if(a.m22 != b.m22) return false;
	if(a.m23 != b.m23) return false;
	if(a.m31 != b.m31) return false;
	if(a.m32 != b.m32) return false;
	return a.m33 != b.m33;
}

gmx_bool operator != (const tsBox& a, const tsBox& b)
{
	if(a.m11 != b.m11) return true;
	if(a.m12 != b.m12) return true;
	if(a.m13 != b.m13) return true;
	if(a.m21 != b.m21) return true;
	if(a.m22 != b.m22) return true;
	if(a.m23 != b.m23) return true;
	if(a.m31 != b.m31) return true;
	if(a.m32 != b.m32) return true;
	return a.m33 != b.m33;
}

} //gmx namespace