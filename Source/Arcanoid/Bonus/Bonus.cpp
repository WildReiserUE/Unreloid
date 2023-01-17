#include "Bonus.h"
#include "GameFramework/KillZVolume.h"
#include "Kismet/KismetMathLibrary.h"

ABonus::ABonus()
{
	PrimaryActorTick.bCanEverTick = true;
	BonusMeshComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BonusMesh"));
	BonusMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABonus::OnBonusBeginOverlap);
	BonusMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(BonusMeshComponent);
}

void ABonus::BeginPlay()
{
	Super::BeginPlay();
	constexpr int LastIndex = int(static_cast<EBonusType>(EBonusType::Last));
	BonusIndex = UKismetMathLibrary::RandomIntegerInRange(0,LastIndex);
	BonusMeshComponent->SetMaterial(0,BnsMaterial);
	BonusMeshComponent->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")),10);
	SelectBonus();
}

void ABonus::SelectBonus()
{
	switch ((EBonusType)GetBonusIndex())
	{
	case EBonusType::None:
		BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusScoreColor));
		RndScore = UKismetMathLibrary::RandomIntegerInRange(5,MaxBonusScore);
		break;

	case EBonusType::BonusLife:
		RndLife = UKismetMathLibrary::RandomIntegerInRange(-1,MaxAddLive);
		if(RndLife < 0)
			BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusBadColor));
		else if(RndLife == 0)
		{
			BonusIndex = 0;
			SelectBonus();
		}
		else
			BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusLifeColor));
		break;

	case EBonusType::BonusSize:
		RndSize = UKismetMathLibrary::RandomFloatInRange(MinPlatformSizeCoeff, MaxPlatformSizeCoeff);
		if(RndSize <=0 )
			BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusBadColor));
		else
			BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusSizeColor));
		break;

	case EBonusType::BonusSuper:
		BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusSuperColor));
		break;

	case EBonusType::BonusSpeed:
		BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusSpeedColor));
		RndSpeed = UKismetMathLibrary::RandomFloatInRange(0.8,1.2);
		break;

	case EBonusType::BonusBall:
		BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusBonusColor));
		break;

	case EBonusType::BonusLockBall:
		BonusMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BonusBonusColor));
		break;

	default:
		break;
	}
}

void ABonus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector BonusVector= FVector(DeltaTime*(Direction*Speed));
	AddActorWorldOffset(BonusVector,true);
}

int ABonus::GetBonusIndex()
{
	return BonusIndex;
}

void ABonus::OnBonusBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto KillZone = Cast<AKillZVolume>(OtherActor);
	auto Player = Cast<APlayerPawn>(OtherActor);
	if(KillZone)
	{
		Destroy();
	}
	else if(Player)
	{
		switch ((EBonusType)GetBonusIndex())
		{
		case EBonusType::None:
			Player->ChangeScore(RndScore);
			break;

		case EBonusType::BonusLife:	
			Player->ChangeLifes(RndLife);
			break;

		case EBonusType::BonusSize:
			Player->Resize(FVector(1,RndSize,1));
			break;

		case EBonusType::BonusSuper:
			if(Player->CurrentBall && Player->CurrentBall->bIsLaunched)
				Player->SetSuperBall(SuperTime);
			break;

		case EBonusType::BonusSpeed:
			if(Player->CurrentBall && Player->CurrentBall->bIsLaunched)
				Player->ChangeSpeedBall(RndSpeed);
			break;

		case EBonusType::BonusBall:
			Player->SpawnBonusBall();
			break;

		case EBonusType::BonusLockBall:
			if(Player->CurrentBall && Player->CurrentBall->bIsLaunched)
				Player->ControlBall(LockTime);
			break;
		default:break;
		}
		Destroy();
	}
}