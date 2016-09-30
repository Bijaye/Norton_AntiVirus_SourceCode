// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// BufferClass.cpp : Defines the entry point for the console application.
//
#ifndef FASTARRAY_H
#define FASTARRAY_H


#ifndef SYM_NLM
	#include <memory.h>
#else
	#include <string.h>
#endif
	#include <stdlib.h>

#include "Array.h"


#ifdef FORCE_STD_NAMESPACE
	#define DEC_MEMSET std::memset 
	#define DEC_MEMCPY std::memcpy
#else
	#define DEC_MEMSET memset 
	#define DEC_MEMCPY memcpy
#endif

// --------------------------------------------------------------------------------------------------------------------
//	FastArray is a class that contains a default buffer for use by Array.
//	The purpose of FastArray is to allow for the use of a stack buffer for normal operations while
//	allowing for larger data via the growth characteristics of Array.
//	This class is templatized of a primitve type and a default buffer size.
// --------------------------------------------------------------------------------------------------------------------

template< class T , size_t S > 	
class FastArray : public SymArray < T > {

public:


// --------------------------------------------------------------------------------------------------------------------
//	
// --------------------------------------------------------------------------------------------------------------------
	FastArray() :  SymArray < T >(&m_Buffer[0],0,sizeof(m_Buffer)/sizeof( T ) ) 
	{	m_nStackBufferSize = sizeof(m_Buffer)/sizeof( T );
		m_nMaxElements = 0;
		m_bInitMemory = false;
	}

	virtual ~FastArray() 
	{	if (SymArray<T>::m_pBuffer && ( SymArray<T>::m_pBuffer != &m_Buffer[0] ))
		{
			free(SymArray<T>::m_pBuffer);
		}
	};

	void initializeMemory(bool flag)
	{
		m_bInitMemory = flag;
	}

// --------------------------------------------------------------------------------------------------------------------
//	setSize resets the size of allocated elements.  This routine will grow or shrink the array as needed.  NOTE:
//	a size of 0 is NOT allowed.
// --------------------------------------------------------------------------------------------------------------------
	void setSize( size_t size )
	{
		if ( size == 0 )
		{
			size = 1;
		}
		else if ( m_nMaxElements != 0 && size > m_nMaxElements )
		{
			size = m_nMaxElements;
		}

		if ( size > SymArray<T>::m_nElements )
		{
			growArray(size);
		}
		else if ( size < SymArray<T>::m_nElements )
		{
			shrinkArray(size);
		}
	}

// --------------------------------------------------------------------------------------------------------------------
//	getMaxSize returns the maximum allowed size for the growable array.  A value of 0 means there is no limit.
// --------------------------------------------------------------------------------------------------------------------
	size_t	getMaxSize() const 
	{
		return m_nMaxElements;
	}

// --------------------------------------------------------------------------------------------------------------------
//	canGrow returns true if the array is allowed to grow further.  This routine always returns true if 
//	no maxSize has been set.
// --------------------------------------------------------------------------------------------------------------------
	inline bool canGrow() const
	{
		bool result = true;
		if ( m_nMaxElements != 0 && SymArray<T>::m_nElements == m_nMaxElements )
		{
			result = false;
		}
		return result;
	}

// --------------------------------------------------------------------------------------------------------------------
//	setMaxSize sets the maximum size the array is allowed to grow to.  A value of zero means no growth limit.  If
//	the value passed is less than the currently allocated size the array will be shrunk to the next maximum.
//	If the position (used by operators *,++,--,&) is beyond the new Max size the position is moved to the new end.
// --------------------------------------------------------------------------------------------------------------------
	void setMaxSize( size_t size )
	{
		if ( size == 0 )
		{
			// grow unbounded
			m_nMaxElements = 0;
		}
		else if ( size > m_nMaxElements )
		{
			m_nMaxElements = size;
		} 
		else 
		{
			m_nMaxElements = shrinkArray(size);
		}
	}

// --------------------------------------------------------------------------------------------------------------------
//	
// --------------------------------------------------------------------------------------------------------------------
	inline SymArray< T >& operator= ( T* pointer ) 
	{
		if ( pointer >= &SymArray<T>::m_pBuffer[0] && pointer <= &SymArray<T>::m_pBuffer[SymArray<T>::m_nElements] )
		{
			SymArray<T>::m_nPosition = (pointer-&SymArray<T>::m_pBuffer[0]);
		}
		else
		{
			throw "Pointer No Longer Valid";
		}
		return *this;
	}

protected:
	virtual void reset_() {
		shrinkArray( S );
		if ( m_bInitMemory ) 
		{
			DEC_MEMSET(SymArray<T>::m_pBuffer,0,SymArray<T>::m_nElements*sizeof( T ));
		}
				
	}
	
