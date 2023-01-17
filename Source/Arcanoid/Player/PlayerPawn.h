#pragma once

#include "CoreMinimal.h"
#include "PlayerBall.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLife);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndLife);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged,int,Score);

UCLASS()
class ARCANOID_API APlayerPawn : public APawn, public IInterdelete
{
	GENERATED_BODY()

public:
	APlayerPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="PlayerSettings")
	UStaticMeshComponent* PlayerMeshComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="PlayerSettings")
	UStaticMeshComponent* ConeComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="PlayerSettings")
	USphereComponent* StartPositionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="PlayerSettings")
	USoundCue* SFXLaunchBall = nullptr;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="PlayerSettings")
	FOnChangeLife ChangeLife;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="PlayerSettings")
	FOnScoreChanged ScoreChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="PlayerSettings")
	FOnEndLife Looser;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="PlayerSettings")
	TSubclassOf<APlayerBall> PlayerBall;

	UPROPERTY()
	APlayerBall* CurrentBall = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PlayerSettings")
	int RemainingLife = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn),Category="PlayerSettings")
	int PlayerScore = 0;
	
	UFUNCTION()
	void LaunchBall();
	UFUNCTION()
	void ChangeLifes(int Live);
	UFUNCTION()
	void SpawnBall();
	UFUNCTION()
	void SpawnBonusBall();
	UFUNCTION()
	void ChangeScore(int Score);
	UFUNCTION()
	void ChangeSpeedBall(float Coefficient);
	UFUNCTION()
	void RemoveLife();
	
	void Resize(FVector ScaleSize);
	void SetSuperBall(float Time);
	void OffSuperBall();
	void ControlBall(float Time);
	void BallMoveToPlatform();
	void OffControlBall();

	APlayerBall* GetMyBall();

	bool bBallIsSuper = false;
	FTimerHandle SuperBallTimer;
	FTimerHandle LockBallTimer;
	FHitResult Result;
	bool bMagnetoActive = false;
	bool bStart = false;	
	float LockBallTime = 0.f;
};