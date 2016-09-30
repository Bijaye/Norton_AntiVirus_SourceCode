// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Array.cpp : Defines the entry point for the console application.
//

#ifndef ARRAY_TEMPLATE
#define ARRAY_TEMPLATE

#ifndef SYM_NLM
	#include <memory.h>
#else
	#include <string.h>
#endif
#include <stdlib.h>

// --------------------------------------------------------------------------------------------------------------------
//@{	class that provides a array over primitive types.
//		
//		Operators:
//			+			Returns pointer to current cursor + value passed. Does not move cursor. (GROWS)
//			++x			Prefix operator moves the cusor ahead by one. (GROWS)
//			x++			Postfix operator moves the cursor ahead by one and returns previous position. (GROWS)
//			-			Returns pointer current cursor - value passed. Does not move cursor. (STICKS)
//			--x			Prefix operator moves the cursor back by one. (STICKS)
//			x--			Postfix operator moves the cursor back by one and returns previous position. (STICKS)
//			[]			Returns element at index specified. (GROWS)
//			*			Returns element at current cursor position.
//			&			Returns pointer to the actual array and NOT this instance!
//			=
//
//		Methods:
//			getPosition	Returns the current position (cursor) in the array. Position used by operators *,++,--,&
//			setPosition Sets the position (cursor) in the array.  Position used by operators *,++,--,&
//			
//@}
// --------------------------------------------------------------------------------------------------------------------


template< class T > 	
class SymArray {

protected:

// --------------------------------------------------------------------------------------------------------------------
//	Protected constructor used by derived classes.
//
//	@param pBuffer			Pointer to the buffer containing the array elements.
//	@param startingOffset	Which element is represented by the first element in the buffer.
//							For example, passing in 512 means that the first valid index is 512. The
//							would then use array[512] to get to the first element in the buffer.
//							This parameter is usually 0.
//	@param elementCount		Indicates how many elements are stored in the array.  An element is a 'T'.
//
// --------------------------------------------------------------------------------------------------------------------
	SymArray(T* pBuffer, size_t startingOffset, size_t elementCount ) 
	: 	m_pBuffer(pBuffer),
		m_nStartingOffset(startingOffset),
		m_nElements(elementCount),
		m_nPosition(m_nStartingOffset)
	{
	};


// --------------------------------------------------------------------------------------------------------------------
//	getBuffer Returns true if the position being passed is valid for use, otherwise false is returned.
// --------------------------------------------------------------------------------------------------------------------
	inline bool getBuffer(size_t nPosition)
	{
		// See if the position is within the buffer.  If not, go a get the data
		if ( !(nPosition >= m_nStartingOffset && (nPosition-m_nStartingOffset) < m_nElements) )
		{
			// Get a buffer that contains nPosition if possible.
			return getBufferFor(nPosition, m_pBuffer, m_nStartingOffset,m_nElements);
		}
		return true;
	}

// --------------------------------------------------------------------------------------------------------------------
//	getBufferFor Given a position the derived class should allocate a buffer that encapsolates that position.  The
//	function must fillin all the fillin paramters specified.  The function returns true if successful, otherwise, false
//	is returned.
// --------------------------------------------------------------------------------------------------------------------
	virtual bool getBufferFor(size_t nPostion, T*& pBufferFillin, size_t& nStartingOffsetFillin, size_t& nElementsFillin) = 0;

// --------------------------------------------------------------------------------------------------------------------
//	reset_ is called when the public reset function is called.
// --------------------------------------------------------------------------------------------------------------------
	virtual void reset_() {};

public:

	inline void reset()
	{
		getBufferFor(0, m_pBuffer, m_nStartingOffset,m_nElements);
		m_nPosition = 0;
		reset_();
	}

// --------------------------------------------------------------------------------------------------------------------
//	append will write the array elements specified to the end of the current array
// --------------------------------------------------------------------------------------------------------------------

