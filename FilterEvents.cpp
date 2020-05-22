/** @file
 * @brief Common Storage Component
 *
 * This file contains the Common Storage Component class to receive events from the Consumer Pools
 * @author José Manuel Ramos Ruiz
 * @date 23 Dic 2018 - Revisión 1.0
 *
 * @see @ref FilterEvents_legal_note_sec
 *
 * @section FilterEvents_legal_note_sec Legal Note
 * Only for Qustodio hiring purposes
 *
 * @section FilterEvents_intro_sec Introduction
 *
 * This is the controller of a Browsing Element to handle concurrenly in C++11
 *
 * @section FilterEvents_revision_sec Versions
 * Versión | Fecha      | Autor                        | Comentarios adicionales
 * ------: | :--------: | :--------------------------  | -----------------------
 *     1.0 | 2018-12-23 | José Manuel Ramos Ruiz       | Initial Release
 *
 * @section FilterEvents_install_sec Use
 *
 * @subsection FilterEvents_step1 Requirements
 * Requires C++11 to use it correctly
 */

#include "FilterEvents.hpp"

#include <iostream>
#include <regex>

Qustodio::FilterEvents::FilterEvents ( Qustodio::CommonStorageComponent &commonStorageComponent, const std::string &filter )
: countFilteredElements(0), mCommonStorageComponent(commonStorageComponent), mFilter(filter)
{
  this->browsingEvent = this->mCommonStorageComponent.BrowsingEvent();
  if ( this->mFilter.length() > 0)
  {
    this->filterBadWords(this->mFilter);
  }
}


void Qustodio::FilterEvents::filterBadWords ( const std::string &regexString )
{
  for ( auto &&result: * this->browsingEvent )
  {
    pool.enqueue( & Qustodio::FilterEvents::filterUrl, this, result, regexString );
  }

  pool.wait_until_empty();
  pool.wait_until_nothing_in_flight();
}

void Qustodio::FilterEvents::showFilteredResultsCount ( void )
{
  std::cout << this->countFilteredElements << std::endl;
}

void Qustodio::FilterEvents::filterUrl ( Qustodio::BrowsingEvent &element, const std::string &regexString )
{
  std::regex strRegex( regexString );
  std::smatch resultUrl;
  std::string url = element.Url();
  std::regex_search( url, resultUrl, strRegex );

  if ( resultUrl[1].length() > 0 )
  {

#if SHOW_INTERMEDIATE
    std::cout << "Found filtered words!" << std::endl;
#endif
    {
      std::lock_guard< std::mutex > lock( this->browsingEventMutex );
      ++this->countFilteredElements;
    }
  }
  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
}