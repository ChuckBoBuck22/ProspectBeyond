#pragma once

UENUM(BlueprintType)
enum class EPBFaction : uint8
{
	EF_wildphire UMETA(DisplayName = "wildphire"),
	EF_AlphaAspect UMETA(DisplayName = "AlphaAspect"),
	EF_proclout UMETA(DisplayName = "proclout"),
	EF_SenseDynamic UMETA(DisplayName = "SenseDynamic"),
	EF_SportingPower UMETA(DisplayName = "SportingPower"),
	EF_Protectorate UMETA(DisplayName = "Protectorate"),
	EF_NoTeam UMETA(DisplayName = "NoTeam"),

	EF_MAX UMETA(DisplayName = "DefaultMax")

};