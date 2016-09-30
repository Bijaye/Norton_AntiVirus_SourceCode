#ifndef __ZDIRECTORY__H__
#define __ZDIRECTORY__H__


class ZDirectory
{
private:
	std::string        _path;
    long               _handle;
    struct _finddata_t _findData;
public:
    // constructors/destructors
	ZDirectory() : _path( "" ){}
    ZDirectory( const std::string& path ) : _path( path ){}
	void set( const std::string& path ){ _path = path; }

    // accessors
    const std::string& path() const { return _path; }

    // iterators
    bool        getFirst		( const std::string& filePattern, std::string& fileName ) const;
    bool        getNext			( std::string& fileName ) const;
	void        findClose		() const;

	// predicates
	bool		isNormalFile	() const;
	bool		isDirectory		() const;
};


#endif
