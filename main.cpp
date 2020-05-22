#include "CommonStorageComponent.hpp"
#include "FilterEvents.hpp"

int main ( void )
{

  Qustodio::CommonStorageComponent commonStorageComponent;

  commonStorageComponent.readFromFile( "input_01.txt" );

  Qustodio::FilterEvents filterEvents(commonStorageComponent, ".*(porn|xxx).*" );
  filterEvents.showFilteredResultsCount();

  return 0;
}
