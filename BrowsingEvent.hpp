/** @file
 * @brief Browsing Event Class
 *
 * This file contains the Browsing Event class to store events from the Consumer Pools and to store the bad events into
 * the Common Storage
 * @author José Manuel Ramos Ruiz
 * @date 23 Dic 2018 - Revisión 1.0
 *
 * @see @ref BrowsingEvent_legal_note_sec
 *
 * @section BrowsingEvent_legal_note_sec Legal Note
 * Only for Qustodio hiring purposes
 *
 * @section BrowsingEvent_intro_sec Introduction
 *
 * This is the model to represents a Browsing Element
 *
 * @section BrowsingEvent_revision_sec Versions
 * Versión | Fecha      | Autor                        | Comentarios adicionales
 * ------: | :--------: | :--------------------------  | -----------------------
 *     1.0 | 2018-12-23 | José Manuel Ramos Ruiz       | Initial Release
 *
 * @section BrowsingEvent_install_sec Use
 *
 * @subsection BrowsingEvent_step1 Requirements
 * Requires C++11 to use it correctly
 */

#pragma once

#ifndef BROWSINGEVENT_H
#define BROWSINGEVENT_H

#include <string>

namespace Qustodio
{

/*! \class BrowsingEvent BrowsingEvent.hpp "BrowsingEvent.hpp"
 *  \brief Browsing event model.
 *
 * Contains the model for a basic browsing event.
 */
class BrowsingEvent
{
public:
  std::string Device ();                           //< Getter to Device MAC-Address
  void Device ( const std::string &device );       //< Setter to Device MAC-Address
  std::string Url ();                              //< Getter to the visited website
  void Url ( const std::string &url );             //< Setter to the visited website
  std::string Timestamp ();                        //< Getter to time in seconds since UNIX epoch event took place
  void Timestamp ( const std::string &timestamp ); //< Setter to time in seconds since UNIX epoch event took place

protected:
private:
  std::string m_Device;    //< Device MAC-Address
  std::string m_Url;       //< the visited website
  std::string m_Timestamp; //< time in seconds since UNIX epoch event took place
};

} // namespace Qustodio

#endif // BROWSINGEVENT_H
