#pragma once

#include "CoreMinimal.h"
#include "Bonus.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKilled);

UCLASS()
class ARCANOID_API ABlock : public AActor, public IInterdelete
{
	GENERATED_BODY()
	
public:
	ABlock();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void Attack();
	void PickUpColor();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* BlockMesh = nullptr;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnKilled OnKilled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	UStaticMesh* NewBlockMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Block Settings")
	UMaterialInstance* BlockMaterialNormal = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Block Settings")
	UParticleSystem* DestroyFx= nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Block Settings")
	USoundCue* HitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Block Settings")
	USoundCue* DestroySound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Block Settings")
	TSubclassOf<ABonus> BonusBall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ClampMin=0,ClampMax=20))
	float BlockEmissiveValue = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bCanChangeMaterial = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bResizable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bCanSpawnBlock = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bIsUndestruktable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bCanMoving = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	bool bCanAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	float AttackTick = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn))
	FVector StartMoveDirection = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn,ClampMin=-1,ClampMax=10)); // NOTE: Change ClampMax and MaxHits to!
	int ContactToDestroy = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Settings", meta=(ExposeOnSpawn,ClampMin=0.5,ClampMax=1));
	float ScaleHit = 1;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Block Settings")
	FColor StartColor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Block Settings")
	FColor EndColor;
	
	// Use in CPP Math
	FHitResult ResultHit;
	FTimerHandle AttackTimer;
	float MaxHits = 10.f;
	float MovingSpeed = 100.f;
	int CalculatedPoint = 0;
	int HitPoint = 5;

	UFUNCTION()
	void OnBlockHit(int Hits);
	void SpawnBonus();
};