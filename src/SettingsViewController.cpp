#include "main.hpp"
#include "SettingsViewController.hpp"
#include "ModConfig.hpp"

#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(SongRankedBadge, SettingsViewController);

void SongRankedBadge::SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    if(!firstActivation)
        return;

    auto container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform())->get_transform();

    AddConfigValueToggle(container, getModConfig().Enabled);
    AddConfigValueToggle(container, getModConfig().DifferentColor);
    AddConfigValueToggle(container, getModConfig().DifferentText);
}