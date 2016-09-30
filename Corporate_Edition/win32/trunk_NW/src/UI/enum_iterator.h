#include <assert.h>

#ifndef ENUM_CHUNK
    #define ENUM_CHUNK 64
#endif

template <typename EnumItf, const IID* pIIDEnumItf,
          typename EnumType, typename CopyClass = _Copy<EnumType> >
class enum_iterator
{
public:
    enum_iterator(IUnknown* punkEnum = 0, ULONG nChunk = ENUM_CHUNK) : m_pRep(0)
    {
        if( punkEnum ) 
        {
            m_pRep = new EnumRep(punkEnum, static_cast<size_t>(nChunk));
            if( m_pRep )
            {
                m_pRep->AddRef();

                if( !m_pRep->First() )
                {
                    Destroy();
                }
            }
        }
    }

    enum_iterator(const enum_iterator& i) : m_pRep(i.m_pRep)
    {
        if( m_pRep ) m_pRep->AddRef();
    }

    ~enum_iterator()
    {
        Destroy();
    }

    enum_iterator& operator=(const enum_iterator& rhs)
    {
        if( *this != rhs )
        {
            Destroy();
            if( m_pRep = rhs.m_pRep ) m_pRep->AddRef();
        }

        return *this;
    }
    
    bool operator!=(const enum_iterator& rhs)
    {
        return !(*this == rhs);
    }
    
    bool operator==(const enum_iterator& rhs)
    {
        return m_pRep == rhs.m_pRep;
    }

    // ++i, i.e. the first half of IEnumXxx::Next ...
    enum_iterator& operator++()
    {
        if( m_pRep && !m_pRep->Next() )
        {
            Destroy();  // We're at the end of the collection
        }

        return *this;
    }
    
    // i++, i.e. half of IEnumXxx::Next ...
    enum_iterator operator++(int)
    {
        enum_iterator   tmp = *this;
        ++(*this);  // Forward to ++i
        return tmp;
    }
    
    // ... the other half of IEnumXxx::Next
    EnumType& operator*()
    {
        assert(m_pRep && "Past the end of the enumeration");
        return m_pRep->Get();
    }

    enum_iterator& operator+=(size_t n)
    {
        if( n ) skip(n-1);
        return *this;
    }

    // IEnumXxx::Skip
    bool skip(size_t n)
    {
        if( m_pRep && !m_pRep->Skip(n) )
        {
            Destroy();  // We're at the end of the collection
            return false;
        }

        return true;
    }

    // IEnumXxx::Reset
    bool reset()
    {
        if( m_pRep && !m_pRep->Reset() )
        {
            Destroy();  // Something bad happened...
            return false;
        }

        return true;
    }

private:
    void Destroy()
    {
        if( m_pRep ) m_pRep->Release();
        m_pRep = 0;
    }

    class EnumRep
    {
    public:
        EnumRep(IUnknown* punkEnum, size_t nChunk)
            : m_pEnum(0), m_rgEnum(0), m_nChunk(static_cast<ULONG>(nChunk)), m_cRef(0), m_n(0), m_cElems(0)
        {
            if( punkEnum )
            {
                punkEnum->QueryInterface(*pIIDEnumItf, (void**)&m_pEnum);
                if( m_pEnum )
                {
                    m_rgEnum = new EnumType[m_nChunk];
                    if( m_rgEnum )
                    {
                        InitChunk();
                    }
                    else
                    {
                        m_pEnum->Release();
                        m_pEnum = 0;
                    }
                }
            }
        }

        ~EnumRep()
        {
            if( m_rgEnum ) {
                DestroyChunk();
                delete[] m_rgEnum;
            }

            if( m_pEnum ) m_pEnum->Release();
        }

        void AddRef()
        {
            ++m_cRef;
        }

        void Release()
        {
            if( !--m_cRef ) delete this;
        }

        bool First()
        {
            return NextChunk();
        }

        bool Next()
        {
            return Skip(0);
        }

        bool Skip(size_t nSkip)
        {
            if( m_pEnum )
            {
                // Skip in our local cache and IEnumXxx::Skip overflow
                m_n += static_cast<ULONG>(nSkip + 1);
                if( m_n >= m_cElems )   // We're past the end of this chunk
                {
                    ULONG   celtToSkip = m_n - m_cElems;
                    HRESULT hr = (celtToSkip ? m_pEnum->Skip(celtToSkip) : S_OK);
                    if( FAILED(hr) ||           // Skipping didn't work so well
                        (hr == S_FALSE) ||      // Not that many elements to skip
                        m_cElems < m_nChunk ||  // Last m_pEnum->Next() returned S_FALSE
                        !NextChunk() )          // Last m_pEnum->Next() return S_OK
                    {
                        return false;   // We're at the end of the collection
                    }
                    else
                    {
                        m_n = 0;    // We're at the beginning of a new chunk
                    }
                }
            }

            return true;
        }

