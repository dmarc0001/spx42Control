#ifndef SPX42SINGLECOMMAND_HPP
#define SPX42SINGLECOMMAND_HPP

#include <QByteArray>
#include <QList>
#include <limits>
#include <memory>
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
    const int diveNum;
    const int sequence;

    public:
    //! Konstruktor
    SPX42SingleCommand( char cmd, const QList< QByteArray > &params, int diveNum = 0, int seq = 0 );
    //! gib das gekapselte Kommando zurück (siehe SPX42CommandDef.hpp)
    char getCommand( void );
    //! gibt den Parameter Nummer idx als QByteArra zurück ( 0..x )
    QByteArray getParamAt( int idx );
    //! gibt den int wert des Parameters zurück aus HEX Source
    uint getValueFromHexAt( int idx );
    //! gibt den Wert eines parameters zurück
    uint getValueAt( int idx );
    //! gib für logdetails den Wert als double
    double getDoubleValueAt( int idx );
    //! gib den Tsag hzurück (bei logdetail die Tauchgangsnummer)
    int getDiveNum( void );
    //! Sequenmznumer innerhalb des Tauchgangslogs
    int getSequence( void );
  };
}  // namespace spx
#endif  // SPX42SINGLECOMMAND_HPP
