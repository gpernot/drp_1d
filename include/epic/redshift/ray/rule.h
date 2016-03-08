#ifndef _REDSHIFT_RAY_RULE_
#define _REDSHIFT_RAY_RULE_

#include <epic/core/common/datatypes.h>
#include <boost/format.hpp>
#include <epic/redshift/linemodel/elementlist.h>

namespace NSEpic
{
  /**
   * \ingroup Redshift
   * Abstract class for common functionality of rules.
   */
  class CRule
  {
  public:
    Bool Enabled;
    std::string Name;
    
    CRule ( );
    void Apply( CLineModelElementList& LineModelElementList );
    virtual Bool Check( CLineModelElementList& LineModelElementList ) = 0;
    virtual void SetUp( Bool EnabledArgument, ... ) = 0;
  private:
    virtual void Correct( CLineModelElementList& LineModelElementList ) = 0;
  protected:
    // Library of common methods:
  };
}

#endif