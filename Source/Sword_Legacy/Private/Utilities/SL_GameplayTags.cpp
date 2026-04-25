#include "Utilities/SL_GameplayTags.h"

namespace SL_GameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipKatana, "InputTag.EquipKatana");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipKatana, "InputTag.UnequipKatana");
	
	/** Player Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Katana, "Player.Weapon.Katana");
}