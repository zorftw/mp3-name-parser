#pragma once

#ifndef MP3_H
#define MP3_H

#include <cstdint>
#include <fstream>
#include <Windows.h>
#include <filesystem>

namespace fs = std::filesystem;


struct s_tagv1 {
	char			_tag[3];
	char			_name[30];
	char			_artist[30];
	char			_album[30];
	char			_year[4];
	char			_comment[30];
	std::int8_t		_genre;
};

class c_mp3file {
private:
	std::size_t		_file_size;
	std::uint8_t	*_file_buffer;
	std::string		_original_file_name;
	std::string		_file_name;

	fs::path		_original_path;
public:

	s_tagv1 *get_tagv1( ) const {
		return reinterpret_cast<s_tagv1 *>( (_file_buffer + _file_size) - sizeof( s_tagv1 ));
	}

	bool is_tagv1_valid( ) const {
		return get_tagv1( ) ? memcmp( get_tagv1( )->_tag, "TAG", 3) == 0 : false;
	}

	std::string strip_string_to_length( std::string &in, std::size_t length = 30 ) const {
		if ( in.length( ) <= length )
			return in;

		auto out = in.substr( 0, 30 );
		memcpy( (void*)(out.c_str( ) + 27), "...", 3 );

		return out;
	}

	// check if the file name contains author and title
	// if so, fix the file name and change the tag
	void fix_file_name_and_tag( ) {
		auto res = _original_file_name.find_first_of( '-' );

		// not found
		if ( res == std::string::npos)
			return;

		// extract title and author
		auto title = _original_file_name.substr( res + 1, _original_file_name.length() - (res + 1) - 4 );
		auto author = _original_file_name.substr( 0, res - 1 );

		// set the new file name
		_file_name = title;
		_file_name += ".mp3"; // add extension lol

		title = strip_string_to_length( title );
		author = strip_string_to_length( author );

		// write title and author to the v1 tag
		auto tag = get_tagv1( );

		// empty out artist and write it
		memset( tag->_artist, 0, 30 );
		memcpy( tag->_artist, author.c_str(), min( author.length( ), 30 ) );

		printf( "track name: %s -> %s", tag->_name, title.c_str( ) );

		// empty out track name and write it
		memset( tag->_name, 0, 30 );
		memcpy( tag->_name, title.c_str( ), min( title.length( ), 30 ) );
	}

	void write_new_and_delete( ) {
		// unchanged? 
		if ( _file_name.compare( _original_file_name ) == 0 )
			return;

		auto new_path = _original_path.parent_path( ).append( _file_name );

		fs::remove( _original_path );

		// open a write stream to a new file
		auto stream = std::ofstream( new_path, std::ios::binary );
		stream.write( (char*)_file_buffer, _file_size );
		stream.close( );
	}


	c_mp3file(fs::path path_to_file, std::ifstream _file ) : _original_path(path_to_file), _original_file_name( path_to_file.filename().string() ) {
		_file_name = _original_file_name;

		if ( !_file.is_open( ) )
		{
			printf( "Unable to parse a file...\n" );
			auto i = getchar( );
			exit( -1 );
		}

		// get the size of file
		_file.seekg( 0, std::ios::end );
		_file_size = _file.tellg( );

		// go back to the beginning
		_file.seekg( 0, std::ios::beg );

		// create a buffer
		_file_buffer = new std::uint8_t[_file_size];
		_file.read( reinterpret_cast<char*>(_file_buffer), _file_size );

		// close it
		_file.close( );
	}

	~c_mp3file( ) {
		if ( !_file_buffer )
			delete[] _file_buffer;
	}


};

#endif