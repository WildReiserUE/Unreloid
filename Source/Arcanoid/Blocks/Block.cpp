#include "Block.h"

#include "Kismet/KismetMathLibrary.h"

int MulDiv(int v, int m, int d)
{
	return v * m / d;
}

FLinearColor GetGradientColor(FColor aStartColor, FColor aStopColor, int aCount, int aNum)
{
	FLinearColor C;
	C.R = (aStartColor.R + MulDiv(aNum, aStopColor.R - aStartColor.R, aCount))/255.f;
	C.G = (aStartColor.G + MulDiv(aNum, aStopColor.G - aStartColor.G, aCount))/255.f;
	C.B = (aStartColor.B + MulDiv(aNum, aStopColor.B - aStartColor.B, aCount))/255.f;
	C.A = 255;
	return C;
}

ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BlockMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	SetRootComponent(BlockMesh);
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockMesh->SetGenerateOverlapEvents(true);
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
	BlockMesh->SetStaticMesh(NewBlockMesh);
	BlockMesh->SetMaterial(0,BlockMaterialNormal);
	if(!bIsUndestruktable)
	{
		PickUpColor();
		CalculatedPoint = (ContactToDestroy + 1)*HitPoint;
	}
	else
	{
		CalculatedPoint = 100.f;
		BlockMesh->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")), FVector(0,0,0));
	}
	if(bCanAttack)
		GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &ABlock::Attack, AttackTick, true,10.f);
}

void ABlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bCanMoving)
	{
		AddActorWorldOffset(FVector(0.f,(DeltaSeconds*(MovingSpeed*StartMoveDirection.Y)),0.f),true, &ResultHit, ETeleportType::None);
		if(ResultHit.bBlockingHit){
			const FVector NewDirection = FMath::GetReflectionVector(FVector(0, StartMoveDirection.Y,0.f),FVector(0, ResultHit.Normal.Y,0.f));
			StartMoveDirection = NewDirection;
		}
	}
}

void ABlock::Attack()
{
	SpawnBonus();
}

void ABlock::PickUpColor()
{
	FLinearColor C;
	C = GetGradientColor(StartColor, EndColor, MaxHits, ContactToDestroy);
	FVector4 BlockColor = FVector4(C.R, C.G, C.B,C.A);
	BlockMesh->SetVectorParameterValueOnMaterials(FName (TEXT("BaseColor")),FVector(BlockColor));
	if(bCanSpawnBlock)
	 	BlockMesh->SetScalarParameterValueOnMaterials(FName (TEXT("Emissive")), BlockEmissiveValue);
}

void ABlock::OnBlockHit(int Hits)
{
	if((ContactToDestroy - Hits) >= 0)
	{
		ContactToDestroy -= 1;
		CalculatedPoint -= HitPoint;
		UGameplayStatics::SpawnSoundAtLocation(this,HitSound,GetActorLocation(),GetActorRotation(), 1.f, true);
		PickUpColor();
	}
	else
	{
		if(bCanSpawnBlock)
			SpawnBonus();
		if(GetWorld()->GetTimerManager().IsTimerActive(AttackTimer))
			GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
		UGameplayStatics::SpawnSoundAtLocation(this,HitSound,GetActorLocation(),GetActorRotation(), 1.f, true);
		UGameplayStatics::SpawnSoundAtLocation(this,DestroySound,GetActorLocation(),GetActorRotation(), 1.f, true);
		UGameplayStatics::SpawnEmitterAtLocation(this,DestroyFx,GetActorLocation(),GetActorRotation(),FVector(1), true);
		OnKilled.Broadcast();
		Destroy();
	}
}

void ABlock::SpawnBonus()
{
	TArray<FName> SocketList = BlockMesh->GetAllSocketNames();
	FVector SoketVector = FVector(0);
	if(SocketList.Num() == 0)
		SoketVector = GetActorLocation();
	else
	{
		int RndInt = UKismetMathLibrary::RandomIntegerInRange(0,SocketList.Num()-1);
		SoketVector = BlockMesh->GetSocketLocation(SocketList[RndInt]);
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	FTransform SpawnTransform = FTransform(SoketVector);
	ABonus* Bonus = Cast<ABonus>(GetWorld()->SpawnActorDeferred<ABonus>(BonusBall, SpawnTransform, SpawnParams.Owner, SpawnParams.Instigator, SpawnParams.SpawnCollisionHandlingOverride));
	UGameplayStatics::FinishSpawningActor(Bonus,SpawnTransform);
}