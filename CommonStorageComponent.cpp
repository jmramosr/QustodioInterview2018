#include "CommonStorageComponent.hpp"

#include <iostream>
#include <fstream>
#include <regex>

void Qustodio::CommonStorageComponent::insertBrowseEvent ( const std::string &line,
                                                           const std::string &function )
{
  Qustodio::BrowsingEvent browseEvent;

  std::regex rRegex( function + ": (.*)" );
  std::smatch result;
  std::regex_search( line, result, rRegex );

  if ( result[1].length() > 0 )
  {
    if ( "url" == function )
    {
#if SHOW_INTERMEDIATE
      std::cout << "Insert Url: " << result[1] << std::endl;
#endif
      browseEvent.Url( result[1] );
    }
    else if ( "device" == function )
    {
#if SHOW_INTERMEDIATE
      std::cout << "Insert Device: " << result[1] << std::endl;
#endif
      browseEvent.Device( result[1] );
    }
    else if ( "timestamp" == function )
    {
#if SHOW_INTERMEDIATE
      std::cout << "Insert Timestamp: " << result[1] << std::endl;
#endif
      browseEvent.Timestamp( result[1] );
    }
    else
    {
      throw std::invalid_argument( "Invalid function" );
    }
    {
      std::lock_guard< std::mutex > lock( this->browsingEventMutex );
      this->browsingEvent->emplace_back( browseEvent );
    }
  }
  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
}

void Qustodio::CommonStorageComponent::readFromFile ( const std::string &fileToRead )
{
  std::ifstream inp;
  std::string line;

  inp.open( fileToRead );

  if ( inp.is_open() )
  {
    while ( inp.good() )
    {
      std::getline( inp, line );
      pool.enqueue( & Qustodio::CommonStorageComponent::insertBrowseEvent, this, line, "url" );
      pool.enqueue( & Qustodio::CommonStorageComponent::insertBrowseEvent, this, line, "device" );
      pool.enqueue( & Qustodio::CommonStorageComponent::insertBrowseEvent, this, line, "timestamp" );
    }
  }

  pool.wait_until_empty();
  pool.wait_until_nothing_in_flight();

  inp.close();

  #if SHOW_INTERMEDIATE
  std::cout << "Results" << std::endl;
  for ( auto &&result: * this->browsingEvent )
  {
    std::cout << result.Url();
    if ( result.Url().length() > 0 )
    {
      std::cout << std::endl;
    }
  }
  #endif
}

const std::shared_ptr< std::vector< Qustodio::BrowsingEvent>> &
Qustodio::CommonStorageComponent::BrowsingEvent () const
{
  return browsingEvent;
}
