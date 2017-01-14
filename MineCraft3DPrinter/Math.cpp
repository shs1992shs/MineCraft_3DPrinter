
/***********************************************************************

				������Implementation of Math Functions

***********************************************************************/

#include "Printer.h"

using namespace Math;

float Math::Vec2_Dot(const VECTOR2& vec1, const VECTOR2& vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y;
};

float Math::Vec2_Length(const VECTOR2& vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

float Math::Vec3_Dot(const VECTOR3& vec1, const VECTOR3& vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
};

VECTOR3 Math::Vec3_Cross(const VECTOR3& vec1, const VECTOR3& vec2)
{
	return VECTOR3(
		vec1.y*vec2.z - vec1.z*vec2.x,
		vec1.z*vec2.x - vec1.x*vec2.z,
		vec1.x*vec2.y - vec1.y*vec2.x);
};

float Math::Vec3_Length(const VECTOR3& vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

VECTOR3 Math::Vec3_Reflect(const VECTOR3 & vec, const VECTOR3 & axis)
{
	VECTOR3 In = vec;
	VECTOR3 Out = (-1)*In + 2*(In - Vec3_Dot(In, axis) / In.Length()*axis);
	return Out;
}

float Math::Vec4_Dot(const VECTOR4& vec1, const VECTOR4& vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z + vec1.w*vec2.w;
};

float Math::Vec4_Length(const VECTOR4& vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w);
}

/*
Y	|    / Z
	|  /
	|/________ X
*/


inline MATRIX4x4 Math::Matrix_Multiply(const MATRIX4x4& leftMat, const MATRIX4x4& rightMat)
{
	MATRIX4x4 outMat;
	for (UINT i = 0;i < 4;++i)
	{
		for (UINT j = 0;j < 4;++j)
		{
			VECTOR4 row_i = leftMat.m[i];
			VECTOR4 column_j = { rightMat.m[0][j],rightMat.m[1][j],rightMat.m[2][j],rightMat.m[3][j] };
			outMat.m[i][j] = Vec4_Dot(row_i, column_j);
		}
	}
	return outMat;
};

inline VECTOR4	Math::Matrix_Multiply(const MATRIX4x4 leftMat, const VECTOR4& rightColumnVec)
{
	VECTOR4 outVec;
	outVec.x = leftMat.m[0][0] * rightColumnVec.x + leftMat.m[0][1] * rightColumnVec.y +
		+leftMat.m[0][2] * rightColumnVec.z + leftMat.m[0][3] * rightColumnVec.w;
	outVec.y = leftMat.m[1][0] * rightColumnVec.x + leftMat.m[1][1] * rightColumnVec.y +
		+leftMat.m[1][2] * rightColumnVec.z + leftMat.m[1][3] * rightColumnVec.w;
	outVec.z = leftMat.m[2][0] * rightColumnVec.x + leftMat.m[2][1] * rightColumnVec.y +
		+leftMat.m[2][2] * rightColumnVec.z + leftMat.m[2][3] * rightColumnVec.w;
	outVec.w = leftMat.m[3][0] * rightColumnVec.x + leftMat.m[3][1] * rightColumnVec.y +
		+leftMat.m[3][2] * rightColumnVec.z + leftMat.m[3][3] * rightColumnVec.w;
	return outVec;
};


MATRIX4x4	Math::Matrix_Translation(float dx, float dy, float dz)
{
	MATRIX4x4 outMatrix;
	outMatrix.SetMatrix(
	{
		{ 1,0,0,dx },
		{ 0,1,0,dy },
		{ 0,0,1,dz },
		{ 0,0,0,1 }
	});
	return outMatrix;
}

MATRIX4x4	Math::Matrix_RotationX(float angle)
{
	//prepared for column vector...
	MATRIX4x4 outMatrix;
	outMatrix.SetMatrix({
		{ 1,			0,					0,					0 },
		{ 0,			cos(angle),	-sin(angle),	0 },
		{ 0,			sin(angle),	cos(angle),	0 },
		{ 0,			0,					0,					1 }
	});
	return outMatrix;
};

MATRIX4x4	Math::Matrix_RotationY(float angle)
{
	//prepared for column vector...
	MATRIX4x4 outMatrix;
	outMatrix.SetRow(0, { cos(angle),	0,			sin(angle),	0 });
	outMatrix.SetRow(1, { 0,				1,			0,					0 });
	outMatrix.SetRow(2, { -sin(angle),0,			cos(angle),	0 });
	outMatrix.SetRow(3, { 0,				0,			0,					1 });
	return outMatrix;
};

MATRIX4x4	Math::Matrix_RotationZ(float angle)
{
	//prepared for column vector...
	MATRIX4x4 outMatrix;
	outMatrix.SetRow(0, { cos(angle),	-sin(angle),	0,		0 });
	outMatrix.SetRow(1, { sin(angle),	cos(angle),	0,		0 });
	outMatrix.SetRow(2, { 0,					0,				1,		0 });
	outMatrix.SetRow(3, { 0,					0,				0,		1 });
	return outMatrix;
};

MATRIX4x4	Math::Matrix_YawPitchRoll(float yaw_Y, float pitch_X, float roll_Z)
{
	//rotation of Cardan Angle. If use column vector, then the
	//matrix will be generated by  [Ry] x [Rx] x[Rz] 
	MATRIX4x4 matRotateX;
	MATRIX4x4 matRotateY;
	MATRIX4x4 matRotateZ;

	matRotateX = Matrix_RotationX(pitch_X);
	matRotateY = Matrix_RotationY(yaw_Y);
	matRotateZ = Matrix_RotationZ(roll_Z);

	//outMatrix = [M_RY] x [M_RX] x [M_RZ]  
	//(a column vector can be pre-Multiplied by this matrix)
	MATRIX4x4 outMatrix;
	outMatrix = Matrix_Multiply(matRotateY, Matrix_Multiply(matRotateX, matRotateZ));
	return outMatrix;
}

MATRIX4x4	Math::Matrix_PerspectiveProjection(float ViewAngleY, float aspectRatio, float NearPlaneZ, float FarPlaneZ)
{
	//In D3D,Perspective Matrix is prepared for column vector.And the Z coord is not simply
	//normalized linearly, it even use the w (homogeneous) coordinate to help map
	//View Space Z into a Non-Linear (inverse-proportion) Space. It takes a multiplication
	//of perspective Matrix and a division by W coord;
	//But !!!!!!!!! I don't know why!!!
	//Is it easier to apply perspective-correction interpolation???????????????????

	//for more specific information about the derivation of this matrix,
	//refer to <<intro to 3d game programming with DX11>> Chapter5

	//the implementation annotated below ,is the implement of D3DX

	/*float term11 = 1.0f / (aspectRatio*tanf(ViewAngleY / 2.0f));
	float term22 = 1.0f / tanf(ViewAngleY / 2.0f));
	float term33 = FarPlaneZ / (FarPlaneZ - NearPlaneZ);
	float term34 = -NearPlaneZ*FarPlaneZ / (FarPlaneZ - NearPlaneZ);

	MATRIX4x4 outMatrix;
	outMatrix.SetRow(0,	{ term11,	0,				0,				0 });
	outMatrix.SetRow(1,	{ 0,			term22,		0,				0 });
	outMatrix.SetRow(2,	{ 0,			0,				term33,		term34 });
	outMatrix.SetRow(3,	{ 0,			0,				1,				0 });*/



	//But now, I am to implement my own Soft Renderer, I can try
	//to map the Z into LINEAR Space, and see how it'll go.
	float term11 = 1.0f / (aspectRatio*tanf(ViewAngleY / 2.0f));
	float term22 = 1.0f / tanf(ViewAngleY / 2.0f);
	float term33 = 1.0f / (FarPlaneZ - NearPlaneZ);
	float term34 = -NearPlaneZ / (FarPlaneZ - NearPlaneZ);

	MATRIX4x4 outMatrix;
	outMatrix.SetRow(0, { term11,	0,				0,				0 });
	outMatrix.SetRow(1, { 0,			term22,		0,				0 });
	outMatrix.SetRow(2, { 0,			0,				term33,		term34 });
	outMatrix.SetRow(3, { 0,			0,				1,				0 });

	return outMatrix;
};

inline MATRIX4x4 Math::Matrix_Transpose(const MATRIX4x4& inMatrix)
{
	MATRIX4x4 outMatrix = inMatrix;
	std::swap(outMatrix.m[0][1], outMatrix.m[1][0]);
	std::swap(outMatrix.m[0][2], outMatrix.m[2][0]);
	std::swap(outMatrix.m[0][3], outMatrix.m[3][0]);
	std::swap(outMatrix.m[2][1], outMatrix.m[1][2]);
	std::swap(outMatrix.m[3][1], outMatrix.m[1][3]);
	std::swap(outMatrix.m[3][2], outMatrix.m[2][3]);
	return outMatrix;
}

UINT Math::Clamp(UINT val, UINT min, UINT max)
{
	if (val > max)val = max;
	if (val < min)val = min;
	return val;
}

inline float Math::Clamp(float val, float min, float max)
{
	if (val > max)val = max;
	if (val < min)val = min;
	return val;
}

inline VECTOR2 Math::Clamp(const VECTOR2 & val, const VECTOR2 & min, const VECTOR2 & max)
{
	return VECTOR2(Clamp(val.x,min.x,max.x),Clamp(val.y,min.y,max.y));
}

inline VECTOR3 Math::Clamp(const VECTOR3 & val, const VECTOR3 & min, const VECTOR3 & max)
{
	return VECTOR3(Clamp(val.x, min.x, max.x), Clamp(val.y, min.y, max.y),Clamp(val.z,min.z,max.z));
}

inline BYTE Math::Lerp(BYTE start, BYTE end, float t)
{
	return char(start*(1.0f - t) + end*t);
}

inline float Math::Lerp(float start, float end, float t)
{
	return start*(1.0f-t)+end*t;
}

inline VECTOR2 Math::Lerp(const VECTOR2& start, const VECTOR2& end, float t)
{
	return VECTOR2(Lerp(start.x,end.x,t),Lerp(start.y,end.y,t));
}

inline VECTOR3 Math::Lerp(const VECTOR3& start, const VECTOR3& end, float t)
{
	return VECTOR3(Lerp(start.x, end.x, t), Lerp(start.y, end.y, t),Lerp(start.z,end.z,t));
}
