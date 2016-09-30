//
// IBM AntiVirus Immune System
//
// File Name: Profile.h
// Author:    Milosz Muszynski
//
// Generic Profile Module
//
// $Log: $
//

#ifndef _PROFILE_H_
#define _PROFILE_H_


// this structure is used for a profile initialization
// it is a simple "C" structure rather than a class
// in order to allow concise initilization syntax
struct ProfileItemData
{
	char *		_key;
	char *		_default;
	char *		_value;
	char *		_description;
};


// this class is used internally by the Profile class
// to maintain all data about profile information items
class ProfileItem
{
private:
	std::string    _key;
	std::string    _default;
	std::string    _value;
	std::string    _description;
public:
	ProfileItem( const ProfileItemData& data );
	virtual ~ProfileItem();

	std::string&	key			(){ return _key;			}
	std::string&	defaultValue(){ return _default;		}
	std::string&	value		(){ return _value;			}
	std::string&	description	(){ return _description;	}
};


typedef ProfileItem * PProfileItem;


typedef std::map< std::string, PProfileItem > ProfileCollection;


class Profile
{
private:
	const ProfileItemData * _items;
	int						_count;
	std::string				_fileName;	
	ProfileCollection		_collection;

	enum					{ LineSize = 4096 };

public:
	// the constructor of this class requires an array of ProfileItemData elements,
	// number of such elements and a file name
	// profile class will not recognize profile items that do not have an entry
	// in the items array
	// the purpose of the items array is to keep information about all profile
	// items including default values and descriptions in one place
	// current state of the profile can be retrieved with use of the getAll method
	Profile( const ProfileItemData * items, int count, std::string fileName );
	virtual ~Profile();

private:
	void 			init		();
	bool			read		();
	void			clear		();
	
public:
	std::string		getString	( const std::string& key );
	bool			getBool		( const std::string& key );
	int				getInt 		( const std::string& key );
	long			getLong		( const std::string& key );

	std::string		getAll		();
};


#endif _PROFILE_H_
