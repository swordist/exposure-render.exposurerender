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

#include "buffer\host\hostbuffer.h"

namespace ExposureRender
{

/*! 2D host buffer */
template<class T>
class EXPOSURE_RENDER_DLL HostBuffer2D : public HostBuffer<T, 2>
{
public:
	/*! Constructor
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST HostBuffer2D(const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Wrap) :
		HostBuffer<T, 2>(FilterMode, AddressMode)
	{
	}
	
	/*! Get buffer element at discrete position \a X, \a Y
		@param[in] X X position in buffer
		@param[in] Y Y position in buffer
		@return Element at \a X, \a Y
	*/
	HOST_DEVICE T& operator()(const int& X = 0, const int& Y = 0) const
	{
		const Vec2i ClampedXY(Clamp(X, 0, this->Resolution[0] - 1), Clamp(Y, 0, this->Resolution[1] - 1));
		return this->Data[ClampedXY[1] * this->Resolution[0] + ClampedXY[0]];
	}
	
	/*! Get buffer element at position \a XY
		@param[in] XY XY position in buffer
		@return Interpolated value at \a XY
	*/
	HOST_DEVICE T& operator()(const Vec2i& XY) const
	{
		const Vec2i ClampedXY(Clamp(XY[0], 0, this->Resolution[0] - 1), Clamp(XY[1], 0, this->Resolution[1] - 1));
		return this->Data[ClampedXY[1] * this->Resolution[0] + ClampedXY[0]];
	}
	
	/*! Get buffer element at (normalized) floating point position \a XY
		@param[in] XY Floating point position
		@param[in] Normalized Whether \a XY is normalized or not
		@return Interpolated value at \a XY
	*/
	HOST_DEVICE T operator()(const Vec2f& XY, const bool Normalized = false) const
	{
		if (!this->Data)
			return T();

		const Vec2f UV = Normalized ? XY * Vec2f((float)this->Resolution[0], (float)this->Resolution[1]) : XY;

		switch (this->FilterMode)
		{
			case Enums::NearestNeighbour:
			{
				return (*this)((int)floorf(UV[0]), (int)floorf(UV[1]));
			}

			case Enums::Linear:
			{
				int Coord[2][2] =
				{
					{ (int)floorf(UV[0]), (int)ceilf(UV[0]) },
					{ (int)floorf(UV[1]), (int)ceilf(UV[1]) },
				};

				const float du = UV[0] - Coord[0][0];
				const float dv = UV[1] - Coord[1][0];

				Coord[0][0] = min(max(Coord[0][0], 0), this->Resolution[0] - 1);
				Coord[0][1] = min(max(Coord[0][1], 0), this->Resolution[0] - 1);
				Coord[1][0] = min(max(Coord[1][0], 0), this->Resolution[1] - 1);
				Coord[1][1] = min(max(Coord[1][1], 0), this->Resolution[1] - 1);

				T Values[4] = 
				{
					T((*this)(Coord[0][0], Coord[1][0])),
					T((*this)(Coord[0][1], Coord[1][0])),
					T((*this)(Coord[0][0], Coord[1][1])),
					T((*this)(Coord[0][1], Coord[1][1]))
				};

				return (1.0f - dv) * ((1.0f - du) * Values[0] + du * Values[1]) + dv * ((1.0f - du) * Values[2] + du * Values[3]);
			}

			default:
				return T();
		}
	}

	/*! Data setter
		@param[in] Data Data to copy
		@param[in] Width Width of the data
		@param[in] Height Height of the data
	*/
	HOST void SetData(void* Data, const int& Width, const int& Height)
	{
		this->Resize(Vec2i(Width, Height));

		memcpy(this->Data, Data, this->GetNoBytes());
	}

	/*! Gets the buffer's width 
		@return Width
	*/
	HOST_DEVICE int Width() const
	{
		return this->Resolution[0];
	}

	/*! Gets the buffer's height 
		@return Height
	*/
	HOST_DEVICE int Height() const
	{
		return this->Resolution[1];
	}

	/*! Gets the buffer aspect ratio (height/width)
		@return Aspect ratio
	*/
	HOST_DEVICE float AspectRatio() const
	{
		return this->Width() == 0.0f ? 0.0f : ((float)this->Height() / (float)this->Width());
	}


};

}
