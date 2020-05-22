#include "BrowsingEvent.hpp"

std::string Qustodio::BrowsingEvent::Device ()
{
  return this->m_Device;
}

void Qustodio::BrowsingEvent::Device ( const std::string &device )
{
  this->m_Device= device;
}

std::string Qustodio::BrowsingEvent::Url ()
{
  return this->m_Url;
}

void Qustodio::BrowsingEvent::Url ( const std::string &url )
{
  this->m_Url= url;
}

std::string Qustodio::BrowsingEvent::Timestamp ()
{
  return this->m_Timestamp;
}

void Qustodio::BrowsingEvent::Timestamp ( const std::string &timestamp )
{
  this->m_Timestamp= timestamp;
}
