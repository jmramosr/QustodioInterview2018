/** @file
 * @brief Filter Events
 *
 * This file contains the filter event class to filter events from the Common Storage Component
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

#ifndef FILTEREVENTS_HPP
#define FILTEREVENTS_HPP

#include "CommonStorageComponent.hpp"
#include "BrowsingEvent.hpp"

namespace Qustodio
{
  class FilterEvents
  {
    public:
    FilterEvents ( Qustodio::CommonStorageComponent &commonStorageComponent, const std::string &filter = 0 );

    /**
     * @brief Filters all captured events using the #regexString filter
     * @param regexString [in] the filter
     */
    void filterBadWords ( const std::string &regexString );

    /**
     * @brief Show the number of filtered results
     */
    void showFilteredResultsCount ( void );
    private:

    /**
     * @brief Given a #regexString, filters all #BrowsingEvent to match the filter
     * @param element [in] a #BrowsingElement
     * @param regexString [in] the filter
     */
    void filterUrl ( Qustodio::BrowsingEvent &element, const std::string &regexString);
    ComposerPool pool;             //< The thread pool to filter the Events
    std::mutex browsingEventMutex; //< The synchronize mechanism to make insertions sequentially
    Qustodio::CommonStorageComponent &mCommonStorageComponent; //< Access to the CommonStorageComponent
    std::string mFilter; //< The filter used
    uint32_t countFilteredElements;//< The amount of filtered elements

    // Can't be auto!
    std::shared_ptr <std::vector< Qustodio::BrowsingEvent>> browsingEvent =
            std::make_shared < std::vector < Qustodio::BrowsingEvent >> (
                    std::vector< Qustodio::BrowsingEvent >() ); //< The vector of #BrowsingEvent
  };
} // namespace Qustodio


#endif //FILTEREVENTS_HPP
