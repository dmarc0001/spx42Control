#ifndef SPX42DATABASECONSTANTS_HPP
#define SPX42DATABASECONSTANTS_HPP

#include <QString>

namespace spx
{
  class SPX42DatabaseConstants
  {
    public:
    static const QString createVersionTable;
    static const QString createDeviceTable;
    static const QString createDiveLogDirectoryTable;
    static const QString createDiveLogTableIndex01;
    static const QString createDiveLogTableIndex02;
    static const QString createDiveLogDetailsTable;
    static const QString createDiveLogDetailsTableIndex01;
    static const QString createDiveLogDetailsTableIndex02;
    //
    static const QString loglineInsertTemplate;
  };
}  // namespace spx
#endif  // SPX42DATABASECONSTANTS_HPP
