#include "PlayerPawn.h"

#include "Particles/ParticleSystemComponent.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerMeshComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	PlayerMeshComponent->SetCollisionObjectType(ECC_Pawn);
	SetRootComponent(PlayerMeshComponent);
	ConeComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeLaser"));
	ConeComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ConeComponent->SetupAttachment(RootComponent);
	ConeComponent->AddRelativeLocation(FVector(1300.f,0.f,0.f));
	ConeComponent->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ConeComponent->SetRelativeScale3D(FVector(0.5f,0.5f,26.f));
	StartPositionComponent=CreateDefaultSubobject<USphereComponent>(TEXT("StartBallPosition"));
	StartPositionComponent->InitSphereRadius(5);
	StartPositionComponent->SetupAttachment(RootComponent);
	StartPositionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StartPositionComponent->AddRelativeLocation(FVector(60.f,0.f,0.f));
	StartPositionComponent->SetUsingAbsoluteScale(true);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	ConeComponent->SetVisibility(false);
	SpawnBall();
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetInputAxisValue(TEXT("Move")) !=0)
		AddActorWorldOffset(FVector((0.f),(DeltaTime*GetInputAxisValue(TEXT("Move"))*1000.f), 0.f),true, &Result, ETeleportType::None);
	if(bMagnetoActive)
	{
		FVector MagnitLocation = RootComponent->GetSocketLocation(FName("BallSocket"));
		if(CurrentBall)
		{
			CurrentBall->BallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FVector BallLocation = CurrentBall->GetActorLocation();
			FVector MoveToPlatformVector = MagnitLocation-BallLocation;
			CurrentBall->MoveDirection = MoveToPlatformVector;
			CurrentBall->MoveDirection.Normalize();
			if(FMath::Abs(FVector::Distance(BallLocation, MagnitLocation)) <= 10.f)
			{
				bMagnetoActive=false;
				CurrentBall->BallMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CurrentBall->MoveDirection = FVector((CurrentBall->Speed*(-1)),0.f,0.f);
			}
		}
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("LaunchBall"), IE_Pressed,this, &APlayerPawn::LaunchBall);
	PlayerInputComponent->BindAxis(TEXT("Move"));
}

void APlayerPawn::ChangeLifes(int Live)
{
	if((RemainingLife + Live) > 0)
	{
		RemainingLife = RemainingLife + Live;
		ChangeLife.Broadcast();
	}
	else
	{
		RemainingLife = 0;
		ChangeLife.Broadcast();
		Looser.Broadcast();
	}
}

void APlayerPawn::RemoveLife()
{
	ChangeLifes(-1);
	if(RemainingLife>0)
		SpawnBall();
}

void APlayerPawn::Resize(FVector ScaleSize)
{
	SetActorScale3D(GetActorScale() * ScaleSize);
}

void APlayerPawn::SetSuperBall(float Time)
{
	if(GetWorld() && CurrentBall)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ATTEMPT SUPER BONUS"));
		if(!GetWorld()->GetTimerManager().IsTimerActive(SuperBallTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(SuperBallTimer, this, &APlayerPawn::OffSuperBall, Time, false, -1);
			CurrentBall->BallMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(1,0,0));
			CurrentBall->BallMeshComponent->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")),1);
			CurrentBall->FxComponent->SetTemplate(CurrentBall->SuperBallFX);
			CurrentBall->bIsSuperBall=true;
		}
		else
		{
			const float Remain = GetWorld()->GetTimerManager().GetTimerRemaining(SuperBallTimer);
			GetWorld()->GetTimerManager().SetTimer(SuperBallTimer, this, &APlayerPawn::OffSuperBall, Time+Remain, false, -1);
		}
	}
}