	bool getBufferFor(size_t nPosition, T*& pBufferFillin, size_t& nStartingOffsetFillin, size_t& nElementsFillin)
	{
		bool result = true;

		if ( m_nMaxElements != 0 && nPosition >= m_nMaxElements )
		{
			result = false;
		}
		else if ( nPosition >= SymArray<T>::m_nElements )
		{
			nStartingOffsetFillin = 0;
			growArray(nPosition);
			nElementsFillin = SymArray<T>::m_nElements;
			pBufferFillin = SymArray<T>::m_pBuffer;
		}
		return result;
	}

private:
	// Disallowed
	inline FastArray< T , S >& operator= ( const FastArray< T , S >& rhs ) 
	{ 
		return *this;
	};

// --------------------------------------------------------------------------------------------------------------------
//	growArray attempts to grow the array to encompase the value passed in.
// --------------------------------------------------------------------------------------------------------------------
	size_t growArray(size_t defaultValue)
	{
		size_t	result = defaultValue;
		size_t	clearIndex = SymArray<T>::m_nElements;

		if ( result >= SymArray<T>::m_nElements ) 
		{
			bool doGrow = true;
			size_t growTo;
			if ( SymArray<T>::m_nElements < 8192 ) 
			{
				growTo = SymArray<T>::m_nElements*2;
			}
			else
			{
				growTo = SymArray<T>::m_nElements+(8192*4);
			}
			
			if ( growTo < defaultValue )
			{
				growTo = defaultValue;
			}


			// see if we have a max size?
			if ( m_nMaxElements != 0 &&
				 growTo > m_nMaxElements )
			{
				// See if we have room to grow and if
				// we do see if the passed in value will fit
				// in the adjusted size.
				if ( SymArray<T>::m_nElements < m_nMaxElements &&
					 result < m_nMaxElements )
				{
					// We can grow upto m_nMaxElements
					// and still meet the needs.
					growTo = m_nMaxElements;

				}
				else
				{
					doGrow = false;
				}
			}


			if ( doGrow )
			{
				T* pNewBuffer = SymArray<T>::m_pBuffer;
				// See if we can use Realloc instead of MALLOC
				if ( SymArray<T>::m_pBuffer != &m_Buffer[0] ) 
				{
					pNewBuffer = ( T* ) realloc(SymArray<T>::m_pBuffer,(growTo*sizeof( T ) ) );
					if ( pNewBuffer == NULL )
					{
						throw "Cannot Allocate Memory";
					}
					if (m_bInitMemory )
					{
						DEC_MEMSET(&pNewBuffer[clearIndex],0, (growTo*sizeof( T ))-(sizeof( T )*clearIndex));
					}

				}
				else
				{
					// allocate a new buffer since we transitioning from our default buffer
					// to another buffer.

					pNewBuffer = ( T* ) malloc(growTo*sizeof( T ) );
					if ( pNewBuffer == NULL )
					{
						throw "Cannot Allocate Memory";
					}
					if (m_bInitMemory )
					{
						DEC_MEMSET(&pNewBuffer[sizeof(m_Buffer)/sizeof( T )],0, (growTo*sizeof( T ))-(sizeof(m_Buffer)));
					}
					DEC_MEMCPY(pNewBuffer,&m_Buffer[0], sizeof(m_Buffer) );
				}

				SymArray<T>::m_pBuffer = pNewBuffer;
				SymArray<T>::m_nElements = growTo;
							
			}
		}

		if ( result >= SymArray<T>::m_nElements )
		{
			result = SymArray<T>::m_nElements-1;
		}
		return result;
	}


// --------------------------------------------------------------------------------------------------------------------
//	ShrinkArray will reduce the size of the array as requested.
// --------------------------------------------------------------------------------------------------------------------
	size_t shrinkArray(size_t shrinkTo)
	{
		if ( shrinkTo < SymArray<T>::m_nElements )
		{
			T*	pNewBuffer = SymArray<T>::m_pBuffer;

			// Will it fit in our internal buffer?
			// Are we already using out internal buffer?
			if ( shrinkTo <=  S  )
			{
				if ( SymArray<T>::m_pBuffer !=  &m_Buffer[0] )
				{
					pNewBuffer = &m_Buffer[0];
					DEC_MEMCPY( &m_Buffer[0], SymArray<T>::m_pBuffer, (shrinkTo*sizeof( T ) ) );
					free(SymArray<T>::m_pBuffer);
				}
			}
			else
			{
				pNewBuffer = ( T* ) realloc(SymArray<T>::m_pBuffer,(shrinkTo*sizeof( T ) ) );
			}


			SymArray<T>::m_pBuffer = pNewBuffer;
			SymArray<T>::m_nElements = shrinkTo;	
			if ( SymArray<T>::m_nPosition >= SymArray<T>::m_nElements )
			{
				SymArray<T>::m_nPosition = SymArray<T>::m_nElements;
			}
		}

		return SymArray<T>::m_nElements;
	}

	T		m_Buffer[ S ];
	size_t	m_nMaxElements;
	size_t	m_nStackBufferSize;
	bool	m_bInitMemory;
};



#endif  //FASTARRAY_H
