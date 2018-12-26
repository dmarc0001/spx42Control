#ifndef SPX42SINGLECOMMAND_HPP
#define SPX42SINGLECOMMAND_HPP

#include <QByteArray>
#include <QList>
#include <limits>
#include "SPX42CommandDef.hpp"

namespace spx
{
  class SPX42SingleCommand;

  // eine lesbarmachung
  using spSingleCommand = std::shared_ptr< SPX42SingleCommand >;

  /**
   * @brief The SPX42SingleCommand class Kapselt eine Antwort vom SPX42
   */
  class SPX42SingleCommand
  {
    private:
    const char command;
    const QList< QByteArray > params;
    const int tag;
    const int sequence;

    public:
    SPX42SingleCommand( char cmd, const QList< QByteArray > &params, int tag = 0, int seq = 0 );  //! Konstruktor
    char getCommand( void );             //! gib das gekapselte Kommando zurück (siehe SPX42CommandDef.hpp)
    QByteArray getParamAt( int idx );    //! gibt den Parameter Nummer idx als QByteArra zurück ( 0..x )
    uint getValueFromHexAt( int idx );   //! gibt den int wert des Parameters zurück aus HEX Source
    uint getValueAt( int idx );          //! gibt den Wert eines parameters zurück
    double getDoubleValueAt( int idx );  //! gib für logdetails den Wert als double
    int getTag( void );                  //! gib den Tsag hzurück (bei logdetail die Tauchgangsnummer)
    int getSequence( void );             //! Sequenmznumer
  };
}
#endif  // SPX42SINGLECOMMAND_HPP
