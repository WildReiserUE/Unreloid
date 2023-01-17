#pragma once

#include "PlayerPawn.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bonus.generated.h"

UENUM(BlueprintType)
enum class EBonusType:uint8
{
	None,			// Empty Bonus just for collect score
	BonusLife,
	BonusSize,
	BonusBall,
	BonusSuper,
	BonusSpeed,
	BonusLockBall,
	Last			//do not delete - use for EnumSize
};

UCLASS()
class ARCANOID_API ABonus : public AActor, public IInterdelete
{
	GENERATED_BODY()

public:
	ABonus();

protected:
	virtual void BeginPlay() override;
	void SelectBonus();

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UStaticMeshComponent* BonusMeshComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	float Speed = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	EBonusType BonusType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	UMaterialInstance* BnsMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusScoreColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusLifeColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusSizeColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusBonusColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusSuperColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusSpeedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusBadColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	FColor BonusLockColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BonusSettings")
	int MaxAddLive = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BonusSettings")
	int MaxBonusScore = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BonusSettings")
	float MinPlatformSizeCoeff = 0.5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BonusSettings")
	float MaxPlatformSizeCoeff = 1.5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BonusSettings")
	float LockTime = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BonusSettings")
	float SuperTime = 10.0f;

	UFUNCTION()
	void OnBonusBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	int GetBonusIndex();
	FVector Direction = FVector (-1.0f,0.0f,0.0f);

	int BonusIndex;
	int RndLife;
	int RndScore;
	float RndSize;
	float RndSpeed;
};
