#ifndef IBUFFER_H
#define IBUFFER_H

#include <cstdint>
#include <vector>

#include "QSplat.h"

// An abstract base class for a splat buffer.
class IBuffer
{
public:
	
	///<summary>
	/// The accessor; 
	///</summary>
	enum Accessor_t
	{
		Traverser = 0,
		Renderer  = 1
	};

	///<summary>
	/// Locks the associated buffer.
	///</summary>
	virtual std::vector<QSplat> &Array(Accessor_t accessor) = 0;

	///<summary>
	/// Locks the associated buffer.
	///</summary>
	virtual const std::vector<QSplat> &Array(Accessor_t accessor) const = 0;

	///<summary>
	/// Locks the associated buffer.
	///</summary>
	virtual void Swap() = 0;

	///<summary>
	/// Locks the associated buffer.
	///</summary>
	virtual void Lock(Accessor_t accessor) = 0;

	///<summary>
	/// Unlocks the associated buffer. (0=Traverser, 1=Renderer)
	///</summary>
	virtual void Unlock(Accessor_t accessor) = 0;

	///<summary>
	///Returns the buffer's version.
	///</summary>
	virtual size_t GetVersion() const = 0;
};

#endif // IBUFFER_H