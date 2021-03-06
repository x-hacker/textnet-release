#ifndef TEXTNET_UPDATER_UPDATER_H_
#define TEXTNET_UPDATER_UPDATER_H_

#include <vector>
#include <map>
#include <string>
#include <mshadow/tensor.h>
#include "../global.h"
#include "../utils/utils.h"
#include "../utils/io.h"
#include "../utils/settingv.h"

/*! \brief namespace of textnet */
namespace textnet {
/*! \brief namespace of layer defintiion */
namespace updater {

/*! \brief use integer to encode layer types */
typedef int UpdaterType;

/*! \brief these are enumeration */
const int kSGD = 0;
const int kAdagrad = 1;
const int kAdam = 2;
const int kSGDSparse = 3;
const int kAdaDelta = 4;
const int kSGDStep = 5;

template<typename xpu, int dim>
class Updater {
 public:
  Updater(void) {}
  virtual ~Updater(void) {}
  
  // To implement this function you need call base function in the end
  virtual void Require(std::map<std::string, SettingV> &setting) {
    defaults["updater_type"] = SettingV(kSGD);
    for (std::map<std::string, SettingV>::iterator it = defaults.begin();
          it != defaults.end(); ++it) {
      std::string name = it->first;
      if (defaults[name].value_type == SET_NONE) {
        utils::Check(setting.count(name), 
            "\tSetting [%s] needed for this layer.\n", name.c_str());
      } else {
        if (!setting.count(name)) {
          setting[name] = defaults[name];
          utils::Printf("\tSetting [%s] set to default value.\n", name.c_str());
        }
      }
    }
  }
  
  virtual void SetupUpdater(std::map<std::string, SettingV> &setting) {
    updater_type = 0;
    this->Require(setting);
  }
  
  virtual void Update(mshadow::Tensor<xpu, dim> data, 
                      mshadow::Tensor<xpu, dim> diff) = 0;
  
  virtual void UpdateSparse(mshadow::Tensor<xpu, dim> data, 
                            mshadow::Tensor<xpu, dim> diff, 
                            mshadow::Tensor<xpu, 1> idx) = 0;
                            
  
  virtual UpdaterType GetUpdaterType() { return updater_type; }
  
 protected:
  UpdaterType updater_type;
  mshadow::Random<xpu>* prnd_;
  // required setting
  std::map<std::string, SettingV> defaults;
  
};

template<typename xpu, int dim>
Updater<xpu, dim>* CreateUpdater(UpdaterType type, std::map<std::string, SettingV> &setting, 
                      mshadow::Random<xpu>* prnd);

}  // namespace updater
}  // namespace textnet
#endif  // TEXTNET_UPDATER_UPDATER_H_
