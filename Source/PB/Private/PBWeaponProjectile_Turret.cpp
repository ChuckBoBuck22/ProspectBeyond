// Fill out your copyright notice in the Description page of Project Settings.


#include "PBWeaponProjectile_Turret.h"
#include "Kismet/GameplayStatics.h"
#include "PBTurret.h"

float APBWeaponProjectile_Turret::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (GetLocalRole() == ROLE_Authority)
	{
		UGameplayStatics::ApplyDamage(GetOwner(), Damage, EventInstigator, DamageCauser, DamageEvent.DamageTypeClass);
		return 0.0f;
	}
	return 0.0f;
}
