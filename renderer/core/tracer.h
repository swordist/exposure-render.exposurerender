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

#include "transferfunction\transferfunctions.h"

namespace ExposureRender
{

/*! \class VolumeProperty
 * \brief Volume property class which determines the appearance of the volume
 */
class EXPOSURE_RENDER_DLL Tracer
{
public:
	/*! Default constructor */
	HOST Tracer() :
		Opacity1D(),
		Diffuse1D(),
		Specular1D(),
		Glossiness1D(),
		IndexOfReflection1D(),
		Emission1D(),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(1000.0f),
		OpacityModulated(true),
		GradientFactor(0.5f),
		GradientMode(Enums::CentralDifferences),
		AcceleratorType(Enums::Octree),
		StepFactorPrimary(1.0f),
		StepFactorOcclusion(1.0f)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Volume property to copy
	*/
	HOST Tracer(const Tracer& Other) :
		Opacity1D(),
		Diffuse1D(),
		Specular1D(),
		Glossiness1D(),
		IndexOfReflection1D(),
		Emission1D(),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(100),
		OpacityModulated(true),
		GradientFactor(0.5f),
		GradientMode(Enums::CentralDifferences),
		AcceleratorType(Enums::Octree),
		StepFactorPrimary(1.0f),
		StepFactorOcclusion(1.0f)
	{
		*this = Other;
	}

	/*! Assignment operator
		@param[in] Other Volume property to copy
		@return Volume property
	*/
	HOST Tracer& operator = (const Tracer& Other)
	{
		this->Opacity1D				= Other.Opacity1D;
		this->Diffuse1D				= Other.Diffuse1D;
		this->Specular1D			= Other.Specular1D;
		this->Glossiness1D			= Other.Glossiness1D;
		this->IndexOfReflection1D	= Other.IndexOfReflection1D;
		this->Emission1D			= Other.Emission1D;
		this->Shadows				= Other.Shadows;
		this->ShadingType			= Other.ShadingType;
		this->DensityScale			= Other.DensityScale;
		this->OpacityModulated		= Other.OpacityModulated;
		this->GradientFactor		= Other.GradientFactor;
		this->GradientMode			= Other.GradientMode;
		this->AcceleratorType		= Other.AcceleratorType;
		this->StepFactorPrimary		= Other.StepFactorPrimary;
		this->StepFactorOcclusion	= Other.StepFactorOcclusion;
		
		return *this;
	}
	
	/*! Gets the opacity at \a Intensity from the opacity transfer function
		@param[in] Intensity Intensity at which to fetch the opacity
		@return Opacity
	*/
	DEVICE float GetOpacity(const short& Intensity)
	{
		return this->Opacity1D.Evaluate(Intensity);
	}
	
	/*! Gets the diffuse color at \a Intensity from the diffuse transfer function
		@param[in] Intensity Intensity at which to fetch the diffuse color
		@return Diffuse color
	*/
	DEVICE ColorXYZf GetDiffuse(const short& Intensity)
	{
		return this->Diffuse1D.Evaluate(Intensity);
	}
	
	/*! Gets the specular color at \a Intensity from the specular transfer function
		@param[in] Intensity Intensity at which to fetch the specular color
		@return Specular color
	*/
	DEVICE ColorXYZf GetSpecular(const short& Intensity)
	{
		return this->Specular1D.Evaluate(Intensity);
	}
	
	/*! Gets the glossiness at \a Intensity from the glossiness transfer function
		@param[in] Intensity Intensity at which to fetch the glossiness
		@return Glossiness
	*/
	DEVICE float GetGlossiness(const short& Intensity)
	{
		return this->Glossiness1D.Evaluate(Intensity);
	}
	
	/*! Gets the index of reflection at \a Intensity from the index of reflection transfer function
		@param[in] Intensity Intensity at which to fetch the index of reflection
		@return Index of reflection
	*/
	DEVICE float GetIndexOfReflection(const short& Intensity)
	{
		return this->IndexOfReflection1D.Evaluate(Intensity);
	}
	
	/*! Gets the emission color at \a Intensity from the emission transfer function
		@param[in] Intensity Intensity at which to fetch the emission color
		@return Emission color
	*/
	DEVICE ColorXYZf GetEmission(const short& Intensity)
	{
		return this->Emission1D.Evaluate(Intensity);
	}

	GET_REF_SET_MACRO(HOST_DEVICE, Opacity1D, ScalarTransferFunction1D)
	GET_REF_SET_MACRO(HOST_DEVICE, Diffuse1D, ColorTransferFunction1D)
	GET_REF_SET_MACRO(HOST_DEVICE, Specular1D, ColorTransferFunction1D)
	GET_REF_SET_MACRO(HOST_DEVICE, Glossiness1D, ScalarTransferFunction1D)
	GET_REF_SET_MACRO(HOST_DEVICE, IndexOfReflection1D, ScalarTransferFunction1D)
	GET_REF_SET_MACRO(HOST_DEVICE, Emission1D, ColorTransferFunction1D)
	GET_SET_MACRO(HOST_DEVICE, Shadows, bool)
	GET_SET_MACRO(HOST_DEVICE, ShadingType, Enums::ShadingMode)
	GET_SET_MACRO(HOST_DEVICE, DensityScale, float)
	GET_SET_MACRO(HOST_DEVICE, OpacityModulated, bool)
	GET_SET_MACRO(HOST_DEVICE, GradientFactor, float)
	GET_SET_MACRO(HOST_DEVICE, GradientMode, Enums::GradientMode)
	GET_SET_MACRO(HOST_DEVICE, AcceleratorType, Enums::AcceleratorType)
	GET_SET_MACRO(HOST_DEVICE, StepFactorPrimary, float)
	GET_SET_MACRO(HOST_DEVICE, StepFactorOcclusion, float)

protected:
	ScalarTransferFunction1D	Opacity1D;					/*! Opacity transfer function */
	ColorTransferFunction1D		Diffuse1D;					/*! Diffuse color transfer function */
	ColorTransferFunction1D		Specular1D;					/*! Specular color transfer function */
	ScalarTransferFunction1D	Glossiness1D;				/*! Glossiness transfer function */
	ScalarTransferFunction1D	IndexOfReflection1D;		/*! Index of reflection transfer function */
	ColorTransferFunction1D		Emission1D;					/*! Emission color transfer function */
	bool						Shadows;					/*! Whether to render shadows */
	Enums::ShadingMode			ShadingType;				/*! Type of shading */
	float						DensityScale;				/*! Overall density scale of the volume */
	bool						OpacityModulated;			/*! Whether hybrid scattering is opacity modulated or not */
	float						GradientFactor;				/*! Parameter which controls the amount of BRDF vs. Phase function scattering */
	Enums::GradientMode			GradientMode;				/*! Determines how gradients are computed */
	Enums::AcceleratorType		AcceleratorType;			/*! Type of ray traversal accelerator */
	float						StepFactorPrimary;			/*! Step factor for primary rays */
	float						StepFactorOcclusion;		/*! Step factor for shadow rays */
};

}
