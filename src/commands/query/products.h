/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/
#ifndef ZYPPER_COMMANDS_QUERY_PRODUCTS_INCLUDED
#define ZYPPER_COMMANDS_QUERY_PRODUCTS_INCLUDED

#include "commands/basecommand.h"
#include "commands/optionsets.h"
#include "utils/flags/zyppflags.h"

#include "src/search.h"
#include "miscqueryinit.h"

class ProductsCmdBase : public ZypperBaseCommand
{
public:
  ProductsCmdBase( std::vector<std::string> &&commandAliases_r );

private:
  InitReposOptionSet _initRepoFlags { *this };
  NotInstalledOnlyOptionSet _instFilterFlags { *this };
  std::vector<std::string> _xmlFwdTags;

  // ZypperBaseCommand interface
protected:
  zypp::ZyppFlags::CommandGroup cmdOptions() const override;
  void doReset() override;

  int execute(Zypper &zypper, const std::vector<std::string> &positionalArgs_r) override;
};

using ProductsCmd = MiscQueryInitMixin<ProductsCmdBase>;

#endif
