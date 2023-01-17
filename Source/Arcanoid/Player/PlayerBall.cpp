#include "PlayerBall.h"
#include "Block.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/KillZVolume.h"

APlayerBall::APlayerBall()
{
	PrimaryActorTick.bCanEverTick = true;
	BallMeshComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	FxComponent=CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BallFX"));
	SetRootComponent(BallMeshComponent);
	FxComponent->SetupAttachment(BallMeshComponent);
	BallMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BallMeshComponent->SetGenerateOverlapEvents(true);
	BallMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);
	BallMeshComponent->OnComponentBeginOverlap.AddDynamic(this,&APlayerBall::OnBallBeginOverlap);
}

void APlayerBall::BeginPlay()
{
	Super::BeginPlay();
	SpeedCoef = Speed * 0.005f;
	//bIsSuperBall = true;  // For tests
	if(bIsBonusBall)
	{
		if(PrepareSound != nullptr)
			UGameplayStatics::PlaySound2D(this, PrepareSound);
		BallMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(1,0,0));
		BallMeshComponent->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")),20);
		GetWorld()->GetTimerManager().SetTimer(TimerToDeadHandle,this, &APlayerBall::DestroySelf, 10.0f, false, -1);
	}
	else
	{
		ResetToBaseColor();
	}
}

void APlayerBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsLaunched)
	{
		if(!bBallIsFree) // Controlled  (NOT False (Default) = True)
		{
			AddActorWorldOffset(FVector((DeltaTime*(Speed*MoveDirection.X)),(DeltaTime*(Speed*MoveDirection.Y)),0),true, &Result, ETeleportType::None);
			const auto Target = Cast<ABlock>(Result.Actor);
			if(Target)
			{
				if(bIsSuperBall)
				{
					if(!Target->bIsUndestruktable)
						Target->OnBlockHit(Target->ContactToDestroy + 1);
					else
					 	Target->Destroy();
					HitBlock.Broadcast(Target->CalculatedPoint);
				}
				else
				{
					if(!Target->bIsUndestruktable)
					{
						Target->OnBlockHit(1);
						HitBlock.Broadcast(Target->HitPoint);
					}
					if(Result.bBlockingHit)
					{
						UGameplayStatics::PlaySound2D(this, HitSound);
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,NiagaraFX,Result.Location);
						const FVector NewDirection = FMath::GetReflectionVector(FVector(MoveDirection.X, 0,0.f),FVector(Result.Normal.X, 0,0.f));
						MoveDirection = NewDirection;
						MoveDirection.Normalize();
					}
				}
			}
			else
			{
				if(Result.bBlockingHit)
				{
					UGameplayStatics::PlaySound2D(this, HitSound);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,NiagaraFX,Result.Location);
					const FVector NewDirection = FMath::GetReflectionVector(FVector(MoveDirection.X, 0,0.f),FVector(Result.Normal.X, 0,0.f));
					MoveDirection = NewDirection;
					MoveDirection.Normalize();
				}
			}
		}
		else // move free
		{
			AddActorWorldOffset(FVector(DeltaTime*(Speed*MoveDirection)),true, &Result, ETeleportType::None);
			const auto Target = Cast<ABlock>(Result.Actor);
			if(Target)
			{
				if(bIsSuperBall)
				{
					if(!Target->bIsUndestruktable)
						Target->OnBlockHit(Target->ContactToDestroy + 1);
					else
						Target->Destroy();
					HitBlock.Broadcast(Target->CalculatedPoint);
				}
				else
				{
					if(!Target->bIsUndestruktable)
					{
						Target->OnBlockHit(1);
						HitBlock.Broadcast(Target->HitPoint);
					}
					if(Result.bBlockingHit)
					{
						UGameplayStatics::PlaySound2D(this, HitSound);
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,NiagaraFX,Result.Location);
						const FVector NewDirection = FMath::GetReflectionVector(FVector(MoveDirection.X, MoveDirection.Y,0.f),FVector(Result.Normal.X, Result.Normal.Y,0.f));
						MoveDirection = NewDirection;
						MoveDirection.Normalize();
						Speed = Speed + SpeedCoef;
						if(Speed>=MaxSpeed)
							Speed = MaxSpeed;
					}
				}
			}
			else
			{
				if(Result.bBlockingHit)
				{
					UGameplayStatics::PlaySound2D(this, HitSound);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,NiagaraFX,Result.Location);
					const FVector NewDirection = FMath::GetReflectionVector(FVector(MoveDirection.X, MoveDirection.Y,0.f),FVector(Result.Normal.X, Result.Normal.Y,0.f));
					MoveDirection = NewDirection;
					MoveDirection.Normalize();
					Speed = Speed + SpeedCoef;
					if(Speed>=MaxSpeed)
						Speed = MaxSpeed;
				}
			}
		}
	}
}

void APlayerBall::OnBallBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Killer = Cast<AKillZVolume>(OtherActor);
	if(Killer)
	{
		UGameplayStatics::PlaySound2D(this, DeadSound);
		bIsLaunched = false;
		BallDestroyed.Broadcast();
		Destroy();
	}
}

void APlayerBall::DestroySelf()
{
	Destroy();
}

void APlayerBall::ResetToBaseColor()
{
	BallMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")), FVector(BallColor));
	BallMeshComponent->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")),0);
}