        bool Reset()
        {
            if( m_pEnum )
            {
                // Ask the enumerator to reset and cache the first chunk
                if( FAILED(m_pEnum->Reset()) || !First() )
                {
                    return false;
                }
            }

            return true;
        }

        EnumType& Get()
        {
            assert(m_pEnum && "No elements cached");
            assert(m_n < m_cElems && "No elements left");
            return m_rgEnum[m_n];
        }

    private:
        EnumItf*    m_pEnum;    // Pointer to the enumeration interface
        EnumType*   m_rgEnum;   // Array of the enumeration data type
        ULONG       m_cElems;   // Number of elements we got last time
        const ULONG m_nChunk;   // Number of elements to ask for each time
        ULONG       m_cRef;     // References to this EnumRep for efficient copying of enum_iterator
        ULONG       m_n;        // Current offset into m_rgEnum array of m_cElems items

        void InitChunk()
        {
            for( EnumType* p = &m_rgEnum[0]; p != &m_rgEnum[m_nChunk]; ++p )
            {
                CopyClass::init(p);
            }
        }

        // Use the CopyClass::destroy but leave data available for next chunk
        void DestroyChunk()
        {
            for( EnumType* p = &m_rgEnum[0]; p < &m_rgEnum[m_cElems]; ++p )
            {
                CopyClass::destroy(p);
            }

            m_cElems = 0;
        }

        bool NextChunk()
        {
            DestroyChunk();

            HRESULT hr = (m_pEnum ? m_pEnum->Next(m_nChunk, m_rgEnum, &m_cElems) : E_UNEXPECTED);
            if( hr == S_OK ) m_cElems = m_nChunk;
            else if( FAILED(hr) ) m_cElems = 0;

            // Coerce m_cElems == 0 to a failure
            // so iterator knows it's stepped off the end
            if( SUCCEEDED(hr) && m_cElems == 0 ) hr = E_FAIL;

            return (SUCCEEDED(hr) ? true : false);
        }
    };

    EnumRep*    m_pRep;
};

// Typedefs for standard enumerators
typedef enum_iterator<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT>  variant_iterator;
typedef enum_iterator<IEnumMoniker, &IID_IEnumMoniker, IMoniker*, _CopyInterface<IMoniker> > mk_iterator;
typedef enum_iterator<IEnumString, &IID_IEnumString, LPOLESTR> olestr_iterator;
typedef enum_iterator<IEnumUnknown, &IID_IEnumUnknown, IUnknown*, _CopyInterface<IUnknown> > unk_iterator;
typedef enum_iterator<IEnumSTATSTG, &IID_IEnumSTATSTG, STATSTG> statstg_iterator;
typedef enum_iterator<IEnumFORMATETC, &IID_IEnumFORMATETC, FORMATETC> formatetc_iterator;
typedef enum_iterator<IEnumSTATDATA, &IID_IEnumSTATDATA, STATDATA> statdata_iterator;
typedef enum_iterator<IEnumSTATPROPSTG, &IID_IEnumSTATPROPSTG, STATPROPSTG> statpropstg_iterator;
typedef enum_iterator<IEnumOLEVERB, &IID_IEnumOLEVERB, OLEVERB> oleverb_iterator;

#ifdef __hlink_h__
    typedef enum_iterator<IEnumHLITEM, &IID_IEnumHLITEM, HLITEM> hlitem_iterator;
#endif // __hlink_h__

#ifdef __ocidl_h__
    typedef enum_iterator<IEnumConnections, &IID_IEnumConnections, CONNECTDATA> connection_iterator;
    typedef enum_iterator<IEnumConnectionPoints, &IID_IEnumConnectionPoints, IConnectionPoint*, _CopyInterface<IConnectionPoint> > connectionpoint_iterator;
    typedef enum_iterator<IEnumOleUndoUnits, &IID_IEnumOleUndoUnits, IOleUndoUnit*, _CopyInterface<IOleUndoUnit> > oleundo_iterator;
#endif

#ifdef __comcat_h__
    typedef enum_iterator<IEnumGUID, &IID_IEnumGUID, GUID> guid_iterator;
    typedef guid_iterator clsid_iterator;
    typedef guid_iterator catid_iterator;
    typedef enum_iterator<IEnumCATEGORYINFO, &IID_IEnumCATEGORYINFO, CATEGORYINFO> catinfo_iterator;
#endif  // __comcat_h__

#ifdef __docobj_h__
    typedef enum_iterator<IEnumOleDocumentViews, &IID_IEnumOleDocumentViews, IOleDocumentView*, _CopyInterface<IOleDocumentView> > oledocview_iterator;
#endif