void APlayerPawn::ControlBall(float Time)
{
	if(GetWorld() && CurrentBall)
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(LockBallTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(LockBallTimer, this, &APlayerPawn::OffControlBall, Time, false, -1);
			BallMoveToPlatform();
		}
		else
		{
			const float Remain = GetWorld()->GetTimerManager().GetTimerRemaining(LockBallTimer);
			GetWorld()->GetTimerManager().SetTimer(LockBallTimer, this, &APlayerPawn::OffControlBall, Time+Remain, false, -1);
		}
	}
}

void APlayerPawn::BallMoveToPlatform()
{
	bMagnetoActive = true;
	FAttachmentTransformRules Rule(EAttachmentRule::KeepWorld, false);
	CurrentBall->AttachToComponent(RootComponent, Rule, FName("BallSocket"));
	CurrentBall->BallMeshComponent->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(0,0,1));
	CurrentBall->BallMeshComponent->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")),30);
	ConeComponent->SetVisibility(true);
	CurrentBall->bBallIsFree=false;
}

void APlayerPawn::OffSuperBall()
{
	if(CurrentBall)
	{
		CurrentBall->ResetToBaseColor();
		CurrentBall->FxComponent->SetTemplate(nullptr);
		CurrentBall->bIsSuperBall=false;
	}
}

void APlayerPawn::OffControlBall()
{
	if(CurrentBall)
	{
		ConeComponent->SetVisibility(false);
		CurrentBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentBall->bBallIsFree=true;
		CurrentBall->ResetToBaseColor();
	}
}

void APlayerPawn::ChangeSpeedBall(float Coefficient)
{
	if(CurrentBall)
		CurrentBall->Speed = CurrentBall->Speed*Coefficient;
}

void APlayerPawn::SpawnBall()
{
	FVector SpawnLocation = RootComponent->GetSocketLocation(FName("BallSocket"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	FTransform SpawnTransform = FTransform(SpawnLocation);
	APlayerBall* MyBall = Cast<APlayerBall>(GetWorld()->SpawnActorDeferred<APlayerBall>(PlayerBall, SpawnTransform, SpawnParams.Owner, SpawnParams.Instigator, SpawnParams.SpawnCollisionHandlingOverride));
	CurrentBall = MyBall;
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	CurrentBall->AttachToComponent(RootComponent, Rule, FName("BallSocket"));
	UGameplayStatics::FinishSpawningActor(MyBall,SpawnTransform);
}

void APlayerPawn::SpawnBonusBall()
{
	UGameplayStatics::PlaySound2D(this,SFXLaunchBall);
	FVector SpawnLocation = RootComponent->GetSocketLocation(FName("BallSocket"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	FTransform SpawnTransform = FTransform(SpawnLocation);
	APlayerBall* MyBonusBall = Cast<APlayerBall>(GetWorld()->SpawnActorDeferred<APlayerBall>(PlayerBall, SpawnTransform, SpawnParams.Owner, SpawnParams.Instigator, SpawnParams.SpawnCollisionHandlingOverride));
	MyBonusBall->bIsBonusBall = true;
	MyBonusBall->bIsLaunched = true;
	MyBonusBall->MoveDirection = FVector(1.f,0.1f,0.f);
	MyBonusBall->HitBlock.AddDynamic(this,&APlayerPawn::ChangeScore);
	UGameplayStatics::FinishSpawningActor(MyBonusBall, SpawnTransform);
	MyBonusBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void APlayerPawn::ChangeScore(int Score)
{
	PlayerScore += Score;
	ScoreChanged.Broadcast(PlayerScore);
}

void APlayerPawn::LaunchBall()
{
	if(CurrentBall && !(CurrentBall->bIsLaunched))
	{
		UGameplayStatics::PlaySound2D(this,SFXLaunchBall);
		CurrentBall->MoveDirection = FVector(1.f,0.1f,0.f);
		CurrentBall->BallDestroyed.AddDynamic(this,&APlayerPawn::RemoveLife);
		CurrentBall->HitBlock.AddDynamic(this,&APlayerPawn::ChangeScore);
		CurrentBall->bIsLaunched = true;
		CurrentBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

APlayerBall* APlayerPawn::GetMyBall()
{
	return CurrentBall;
}