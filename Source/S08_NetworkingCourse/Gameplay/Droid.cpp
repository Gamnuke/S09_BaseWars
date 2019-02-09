// Fill out your copyright notice in the Description page of Project Settings.

#include "Droid.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Classes/AIController.h "

#include "PlatformerGameInstance.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Gameplay/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h "
#include "Sound/SoundBase.h"
#include "PlayingGameMode.h"

// Sets default values
ADroid::ADroid()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);

	bReplicates = true;
}

void ADroid::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADroid, DroidState);

}
// Called when the game starts or when spawned
void ADroid::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = DroidSettings.Health;

	if (HasAuthority()) {
		TimeUntilStartRunning = GetWorld()->GetTimeSeconds() + FMath::RandRange(30, 100);
	}
}

// Called every frame
void ADroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && GetWorld()->GetTimeSeconds() > TimeUntilStartRunning)
	{
		DroidState.Sprinting = true;
	}

	LeadFlinchRotation = FMath::RInterpTo(LeadFlinchRotation, FRotator::ZeroRotator, DeltaTime, 5);
	CurrentFlinchRotation = FMath::RInterpTo(CurrentFlinchRotation, LeadFlinchRotation, DeltaTime, 5);

	if (DroidState.Sprinting) {
		GetCharacterMovement()->MaxWalkSpeed = DroidSettings.RunSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = DroidSettings.WalkSpeed;
	}

	if (DroidState.Attacking || GetVelocity() == FVector::ZeroVector) {
		if (DroidState.TargetCharacter != nullptr) {
			FVector Start = GetActorLocation();
			FVector End = DroidState.TargetCharacter->GetActorLocation();
			Start.Z = 0;
			End.Z = 0;
			TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		}
	}
	else {
		TargetRotation = GetBaseAimRotation();
	}
		//TargetRotation.Pitch = FMath::ClampAngle((TargetRotation - MeshRotation).Yaw, WeaponEquipped ? -30.0f : -15.0f, WeaponEquipped ? 30.0f : 15.0f);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3));

	if (GetVelocity().Size() > 0) {
		DroidState.Moving = true;
	}
	else {
		DroidState.Moving = false;
	}

	if (HasAuthority() && GetActorLocation().Z < -10000) {
		APlayingGameMode *Gamemode = Cast<APlayingGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (Gamemode == nullptr) { return; }
		Gamemode->CountKill();
		Destroy();
	}

	if (HasAuthority() && DroidState.Dead && GetWorld()->GetTimeSeconds() > DespawnTime) {
		Destroy();
	}
}

// Called to bind functionality to input
void ADroid::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADroid::StartSprinting);
	//PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADroid::StopSprinting);

	//PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::StartAiming);
	//PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::StopAiming);

	//PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AMainCharacter::StartFireWeapon);
	//PlayerInputComponent->BindAction("FireWeapon", IE_Released, this, &AMainCharacter::StopFireWeapon);

	//PlayerInputComponent->BindAction("Taunt", IE_Pressed, this, &AMainCharacter::StartTaunting);

	//PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &AMainCharacter::ScrollUp);
	//PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &AMainCharacter::ScrollDown);

	//PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMainCharacter::EquipWeapon);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ADroid::OpenPauseMenu);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADroid::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADroid::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}
void ADroid::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}
void ADroid::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void ADroid::OpenPauseMenu()
{
	if (GetWorld()->GetGameInstance() == nullptr) { return; }
	UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	GameInstance->OpenPauseMenu();
}

void ADroid::Attack() {
	if (DroidState.TargetCharacter == nullptr) { return; }
	if ((DroidState.TargetCharacter->GetActorLocation() - GetActorLocation()).Size() < DroidSettings.AttackRange) {
		if (HasAuthority() && !DroidState.Dead) {
			DroidState.TargetCharacter->DamagePlayer(DroidSettings.Damage);
		}
		int RandomSound = (FMath::RandRange(0, AttackImpactSounds.Num() - 1));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackImpactSounds[RandomSound], DroidState.TargetCharacter->GetActorLocation(), FMath::RandRange(0.8f,1.2f), FMath::RandRange(0.8f, 1.2f));
	}
}

void ADroid::DamageDroid_Implementation(int Amount, FHitResult Hit) {
	CurrentHealth -= Amount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, DroidSettings.Health);

	if (CurrentHealth == 0 && !DroidState.Dead) {
		DroidState.Dead = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		if (GetController() != nullptr) {
			GetController()->UnPossess();
		}
		if (HasAuthority()) {
			APlayingGameMode *Gamemode = Cast<APlayingGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (Gamemode == nullptr) { return; }
			Gamemode->CountKill();
			DespawnTime = GetWorld()->GetTimeSeconds() + 20; 
		}
		//GetMesh()->AddImpulse((Hit.ImpactPoint - Hit.TraceStart).GetSafeNormal() * 1000);
	}
}

void ADroid::SetHit(bool NewHit) { DroidState.Hit = NewHit; }
void ADroid::SetSprinting(bool Sprinting){ DroidState.Sprinting = Sprinting; }
void ADroid::SetAttacking(bool Attacking){ DroidState.Attacking = Attacking; }
void ADroid::SetTargetCharacter(AMainCharacter * NewTargetCharacter){ DroidState.TargetCharacter = NewTargetCharacter; }

