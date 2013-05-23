#pragma once
#include <cmath>
#include "IBuffer.h"
#include "QSplat.h"

#define M_PI       3.14159265358979323846

using namespace glm;
using namespace std;

class MockBuffer : public IBuffer {
private:
	QSplat *array;
	vector<QSplat> vectorArray;
	int lenght;

	float radius;

	inline void setSplat(int index, vec3 position) {
		vectorArray[index].position = position;
		vectorArray[index].radius = radius;//.003;//.1f;
		vectorArray[index].normal = QSplat::pack(position, 0);
		vectorArray[index].color = QSplat::packColor(position*.5f + vec3(.5f), 255); //0xFFFFFFFF;
	}

public:

	MockBuffer(void) {
		lenght = 1188;
		//array = new QSplat[lenght];
		vectorArray.resize(lenght);
		radius = .1f;

		for (int i = 1; i < lenght; i++) {
			int mod6 = i % 36; //8
			int div6 = i / 36+1; //8
			float thetha = (2.0f * M_PI / 36) * (mod6 ); //8
			float phi = (5.0f * M_PI / 180.0f) * (div6 + 1); //15.0f

			float x = cos(thetha) * sinf(phi);
			float y = sin(thetha) * sinf(phi);
			float z = cosf(phi);

			setSplat(i, vec3(x,y,z));
		}

		setSplat(0, vec3(0,0,1));
	}

	MockBuffer(int i) {
		lenght = 1920*1200;
		//array = new QSplat[lenght];
		vectorArray.resize(lenght);
		radius = .003f;


		for (int i = 1; i < lenght; i++) {
			int mod6 = i % 2520; //8
			int div6 = i / 2520; //8
			float thetha = (2.0f * M_PI / 2160) * (mod6 ); //8
			float phi = (.195f * M_PI / 180.0f) * (div6 + 1); //15.0f

			float x = cos(thetha) * sinf(phi);
			float y = sin(thetha) * sinf(phi);
			float z = cosf(phi);

			setSplat(i, vec3(x,y,z));
		}

		setSplat(0, vec3(0,0,1));
	}

	~MockBuffer(void) {
		//delete array;
	}

	// Return the array of splats
	virtual const QSplat *GetArray(void) const {
		//return array;
		return &vectorArray[0];
	}

	// Return the length of the array
	virtual size_t Length(void) const {
		return lenght;
	}
	
	
	virtual std::vector<QSplat> &Array(Accessor_t accessor) {
		return vectorArray;
	}

	virtual const std::vector<QSplat> &Array(Accessor_t accessor) const {
		return vectorArray;
	}

	// Add a splat to the end of the array
	virtual void Add(const QSplat *splat) {
	}
	

	
	// (Producer) Call this to swap the buffer
	virtual void Swap(void) {
	}

	///<summary>
	/// Locks the associated buffer. (0=Traverser, 1=Renderer)
	///</summary>
	///<param name="index">The index of the buffer to lock (0=Traverser, 1=Renderer)</param>
	virtual void Lock(Accessor_t accessor) {
	}

	///<summary>
	/// Unlocks the associated buffer. (0=Traverser, 1=Renderer)
	///</summary>
	///<param name="index">The index of the buffer to lock (0=Traverser, 1=Renderer)</param>
	virtual void Unlock(Accessor_t accessor) {
	}

	
	///<summary>
	///Returns which version of the buffer's contents.
	///</summary>
	virtual size_t GetVersion() const {
		return 0;
		static char s = 0;
		return s++;
	}
};

