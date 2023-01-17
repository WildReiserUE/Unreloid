// Created with UE4.27.2 WildReiser

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARCGameModeBase.generated.h"

UCLASS()
class ARCANOID_API AARCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AARCGameModeBase();
	
protected:
	virtual void BeginPlay() override;
};
