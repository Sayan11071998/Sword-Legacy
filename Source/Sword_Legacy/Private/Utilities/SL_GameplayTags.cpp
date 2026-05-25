#include "Utilities/SL_GameplayTags.h"

namespace SL_GameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Roll, "InputTag.Roll");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipKatana, "InputTag.EquipKatana");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipKatana, "InputTag.UnequipKatana");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Katana, "InputTag.LightAttack.Katana");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Katana, "InputTag.HeavyAttack.Katana");
	
	/** Player Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Katana, "Player.Ability.Equip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Katana, "Player.Ability.Unequip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll, "Player.Ability.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Light_Katana, "Player.Ability.Attack.Light.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Heavy_Katana, "Player.Ability.Attack.Heavy.Katana");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Katana, "Player.Weapon.Katana");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Katana, "Player.Event.Equip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Katana, "Player.Event.Unequip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rolling, "Player.Status.Rolling");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy");
	
	/** Enemy Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon, "Enemy.Weapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");
	
	/** Shared Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Death, "Shared.Status.Death");
	
	/** Gameplay Cues **/
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_MeleeHit_Katana, "GameplayCue.Sounds.MeleeHit.Katana");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_MeleeHit_EnemyMeleeWeapon, "GameplayCue.Sounds.MeleeHit.EnemyMeleeWeapon");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_Death_Enemy_Serath, "GameplayCue.Sounds.Death.Enemy.Serath");
}