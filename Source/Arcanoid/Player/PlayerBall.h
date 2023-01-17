#pragma once

#include "CoreMinimal.h"
#include "Interdelete.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PlayerBall.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitBlock,int,Score);

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class ARCANOID_API APlayerBall : public AActor, public IInterdelete
{
	GENERATED_BODY()
	
public:	
	APlayerBall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* BallMeshComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UParticleSystemComponent* FxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BallSettings")
	UMaterialInstance* DefaultMaterial=nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BallSettings")
	UNiagaraSystem* NiagaraFX;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnDestroyed BallDestroyed;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnHitBlock HitBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BallSettings", meta=(ExposeOnSpawn))
	FVector MoveDirection = FVector (0.f,0.f,0.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	USoundCue* HitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	USoundCue* PrepareSound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	USoundCue* DeadSound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	FColor BallColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BallSettings")
	UParticleSystem* SuperBallFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BallSettings")
	float Speed = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	float SpeedCoef = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BallSettings")
	float MaxSpeed = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BallSettings")
	bool bIsBonusBall = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BallSettings")
	bool bIsLaunched = false;
	
	UFUNCTION()
	void OnBallBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void DestroySelf();
	void ResetToBaseColor();

	FHitResult Result;
	FTimerHandle TimerToDeadHandle;
	bool bMovingToPlatform = false;
	bool bIsSuperBall = false;
	bool bBallIsFree = true;
};
