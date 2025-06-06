#include "baldr/streetnames_us.h"
#include "baldr/streetname.h"
#include "midgard/util.h"
#include "proto/common.pb.h"

#include <memory>
#include <optional>

namespace valhalla {
namespace baldr {

StreetNamesUs::StreetNamesUs() : StreetNames() {
}

StreetNamesUs::StreetNamesUs(const std::vector<std::pair<std::string, bool>>& names) {
  for (auto& name : names) {
    this->emplace_back(std::make_unique<StreetNameUs>(name.first, name.second, std::nullopt));
  }
}

StreetNamesUs::StreetNamesUs(const google::protobuf::RepeatedPtrField<valhalla::StreetName>& names) {
  for (auto& name : names) {
    std::optional<baldr::Pronunciation> pronunciation =
        name.has_pronunciation()
            ? std::make_optional(
                  baldr::Pronunciation{name.pronunciation().alphabet(), name.pronunciation().value()})
            : std::nullopt;

    this->emplace_back(
        std::make_unique<StreetNameUs>(name.value(), name.is_route_number(), pronunciation));
  }
}

StreetNamesUs::~StreetNamesUs() {
}

std::unique_ptr<StreetNames> StreetNamesUs::clone() const {
  std::unique_ptr<StreetNames> clone_street_names = std::make_unique<StreetNamesUs>();
  for (const auto& street_name : *this) {
    clone_street_names->emplace_back(std::make_unique<StreetNameUs>(street_name->value(),
                                                                    street_name->is_route_number(),
                                                                    street_name->pronunciation()));
  }

  return clone_street_names;
}

std::unique_ptr<StreetNames>
StreetNamesUs::FindCommonStreetNames(const StreetNames& other_street_names) const {
  std::unique_ptr<StreetNames> common_street_names = std::make_unique<StreetNamesUs>();
  for (const auto& street_name : *this) {
    for (const auto& other_street_name : other_street_names) {
      if (*street_name == *other_street_name) {
        common_street_names->emplace_back(
            std::make_unique<StreetNameUs>(street_name->value(), street_name->is_route_number(),
                                           street_name->pronunciation()));
        break;
      }
    }
  }

  return common_street_names;
}

std::unique_ptr<StreetNames>
StreetNamesUs::FindCommonBaseNames(const StreetNames& other_street_names) const {
  std::unique_ptr<StreetNames> common_base_names = std::make_unique<StreetNamesUs>();
  for (const auto& street_name : *this) {
    for (const auto& other_street_name : other_street_names) {
      if (street_name->HasSameBaseName(*other_street_name)) {
        // Use the name with the cardinal directional suffix
        // thus, 'US 30 West' will be used instead of 'US 30'
        if (!street_name->GetPostCardinalDir().empty()) {
          common_base_names->emplace_back(
              std::make_unique<StreetNameUs>(street_name->value(), street_name->is_route_number(),
                                             street_name->pronunciation()));
        } else if (!other_street_name->GetPostCardinalDir().empty()) {
          common_base_names->emplace_back(
              std::make_unique<StreetNameUs>(other_street_name->value(),
                                             other_street_name->is_route_number(),
                                             other_street_name->pronunciation()));
          // Use street_name by default
        } else {
          common_base_names->emplace_back(
              std::make_unique<StreetNameUs>(street_name->value(), street_name->is_route_number(),
                                             street_name->pronunciation()));
        }
        break;
      }
    }
  }

  return common_base_names;
}

std::unique_ptr<StreetNames> StreetNamesUs::GetRouteNumbers() const {
  std::unique_ptr<StreetNames> route_numbers = std::make_unique<StreetNamesUs>();
  for (const auto& street_name : *this) {
    if (street_name->is_route_number()) {
      route_numbers->emplace_back(std::make_unique<StreetNameUs>(street_name->value(),
                                                                 street_name->is_route_number(),
                                                                 street_name->pronunciation()));
    }
  }

  return route_numbers;
}

std::unique_ptr<StreetNames> StreetNamesUs::GetNonRouteNumbers() const {
  std::unique_ptr<StreetNames> non_route_numbers = std::make_unique<StreetNamesUs>();
  for (const auto& street_name : *this) {
    if (!street_name->is_route_number()) {
      non_route_numbers->emplace_back(std::make_unique<StreetNameUs>(street_name->value(),
                                                                     street_name->is_route_number(),
                                                                     street_name->pronunciation()));
    }
  }

  return non_route_numbers;
}

} // namespace baldr
} // namespace valhalla