	inline void setAt(size_t index, T* elements, size_t count )
	{
		bool done = false;

		if ( count == 0 || elements == NULL )
		{
			done = true;
		}

		size_t bytesToCopy = (count*sizeof( T ));
		size_t sourceIndex = 0;
		size_t newPosition = index+count;

		while ( !done )
		{
			 if ( getBuffer(index) == false )
			 {
				 throw "Cannot allocate buffer";
			 }
			 // Calculate how much data will fit in the current buffer

			 // See how many elements can fit in the buffer

			 size_t availableBytes = ((m_nElements - (index-m_nStartingOffset))*sizeof( T ));

			 if (  availableBytes > 0 )
			 {
				 size_t copyCount = 0;
				 if ( availableBytes > bytesToCopy )
				 {
					copyCount = bytesToCopy;
				 }
				 else
				 {
					 copyCount = availableBytes;
				 }

				 memcpy(&m_pBuffer[index],&elements[sourceIndex],copyCount);

				 bytesToCopy -= copyCount;

				 if ( bytesToCopy == 0 )
				 {
					 done = true;
				 }
				 index += (copyCount/sizeof( T ) );
				 sourceIndex += (copyCount/sizeof( T ) );
			 }
			 else
			 {
				 throw "Cannot allocate buffer";
			 }
		}

		m_nPosition = newPosition;

	}
// --------------------------------------------------------------------------------------------------------------------
//	getSize returns the number of elements in the array. 
// --------------------------------------------------------------------------------------------------------------------
	size_t getSize() const
	{
		return m_nElements;
	}

// --------------------------------------------------------------------------------------------------------------------
//	Destructor 
// --------------------------------------------------------------------------------------------------------------------
	virtual ~SymArray()
	{
	};


// --------------------------------------------------------------------------------------------------------------------
//	getPosition returns the position of the cursor.  The position is used by operators: *,++,--,&
// --------------------------------------------------------------------------------------------------------------------
	inline size_t getPosition( ) const
	{
		return m_nPosition;
	}


// --------------------------------------------------------------------------------------------------------------------
//	setPosition sets a new position within the allocated elements.  The position is used by operators: *,++,--,&
// --------------------------------------------------------------------------------------------------------------------
	inline bool setPosition ( size_t nPosition ) 
	{ 
		bool result;
		
		if ( (result = getBuffer(nPosition)) )
		{
			m_nPosition = nPosition;
		}
		return result;
	};


// --------------------------------------------------------------------------------------------------------------------
//	operator[] returns a reference to the element at index specified by the parameter.  A reference is returned to
//	allow operator[] to be used as an lvalue as well as a getter.
// --------------------------------------------------------------------------------------------------------------------
	inline T& operator[](size_t nPosition)  
	{  
		if ( !getBuffer(nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}
		return m_pBuffer[nPosition-m_nStartingOffset];  
	};


// --------------------------------------------------------------------------------------------------------------------
//	operator* returns a reference to the element at the current position (cursor).  This operator can be used
//	to read or write the element.
// --------------------------------------------------------------------------------------------------------------------
	inline T& operator *()  
	{  
		if ( !getBuffer(m_nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}
		return m_pBuffer[m_nPosition-m_nStartingOffset];  
	};


// --------------------------------------------------------------------------------------------------------------------
//	operator& returns a pointer to the array contained in this class.  NOTE:  because we override operator& you
//	are not able to get the address of the object itself.
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator& ()  
	{  
		if ( !getBuffer(m_nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}
		return &m_pBuffer[0];  
	};


// --------------------------------------------------------------------------------------------------------------------
//	operator++ increments the position (cursor) by one and returns the previous positions contents.
//	This is a Postfix operator ( i.e. x++ ).
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator++(int) 
	{
		if ( !getBuffer(m_nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}

		return  &m_pBuffer[((m_nPosition++)-m_nStartingOffset)];
	}


// --------------------------------------------------------------------------------------------------------------------
//	opertor++ increments the position by one.
//	This is a Prefix operator ( i.e. ++x ).
// --------------------------------------------------------------------------------------------------------------------
	inline void operator++() 
	{
		if ( !getBuffer(m_nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}
		m_nPosition++;
	}


// --------------------------------------------------------------------------------------------------------------------
//	operator-- decrements the current position by one if position is greater than 0 and returns current contents.
//	This is a Postfix operator ( i.e. x-- ).
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator--(int) 
	{
		if ( m_nPosition == 0 || !getBuffer(m_nPosition))
		{
			throw "Array Index Out Of Bounds";
		}

		return  &m_pBuffer[((m_nPosition--)-m_nStartingOffset)];
	}


// --------------------------------------------------------------------------------------------------------------------
//	operator-- decrements the current position by one if position is greater than 0.
//	This is a Prefix operator ( i.e. --x ).
// --------------------------------------------------------------------------------------------------------------------
	inline void operator--() 
	{
		if ( !getBuffer(m_nPosition) )
		{
			throw "Array Index Out Of Bounds";
		}
		m_nPosition--;
	}


// --------------------------------------------------------------------------------------------------------------------
//	operator- returns a pointer to the element specifiec by current position minus value of parameter passed.
//	The position is not modified.
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator- (size_t value) 
	{
		if ( value > m_nPosition || !getBuffer(m_nPosition-value))
		{
			throw "Array Index Out Of Bounds";
		}

		return  &m_pBuffer[(m_nPosition-m_nStartingOffset)-value];
	}


// --------------------------------------------------------------------------------------------------------------------
//	operator-= returns a pointer to the element specifiec by current position minus value of parameter passed.
//	The position is modified.  Same as x = x-y;  If the value passed is greater than the size of the array the 
//	value returned will reference the first element.
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator-= (size_t value) 
	{
		if ( value > m_nPosition || !getBuffer(m_nPosition-value))
		{
			throw "Array Index Out Of Bounds";
		}
		m_nPosition = m_nPosition-value;
		return  &m_pBuffer[(m_nPosition-m_nStartingOffset)];
	}



// --------------------------------------------------------------------------------------------------------------------
//	operator+ returns a pointer to the element specified by current position + value expressed by the paremters.
//	If the value specified is beyond the end of the array and there is no max set, the array will grow to include
//	the value specified.  If the array cannot grow the value returned will be the last element.
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator+ (size_t value) 
	{
		if ( !getBuffer(m_nPosition+value) )
		{
			throw "Array Index Out Of Bounds";
		}
		return  &m_pBuffer[(m_nPosition-m_nStartingOffset)+value];
	}


// --------------------------------------------------------------------------------------------------------------------
//	operator+= returns a pointer to the element specified by current position + value expressed by the paremters.
//	If the value specified is beyond the end of the array and there is no max set, the array will grow to include
//	the value specified.  If the array cannot grow the value returned will be the last element.
//	The current position is modified by this call.  Same as x = x+y.
// --------------------------------------------------------------------------------------------------------------------
	inline T* operator+= (size_t value) 
	{
		if ( !getBuffer(m_nPosition+value) )
		{
			throw "Array Index Out Of Bounds";
		}

		m_nPosition += value;

		return  &m_pBuffer[(m_nPosition-m_nStartingOffset)];
	}



private:
	// Disallowed
	inline SymArray< T >& operator= ( const SymArray< T >& rhs ) 
	{ 
		return *this;
	};

protected:

	T*				m_pBuffer;
	size_t			m_nStartingOffset;
	size_t			m_nElements;
	size_t			m_nPosition;

};

#endif //ARRAY_TEMPLATE


