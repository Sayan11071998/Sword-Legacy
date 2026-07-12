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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchTarget, "InputTag.SwitchTarget");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Pickup_Stones, "InputTag.Pickup.Stones");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Light, "InputTag.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Heavy, "InputTag.SpecialWeaponAbility.Heavy");
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Katana, "InputTag.LightAttack.Katana");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Katana, "InputTag.HeavyAttack.Katana");
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld, "InputTag.MustBeHeld");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld_Block, "InputTag.MustBeHeld.Block");
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable, "InputTag.Toggleable");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_TargetLock, "InputTag.Toggleable.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_Rage, "InputTag.Toggleable.Rage");
	
	/** Player Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Katana, "Player.Ability.Equip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Katana, "Player.Ability.Unequip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll, "Player.Ability.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Block, "Player.Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_TargetLock, "Player.Ability.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Rage, "Player.Ability.Rage");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Light_Katana, "Player.Ability.Attack.Light.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Heavy_Katana, "Player.Ability.Attack.Heavy.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Light, "Player.Ability.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Heavy, "Player.Ability.SpecialWeaponAbility.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Pickup_Stones, "Player.Ability.Pickup.Stones");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Light, "Player.Cooldown.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Heavy, "Player.Cooldown.SpecialWeaponAbility.Heavy");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Katana, "Player.Weapon.Katana");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Katana, "Player.Event.Equip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Katana, "Player.Event.Unequip.Katana");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SuccessfulBlock, "Player.Event.SuccessfulBlock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_ActivateRage, "Player.Event.ActivateRage");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Left, "Player.Event.SwitchTarget.Left");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Right, "Player.Event.SwitchTarget.Right");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_AOE, "Player.Event.AOE");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_ConsumeStones, "Player.Event.ConsumeStones");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rolling, "Player.Status.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Blocking, "Player.Status.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_TargetLock, "Player.Status.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Activating, "Player.Status.Rage.Activating");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Active, "Player.Status.Rage.Active");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Full, "Player.Status.Rage.Full");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_None, "Player.Status.Rage.None");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy");
	
	/** Enemy Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SummonEnemies, "Enemy.Ability.SummonEnemies");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SpawnStone, "Enemy.Ability.SpawnStone");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon, "Enemy.Weapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_SummonEnemies, "Enemy.Event.SummonEnemies");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Unblockable, "Enemy.Status.Unblockable");
	
	/** Shared Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnProjectile, "Shared.Event.SpawnProjectile");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Death, "Shared.Status.Death");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Invincible, "Shared.Status.Invincible");
	
	/** Gameplay Cues **/
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_MeleeHit_Katana, "GameplayCue.Sounds.MeleeHit.Katana");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_MeleeHit_EnemyMeleeWeapon, "GameplayCue.Sounds.MeleeHit.EnemyMeleeWeapon");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_MeleeHit_BossEnemy, "GameplayCue.Sounds.MeleeHit.BossEnemy");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_Death_Enemy_Serath, "GameplayCue.Sounds.Death.Enemy.Serath");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_Death_Enemy_Phase, "GameplayCue.Sounds.Death.Enemy.Phase");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Sounds_Death_Enemy_FrostGiant, "GameplayCue.Sounds.Death.Enemy.FrostGiant");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects, "GameplayCue.Effects");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects_UnblockableWarning, "GameplayCue.Effects.UnblockableWarning");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects_Rage_Activating, "GameplayCue.Effects.Rage.Activating");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects_Katana, "GameplayCue.Effects.Katana");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects_Katana_SuccessfulBlock, "GameplayCue.Effects.Katana.SuccessfulBlock");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effects_Katana_PerfectBlock, "GameplayCue.Effects.Katana.PerfectBlock");
	
	/** UI Widget Stack **/
	UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_Modal, "UI.WidgetStack.Modal");
	UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_GameMenu, "UI.WidgetStack.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_GameHUD, "UI.WidgetStack.GameHUD");
	UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_Frontend, "UI.WidgetStack.Frontend");
}