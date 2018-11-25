#ifndef SPX42SINGLECOMMAND_HPP
#define SPX42SINGLECOMMAND_HPP

#include <QByteArray>
#include <QList>
#include <limits>
#include "SPX42CommandDef.hpp"

namespace spx
{
  /**
   * @brief The SPX42SingleCommand class Kapselt eine Antwort vom SPX42
   */
  class SPX42SingleCommand
  {
    private:
    const char command;
    const QList< QByteArray > params;

    public:
    SPX42SingleCommand( char cmd, const QList< QByteArray > &params );  //! Konstruktor
    char getCommand( void );           //! gib das gekapselte Kommando zurück (siehe SPX42CommandDef.hpp)
    QByteArray getParamAt( int idx );  //! gibt den Parameter Nummer idx als QByteArra zurück ( 0..x )
    uint getValueAt( int idx );        //! gibt den int wert des Parameters zurück
  };
}
#endif  // SPX42SINGLECOMMAND_HPP
