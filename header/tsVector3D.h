#pragma once

// ******************************************************************

class tsVector3D
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float u;
			float v;
			float w;
		};
		float c[3];
	};

	tsVector3D() {}
	tsVector3D(float _x, float _y, float _z)	{x = _x;y = _y;z = _z;}
	tsVector3D(float a)				{x=a;y=a;z=a;}
//	tsVector3D( tsVector3D& v)		{x=v.x;y=v.y;z=v.z;}
	tsVector3D( const tsVector3D& v) {x=v.x;y=v.y;z=v.z;}
#ifdef CTSGMX
        tsVector3D( rvec v) {x=v[0];y=v[1];z=v[2];}
#endif
	~tsVector3D() {}

	// ******************************************************************

	operator float* ()	{return (float*)(c);}

	float operator () (const int i) const {if(i==1) return x; else if(i==2) return y; else return z;}
	float& operator () (const int i) {if(i==1) return x; else if(i==2) return y; else return z;}


	// ******************************************************************

	tsVector3D& operator = (const tsVector3D& v)	{x=v.x; y=v.y;z=v.z; return *this;}
	tsVector3D& operator += (const tsVector3D& v)	{x+=v.x; y+=v.y;z+=v.z; return *this;}
	tsVector3D& operator -= (const tsVector3D& v)	{x-=v.x; y-=v.y;z-=v.z; return *this;}
	tsVector3D& operator *= (const tsVector3D& v)	{x*=v.x; y*=v.y;z*=v.z; return *this;}
	tsVector3D& operator *= (const float f)			{x*=f; y*=f;z*=f; return *this;}
	tsVector3D& operator /= (const tsVector3D& v)	{x/=v.x; y/=v.y;z/=v.z; return *this;}
	tsVector3D& operator /= (const float f)			{x/=f; y/=f;z/=f; return *this;}
};

// ******************************************************************

inline tsVector3D operator + (const tsVector3D& v1, const tsVector3D& v2) {return tsVector3D(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}
inline tsVector3D operator - (const tsVector3D& v1, const tsVector3D& v2) {return tsVector3D(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}
inline tsVector3D operator * (const tsVector3D& v1, const tsVector3D& v2) {return tsVector3D(v1.x*v2.x,v1.y*v2.y,v1.z*v2.z);}
inline tsVector3D operator * (const tsVector3D& v1, const float f) {return tsVector3D(v1.x*f,v1.y*f,v1.z*f);}
inline tsVector3D operator / (const tsVector3D& v1, const tsVector3D& v2) {return tsVector3D(v1.x/v2.x,v1.y/v2.y,v1.z/v2.z);}
inline tsVector3D operator / (const tsVector3D& v1, const float f) {return tsVector3D(v1.x/f,v1.y/f,v1.z/f);}
inline tsVector3D operator - (const tsVector3D& v1) {return tsVector3D(-v1.x,-v1.y,-v1.z);}

// ******************************************************************

inline bool operator == (const tsVector3D& v1, const tsVector3D& v2) {if(v1.x!=v2.x) return false; if(v1.y!=v2.y) return false; return v1.z==v2.z;}
inline bool operator != (const tsVector3D& v1, const tsVector3D& v2) {if(v1.x!=v2.x) return true; if(v1.y!=v2.y) return true; return v1.z!=v2.z;}

// ******************************************************************

inline float		tsVector3DLength(const tsVector3D& v)													{return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);}
inline float		tsVector3DLengthSq(const tsVector3D& v)												{return v.x * v.x + v.y * v.y + v.z * v.z;}
inline tsVector3D	tsVector3DNormalize(const tsVector3D& v)												{return v / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);}
inline tsVector3D	tsVector3DNormalizeEx(const tsVector3D& v)											{return v / (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z) + 0.0001f);}
inline tsVector3D	tsVector3DCross(const tsVector3D& v1, const tsVector3D& v2)							{return tsVector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
inline float		tsVector3DDot(const tsVector3D& v1, const tsVector3D& v2)								{return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
inline float		tsVector3DAngle(const tsVector3D& v1, const tsVector3D& v2)							{return acosf((v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) / sqrtf((v1.x * v1.x + v1.y * v1.y + v1.z * v1.z) * (v2.x * v2.x + v2.y * v2.y + v2.z * v2.z)));}
inline tsVector3D	tsVector3DInterpolateCoords(const tsVector3D& v1, const tsVector3D& v2, const float p)	{return v1 + p * (v2 - v1);}
inline tsVector3D	tsVector3DInterpolateNormal(const tsVector3D& v1, const tsVector3D& v2, const float p)	{return tsVector3DNormalizeEx(v1 + p * (v2 - v1));}
//inline tsVector3D	tsVector3DMin(const tsVector3D& v1, const tsVector3D& v2)								{return tsVector3D(TS_MIN(v1.x, v2.x), TS_MIN(v1.y, v2.y), TS_MIN(v1.z, v2.z));}
//inline tsVector3D	tsVector3DMax(const tsVector3D& v1, const tsVector3D& v2)								{return tsVector3D(TS_MAX(v1.x, v2.x), TS_MAX(v1.y, v2.y), TS_MAX(v1.z, v2.z));}
//inline tsVector3D	tsVector3DRandom()												{return tbVector2NormalizeEx(tbVector2(tbFloatRandom(-1.0f, 1.0f), tbFloatRandom(-1.0f, 1.0f)));}

