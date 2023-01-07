#include "mp3.hpp"


int main( ) {

	fs::path directory( "C:\\Users\\Zor\\Music\\Muziek for setjes\\Hard Techno of  Industrial" );

	if ( !fs::is_directory( directory ) )
		return 0;

	auto i = 0;

	for ( const auto &entry : fs::directory_iterator{ directory } )
	{
		if ( entry.is_directory( ) )
			continue;

		if ( !entry.is_regular_file( ) )
			continue;

		auto path = entry.path( );

		if ( !path.has_extension( ) )
			continue;

		// check if it's an MP3
		if ( path.extension( ).string( ).find( "mp3" ) == std::string::npos )
			continue;

		try
		{

			c_mp3file mp3( path, std::ifstream( path, std::ios::binary ) );
			mp3.fix_file_name_and_tag( );
			mp3.write_new_and_delete( );
			++i;

		}
		// too lazy to fix for wide-string characters
		catch ( std::exception e ) { continue; }
	}

	printf( " Changed %d files...\n", i );

	return 0;
}