#include "main.hpp"
#include "ModConfig.hpp"
#include "SettingsViewController.hpp"

#include "questui/shared/QuestUI.hpp"

#include "GlobalNamespace/LevelListTableCell.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#include "HMUI/ImageView.hpp"

#include "TMPro/TMP_Text.hpp"

#include "Polyglot/LocalizedTextMeshProUGUI.hpp"

#include "song-details/shared/SongDetails.hpp"

static ModInfo modInfo;

SongDetailsCache::SongDetails* songDetails;

enum class RankedStatus {
    Beatleader,
    Scoresaber,
    Ranked,
    None
};

UnityEngine::Color getColor(RankedStatus status)
{
    switch(status)
    {
        case RankedStatus::Beatleader:
            return UnityEngine::Color(0.54509803921f, 0.38823529411f, 0.73333333333f, 1);
        case RankedStatus::Scoresaber:
            return UnityEngine::Color(0.9294117647f, 0.8f, 0.03137254901f, 1);
        default:
            break;
    }
    return UnityEngine::Color(0.87450980392f, 0.0862745098f, 0.43529411764f, 1);
}

StringW getText(RankedStatus status)
{
    switch(status)
    {
        case RankedStatus::Ranked:
            return "Ranked";
        case RankedStatus::Beatleader:
            return "BL Ranked";
        case RankedStatus::Scoresaber:
            return "SS Ranked";
        default:
            break;
    }
    // default cell text
    return "new";
}

RankedStatus GetRankedStatus(std::string hash)
{
    const SongDetailsCache::Song* song;
    if(!songDetails->songs.FindByHash(static_cast<std::string>(hash), song))
        return RankedStatus::None;

    using Ranked = SongDetailsCache::RankedStates;
    auto& states = song->rankedStates;

    if(SongDetailsCache::hasFlags(states, Ranked::BeatleaderRanked) && SongDetailsCache::hasFlags(states, Ranked::ScoresaberRanked))
        return RankedStatus::Ranked;
    if(SongDetailsCache::hasFlags(states, Ranked::BeatleaderRanked))
        return RankedStatus::Beatleader;
    if(SongDetailsCache::hasFlags(states, Ranked::ScoresaberRanked))
        return RankedStatus::Scoresaber;

    return RankedStatus::None;
}

MAKE_HOOK_MATCH(LevelListTableCell_SetDataFromLevelAsync, &GlobalNamespace::LevelListTableCell::SetDataFromLevelAsync, void, GlobalNamespace::LevelListTableCell* self, GlobalNamespace::IPreviewBeatmapLevel* level, bool isFavorite, bool isPromoted, bool isUpdated)
{
    LevelListTableCell_SetDataFromLevelAsync(self, level, isFavorite, isPromoted, isUpdated);


    RankedStatus rankedStatus = RankedStatus::None;

    if(level->get_levelID()->StartsWith("custom_level"))
    {
        StringW hash = level->get_levelID()->Substring(13);
        rankedStatus = GetRankedStatus(static_cast<std::string>(hash));
    }

    bool isRanked = rankedStatus != RankedStatus::None;
    
    self->promoBadgeGo->SetActive((isRanked && getModConfig().Enabled.GetValue()) || isPromoted);

    auto promoTextGo = self->promoBadgeGo->get_transform()->Find("PromoText")->get_gameObject();
    auto localization = promoTextGo->GetComponent<Polyglot::LocalizedTextMeshProUGUI*>();
    localization->set_enabled(!isRanked);

    auto promoText = promoTextGo->GetComponent<TMPro::TMP_Text*>();
    auto promoTextBg = self->promoBadgeGo->GetComponent<HMUI::ImageView*>();

    if(isRanked && getModConfig().Enabled.GetValue())
    {
        promoText->SetText(getModConfig().DifferentText.GetValue() ? getText(rankedStatus) : getText(RankedStatus::Ranked));
        promoTextBg->set_color(getModConfig().DifferentText.GetValue() ? getColor(rankedStatus) : getColor(RankedStatus::Ranked));
    }
    // fix issues with reused cells
    else
    {
        promoText->SetText(getText(RankedStatus::None));
        promoTextBg->set_color(getColor(RankedStatus::None));
    }
}

Logger &getLogger()
{
    static Logger *logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info)
{
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load()
{
    il2cpp_functions::Init();

    getModConfig().Init(modInfo);

    songDetails = SongDetailsCache::SongDetails::Init(0).get();

    QuestUI::Init();
    QuestUI::Register::RegisterAllModSettingsViewController<SongRankedBadge::SettingsViewController*>(modInfo);

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), LevelListTableCell_SetDataFromLevelAsync);
    getLogger().info("Installed all hooks!");
}