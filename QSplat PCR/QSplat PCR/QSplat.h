#pragma once

#include <cstdint>
#include "QTypes.h"
#include <glm/glm.hpp>

///<summary>
/// Stored Representation of the partial decoded QSplat Node
///</summary>
struct QSplat_t {

	///<summary>
	/// Pack a vec3 and a signed char into a uint32_t.
	///
	/// Ex:
	///		Normal vector + cone width: pack(vec3(x, y, z), w)
	///		Color channels:             pack(vec3(r, g, b), a)
	///</summary>
	static uint32_t pack(glm::vec3 vector, signed char w) {
		unsigned char nx = (signed char) ((vector.x * 127));
		unsigned char ny = (signed char) ((vector.y * 127));
		unsigned char nz = (signed char) ((vector.z * 127));
		unsigned char nw = w;

		uint32_t tore = 0;

		tore |= nw << 24;
		tore |= nz << 16;
		tore |= ny << 8;
		tore |= nx << 0;

		return tore;
	}

	///<summary>
	/// Pack a vec3 and a unsigned char into a uint32_t.
	///
	/// Ex:
	///		Normal vector + cone width: pack(vec3(x, y, z), w)
	///		Color channels:             pack(vec3(r, g, b), a)
	///</summary>
	static uint32_t packColor(glm::vec3 vector, unsigned char w) {
		unsigned char nx = (unsigned char) (((vector.x) * 255));
		unsigned char ny = (unsigned char) (((vector.y) * 255));
		unsigned char nz = (unsigned char) (((vector.z) * 255));
		unsigned char nw = w;

		uint32_t tore = 0;

		tore |= nw << 24;
		tore |= nz << 16;
		tore |= ny << 8;
		tore |= nx << 0;

		return tore;
	}

	///<summary>
	/// The radian of the Sphere
	///</summary>
	glm::vec3 position;

	///<summary>
	/// The radius of the Sphere
	///</summary>
	float radius;
	
	///<summary>
	/// Normal Vector (x, y, z) and w = Width of Normal Cone
	///</summary>
	uint32_t normal;

	///<summary> 
	/// 32-bit Color Value
	///</summary>
	uint32_t color;

	QSplat_t() :
		position(0.0f),
		radius(0.0f),
		normal(0),
		color(0)
	{}
};

///<summary>
/// Stored Representation of the partial decoded QSplat Node
/// <para/> Redefines: struct QSplat_t
///</summary>
typedef struct QSplat_t QSplat;

///<summary>
/// Represents the QSplat sphere for Occlusion Culling
///</summary>
struct CoverageData_t {

	///<summary>
	/// Represents the radius of the project QSplat circle in screen space
	///</summary>
	float radius;

	///<summary>
	/// Represents the position of the project QSplat circle in screen space
	///</summary>
	glm::vec2 position;

	///<summary>
	/// Represents the depth of the  back of the QSplat sphere in clip space.
	///</summary>
	float depth;
};

///<summary>
/// Represents the QSplat sphere for Occlusion Culling
/// <para/> Redefines: struct CoverageData_t
///</summary>
typedef struct CoverageData_t CoverageData;

