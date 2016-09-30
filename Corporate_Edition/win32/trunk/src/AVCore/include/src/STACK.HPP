#ifndef _CSTACKOBJECTS_HPP_
#define _CSTACKOBJECTS_HPP_

class CStackObjects
{
public:
    CStackObjects( UINT uMemSize );
   ~CStackObjects();

    BOOL WINAPI Push  (                 // Add a block of memory to the top
        LPSTR );                        // of the stack

    BOOL WINAPI Pop (                   // Remove a block of memory from the
        LPSTR );                        // bottom of the stack

    DWORD WINAPI GetCount (             // Get number of blocks of memory
        VOID );                         // pushed to the stack

    BOOL WINAPI Clear (                 // Clears the stack from everything
        VOID );

    BOOL WINAPI GetObject (             // Get block of memory
        DWORD   dwIndex,                // from this index location and 
        LPSTR   lpMem );                // place it in here

private:
    UINT    m_uCount;
    UINT    m_uBufferSize;
    UINT    m_uBlockSize;
    LPSTR   m_lpBuffer;
};

typedef class CStackObjects FAR * LPSTACKOBJECT;

#endif // _CSTACKOBJECTS_HPP_
