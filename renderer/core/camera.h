/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "core\film.h"
#include "core\rng.h"
#include "geometry\montecarlo.h"

namespace ExposureRender
{

/*! Camera class */
class EXPOSURE_RENDER_DLL Camera
{
public:
	/*! Default constructor */
	HOST Camera() :
		Film(Vec2i(640, 480)),
		Pos(100.0f),
		Target(0.0f),
		Up(0.0f, 1.0f, 0.0f),
		FocusMode(Enums::AutoFocus),
		FocusUV(0.5f),
		FocalDistance(10.0f),
		ApertureShape(Enums::Polygon),
		ApertureSize(0.0f),
		NoApertureBlades(6),
		ApertureAngle(0.0f),
		FOV(35.0f),
		N(0.0f),
		U(0.0f),
		V(0.0f)
	{
	}
	
	/*! Samples the camera
		@param[in,out] R Sampled ray
		@param[in] UV Position on the film plane
		@param[in] RNG Random number generator 
	*/
	DEVICE void Sample(Ray& R, const Vec2i& UV, RNG& RNG)
	{
		Vec2f ScreenPoint;

		R.ImageUV[0] = UV[0] + RNG.Get1();
		R.ImageUV[1] = UV[1] + RNG.Get1();

		ScreenPoint[0] = this->Film.Screen[0][0] + (this->Film.InvScreen[0] * R.ImageUV[0]);
		ScreenPoint[1] = this->Film.Screen[1][0] + (this->Film.InvScreen[1] * R.ImageUV[1]);

		R.O		= this->Pos;
		R.D		= Normalize(this->N + (ScreenPoint[0] * this->U) - (ScreenPoint[1] * this->V));
		R.MinT	= -1000.0f;
		R.MaxT	= 1000.0f;
		
		if (this->ApertureSize != 0.0f)
		{
			Vec2f LensUV;

			switch (this->ApertureShape)
			{
				case Enums::Circular:
				{
					LensUV = this->ApertureSize * ConcentricSampleDisk(RNG.Get2());
					break;
				}

				case Enums::Polygon:
				{
					const float LensY		= RNG.Get1() * this->NoApertureBlades;
					const int Side			= (int)LensY;
					const float Offset		= (float) LensY - Side;
					const float Distance	= (float) sqrtf(RNG.Get1());
					const float A0 			= (float) (Side * PI_F * 2.0f / this->NoApertureBlades + this->ApertureAngle);
					const float A1 			= (float) ((Side + 1.0f) * PI_F * 2.0f / this->NoApertureBlades + this->ApertureAngle);
					const float EyeX 		= (float) ((cos(A0) * (1.0f - Offset) + cos(A1) * Offset) * Distance);
					const float EyeY 		= (float) ((sin(A0) * (1.0f - Offset) + sin(A1) * Offset) * Distance);
					
					LensUV[0] = EyeX * this->ApertureSize;
					LensUV[1] = EyeY * this->ApertureSize;
					break;
				}
			}
			
			const Vec3f LI = this->U * LensUV[0] + this->V * LensUV[1];

			R.O += LI;
			R.D = Normalize(R.D * this->FocalDistance - LI);
		}
	}

	/*! Projects a point \a P in world space onto the camera film plane
		@param[in] P Point in world space
		@param[out] FilmUV Position on the film plane
		@return Whether the project point is with the film plane
	*/
	HOST_DEVICE bool ProjectPointToFilmPlane(const Vec3f& P, Vec2f& FilmUV) const
	{
		const Vec3f D = P - this->Pos;
		const Vec3f Dn = Normalize(P - this->Pos);

		const float L = Dot(Dn, this->N);

		const Vec3f FilmP = this->Pos + Dn * (1.0f * L);

		const Vec2f CamUV(Dot(FilmP, this->U), -Dot(FilmP, this->V));

		if (CamUV[0] < this->Film.Screen[0][0] || CamUV[0] > this->Film.Screen[0][1])
			return false;

		if (CamUV[1] < this->Film.Screen[1][0] || CamUV[1] > this->Film.Screen[1][1])
			return false;

		const Vec2f ScreenSize(this->Film.Screen[0][1] - this->Film.Screen[0][0], this->Film.Screen[1][1] - this->Film.Screen[1][0]);

		Vec2f Offset = CamUV - Vec2f(this->Film.Screen[0][0], this->Film.Screen[1][0]);
		
		Offset /= ScreenSize;
		
		FilmUV[0] = (float)this->Film.GetResolution()[0] * Offset[0];
		FilmUV[1] = (float)this->Film.GetResolution()[1] * Offset[1];

		return true;
	}

	GET_SET_TS_MACRO(HOST_DEVICE, Pos, Vec3f)
	GET_SET_TS_MACRO(HOST_DEVICE, Target, Vec3f)
	GET_SET_TS_MACRO(HOST_DEVICE, Up, Vec3f)
	GET_SET_TS_MACRO(HOST_DEVICE, FocusMode, Enums::FocusMode)
	GET_SET_TS_MACRO(HOST_DEVICE, FocusUV, Vec2f)
	GET_SET_TS_MACRO(HOST_DEVICE, FocalDistance, float)
	GET_SET_TS_MACRO(HOST_DEVICE, ApertureShape, Enums::ApertureShape)
	GET_SET_TS_MACRO(HOST_DEVICE, ApertureSize, float)
	GET_SET_TS_MACRO(HOST_DEVICE, NoApertureBlades, int)
	GET_SET_TS_MACRO(HOST_DEVICE, ApertureAngle, float)
	GET_SET_TS_MACRO(HOST_DEVICE, FOV, float)
	GET_SET_TS_MACRO(HOST_DEVICE, N, Vec3f)
	GET_SET_TS_MACRO(HOST_DEVICE, U, Vec3f)
	GET_SET_TS_MACRO(HOST_DEVICE, V, Vec3f)


	/*! Updates internal members */
	HOST void Update()
	{
		this->Film.Update(this->FOV);

		this->N = Normalize(this->Target - this->Pos);
		this->U = Normalize(Cross(this->N, this->Up));
		this->V = Normalize(Cross(this->N, this->U));

		if (this->FocalDistance == -1.0f)
			this->FocalDistance = Length(this->Target, this->Pos);
	}

	/*! Returns the film
		@return Film
	*/
	HOST_DEVICE Film& GetFilm()
	{
		return this->Film;
	}

protected:
	Film					Film;					/*! Film plane */
	Vec3f					Pos;					/*! Camera position */
	Vec3f					Target;					/*! Camera target */
	Vec3f					Up;						/*! Camera up vector */
	Enums::FocusMode		FocusMode;				/*! Type of focusing e.g. manual, auto-focus */
	Vec2f					FocusUV;				/*! Focus position on the film plane, used for auto-focus */
	float					FocalDistance;			/*! Focal distance, in case of manual focus */
	Enums::ApertureShape	ApertureShape;			/*! Shape of the aperture e.g. circular, polygonal */
	float					ApertureSize;			/*! Size of the aperture */
	int						NoApertureBlades;		/*! Number of aperture blades, in case of a polygonal aperture */
	float					ApertureAngle;			/*! Offset angle of the aperture blades */
	float					FOV;					/*! Field of view */
	Vec3f					N;						/*! Camera normal vector */
	Vec3f					U;						/*! Camera U vector (to the left) */
	Vec3f					V;						/*! Camera V vector (up) */
};

}
