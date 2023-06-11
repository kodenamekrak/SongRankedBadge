#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(Enabled, bool, "Enabled", true);
    CONFIG_VALUE(DifferentText, bool, "Different Text", true);
    CONFIG_VALUE(DifferentColor, bool, "Different Color", true);
)