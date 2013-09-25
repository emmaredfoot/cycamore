// manager_inst.cc
// Implements the ManagerInst class

#include "manager_inst.h"
#include "logger.h"

namespace cycamore {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ManagerInst::ManagerInst(cyclus::Context* ctx) : cyclus::InstModel(ctx) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ManagerInst::~ManagerInst() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ManagerInst::RegisterAvailablePrototype(std::string prototype) {
  std::cout << name() << "is registering\n";
  using cyclus::supply_demand::CommodityProducer;
  try {
    CommodityProducer* cast = context()->CreateModel<CommodityProducer>(prototype);
    cyclus::action_building::Builder::RegisterProducer(cast);
    LOG(cyclus::LEV_DEBUG3, "maninst") << "ManagerInst " << name()
                                       << " has registered a producer prototype: "
                                       << prototype
                                       << " and "
                                       << " now has " << NBuildingPrototypes()
                                       << " registered total.";
  } catch (cyclus::Error err) {
    std::cout << "registration failed: " << err.what() << "\n";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ManagerInst::RegisterCloneAsBuilt(cyclus::Model* clone) {
  cyclus::supply_demand::CommodityProducer* cast =
    dynamic_cast<cyclus::supply_demand::CommodityProducer*>(clone);
  if (cast) {
    cyclus::supply_demand::CommodityProducerManager::RegisterProducer(cast);
    if (cyclus::LEV_DEBUG3 >= cyclus::Logger::ReportLevel()) {
      LOG(cyclus::LEV_DEBUG3, "maninst") << "ManagerInst " << name()
                                         << " has registered a producer clone:";
      WriteProducerInformation(cast);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ManagerInst::RegisterCloneAsDecommissioned(cyclus::Model* clone) {
  cyclus::supply_demand::CommodityProducer* cast =
    dynamic_cast<cyclus::supply_demand::CommodityProducer*>(clone);
  if (cast) {
    cyclus::supply_demand::CommodityProducerManager::UnRegisterProducer(cast);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ManagerInst::WriteProducerInformation(
  cyclus::supply_demand::CommodityProducer* producer) {
  using std::set;
  set<cyclus::Commodity, cyclus::CommodityCompare> commodities =
    producer->ProducedCommodities();
  set<cyclus::Commodity, cyclus::CommodityCompare>::iterator it;

  LOG(cyclus::LEV_DEBUG3, "maninst") << " Clone produces " << commodities.size()
                                     << " commodities.";
  for (it = commodities.begin(); it != commodities.end(); it++) {
    LOG(cyclus::LEV_DEBUG3, "maninst") << " Commodity produced: " << it->name();
    LOG(cyclus::LEV_DEBUG3, "maninst") << "           capacity: " <<
                                       producer->ProductionCapacity(*it);
    LOG(cyclus::LEV_DEBUG3, "maninst") << "               cost: " <<
                                       producer->ProductionCost(*it);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructManagerInst(cyclus::Context* ctx) {
  return new ManagerInst(ctx);
}
} // namespace cycamore


