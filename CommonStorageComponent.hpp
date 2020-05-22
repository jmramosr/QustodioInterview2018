/** @file
 * @brief Common Storage Component
 *
 * This file contains the Common Storage Component class to receive events from the Consumer Pools
 * @author José Manuel Ramos Ruiz
 * @date 23 Dic 2018 - Revisión 1.0
 *
 * @see @ref CommonStorageComponent_legal_note_sec
 *
 * @section CommonStorageComponent_legal_note_sec Legal Note
 * Only for Qustodio hiring purposes
 *
 * @section CommonStorageComponent_intro_sec Introduction
 *
 * This is the controller of a Browsing Element to handle concurrenly in C++11
 *
 * @section CommonStorageComponent_revision_sec Versions
 * Versión | Fecha      | Autor                        | Comentarios adicionales
 * ------: | :--------: | :--------------------------  | -----------------------
 *     1.0 | 2018-12-23 | José Manuel Ramos Ruiz       | Initial Release
 *
 * @section CommonStorageComponent_install_sec Use
 *
 * @subsection CommonStorageComponent_step1 Requirements
 * Requires C++11 to use it correctly
 */

#ifndef COMMONSTORAGECOMPONENT_HPP
#define COMMONSTORAGECOMPONENT_HPP

#include "BrowsingEvent.hpp"
#include "ComposerPool.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace Qustodio
{

/*! \class CommonStorageComponent CommonStorageComponent.hpp "CommonStorageComponent.hpp"
 *  \brief Controller class to use the Browsing Events.
 *
 * Implements the controller of BrowsingEvents
 */
  class CommonStorageComponent
  {
    public:
    CommonStorageComponent () = default;

    ~CommonStorageComponent () = default;

    /**
     * @brief Reads from a file to the CommonStorageComponent
     * @param fileToRead the file to read
     */
    void readFromFile ( const std::string &fileToRead );

    const std::shared_ptr< std::vector< BrowsingEvent>> &BrowsingEvent () const;

    private:
    /**
     * @brief Method to insert a #BrowseEvent in the shared_ptr vector giving the #function name
     * @param line [in] the line to inspect
     * @param function [in] the name of the function to select
     */
    void insertBrowseEvent ( const std::string &line, const std::string &function );

    ComposerPool pool;             //< The thread pool to launch the insertions
    std::mutex browsingEventMutex; //< The synchronize mechanism to make insertions sequentially

    // Can't be auto!
    std::shared_ptr <std::vector< Qustodio::BrowsingEvent>> browsingEvent =
            std::make_shared < std::vector < Qustodio::BrowsingEvent >> (
                    std::vector< Qustodio::BrowsingEvent >() ); //< The vector of #BrowsingEvent
  };

} // namespace Qustodio

#endif // COMMONSTORAGECOMPONENT_HPP
