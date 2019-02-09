// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"

#include "PhysicsEngine/BodyInstance.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/CircularThrobber.h"

#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayWidget.h"
#include "MenuSystem/InGameUI/InGameHUD.h"

#include "Camera/CameraComponent.h"

#include "UnrealNetwork.h"
#include "ConstructorHelpers.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "PlatformerGameInstance.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatTab.h"
#include "Misc/Optional.h"

#include "Gameplay/CharacterAnimInstance.h"
#include "Gameplay/WeaponComponent.h"
#include "Gameplay/Projectile.h"
#include "Gameplay/ActiveGameState.h"
#include "Gameplay/Weapon.h"

#include "Materials/Material.h"

// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh(), FName("headSocket"));

	PrimaryWeaponModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimaryWeaponModel"));
	PrimaryWeaponModel->SetupAttachment(GetMesh(), FName("PrimaryWeaponSocket"));

	SecondaryWeaponModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryWeaponModel"));
	SecondaryWeaponModel->SetupAttachment(GetMesh(), FName("SecondaryWeaponSocket"));

	static ConstructorHelpers::FClassFinder<UUserWidget> ChatDisplayWidgetSearch(TEXT("/Game/UI/InGame/ChatUI/ChatDisplay_WBP"));
	if (ChatDisplayWidgetSearch.Class != NULL)
	{
		ChatDisplayWidgetClass = ChatDisplayWidgetSearch.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ChatDisplayTabSearch(TEXT("/Game/UI/InGame/ChatUI/ChatTab_WBP"));
	if (ChatDisplayTabSearch.Class != NULL)
	{
		ChatDisplayTabClass = ChatDisplayTabSearch.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> HUDSearch(TEXT("/Game/UI/InGame/InGameHUD_WBP"));
	if (HUDSearch.Class != NULL)
	{
		InGameHUDClass = HUDSearch.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ChatTab(TEXT("/Game/UI/InGame/ChatUI/ChatTab_WBP"));
	if (ChatTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *ChatTab.Class->GetName());
		ChatTabClass = ChatTab.Class;
	}


}

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, ChatDisplayWidget);
	DOREPLIFETIME(AMainCharacter, InGameHUD);
	DOREPLIFETIME(AMainCharacter, ServerState);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	AssignedColor = FLinearColor(FMath::FRand(), FMath::FRand(), FMath::FRand());
	/*if (PlayerState != nullptr) {
		Server_CreateChatDisplay(FText::FromString(PlayerState->GetPlayerName()), FText(), AssignedColor);
	}*/
	WeaponModels.Init(nullptr, 2);
	if (GetWorld() != nullptr) {
		if (GetWorld()->GetName() == FString("Lobby")) {
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr) {
				FInputModeGameAndUI InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
			}
		}
		else {
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr) {
				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = false;
			}
		}


		UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance == nullptr) { return; }
		GameInstance->SetupGame();
	}
	int ii = 0;
	for (TSubclassOf<UWeaponComponent> WeaponComponent : Weapons) {
		if (WeaponComponent != nullptr && Weapons[ii] != nullptr) {
			UWeaponComponent *Model = NewObject<UWeaponComponent>(this, Weapons[ii], *WeaponComponent.GetDefaultObject()->GetName());
			
			//WeaponModels.Insert(Model, ii)\;
			if (WeaponModels.IsValidIndex(ii)) {
				WeaponModels[ii] = Model;
			}
			if (SocketNames[ii].IsValid()) {
				Model->SetupAttachment(GetMesh(), SocketNames[ii]);
			}
			Model->RegisterComponent();
			Model->CanShoot = false;
			Model->CurrentMagazineAmmo = Model->WeaponSettings.MagazineCapacity;
			Model->CurrentMaxAmmo = Model->WeaponSettings.MaxAmmo;
		}
		ii++;

	}

	StartSwitchingWeapon();
	WeaponEquipped = true;
}

void AMainCharacter::SetMeshMaterial(UStaticMeshComponent *Mesh, UWeaponComponent *MeshToSetMaterial) {
	for (int32 i = 0; i < MeshToSetMaterial->GetNumMaterials(); i++)
	{
		Mesh->SetMaterial(i, MeshToSetMaterial->GetMaterial(i));
	}
}

void AMainCharacter::OnRep_ColorAssigned() {
	Server_CreateChatDisplay(FText::FromString(this->GetPlayerState()->GetPlayerName()), FText(), AssignedColor);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		AActiveGameState * GS = Cast<AActiveGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (GS != nullptr) {
			FRoundData RD = GS->RoundData;
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(4, 5, FColor::Red, FString::FromInt(RD.EnemiesLeft));
			//InGameHUD->RoundDisplay->SetText(FText::FromString(FString("Round: " + FString::FromInt(RD.CurrentRound))));
			//InGameHUD->RoundProgress->SetPercent(1.0f - float(RD.EnemiesLeft) / float(RD.MaxEnemiesThisRound));
		}
	}
	AActiveGameState * GS = GetWorld()->GetGameState<AActiveGameState>();
	if (GS != nullptr && InGameHUD != nullptr) {
		FRoundData RD = GS->RoundData;
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(3, 5, FColor::Orange, FString("FOUND GAME STATEEEEEEE"));
		InGameHUD->RoundDisplay->SetText(FText::FromString(FString("Round: " + FString::FromInt(RD.CurrentRound))));
		InGameHUD->RoundProgress->SetPercent(1.0f - float(RD.EnemiesLeft) / float(RD.MaxEnemiesThisRound));
	}

	if (HasAuthority()) {
		if (CurrentHealth != 100 && !Dead && GetWorld()->GetTimeSeconds() >= NextCanHealthRegenTime && GetWorld()->GetTimeSeconds() >= NextHealthRegenTime) 
		{
			NextHealthRegenTime = GetWorld()->GetTimeSeconds() + 0.5;
			DamagePlayer(-5);
		}
	}

	if (InGameHUD != nullptr && IsLocallyControlled()) {
		if (ReloadingWeapon) {
			InGameHUD->ReloadThrobber->SetVisibility(ESlateVisibility::Visible);
			InGameHUD->AmmoDisplay->SetText(FText::FromString("| Reloading |"));
		}
		else {
			InGameHUD->ReloadThrobber->SetVisibility(ESlateVisibility::Collapsed);
			if (CurrentWeapon != nullptr) {
				UpdateAmmoDisplay(CurrentWeapon->CurrentMagazineAmmo, CurrentWeapon->CurrentMaxAmmo);
			}
		}
	}

	if (GetWorld()->GetTimeSeconds() > NextDisplayUpdate && IsActorInitialized()) {
		NextDisplayUpdate = GetWorld()->GetTimeSeconds() + 10;
		if (this->GetPlayerState() != nullptr) {
			Server_CreateChatDisplay(FText::FromString(this->GetPlayerState()->GetPlayerName()), FText(), AssignedColor);
		}
	}

	Idle = GetVelocity().Size() == 0;
	//Falling = GetCharacterMovement()->IsFalling();

	TargetCameraPitch = FMath::FInterpTo(TargetCameraPitch, 0, DeltaTime, 5);
	CameraComponent->SetRelativeRotation(FRotator(FMath::FInterpTo(CameraComponent->RelativeRotation.Pitch, TargetCameraPitch, DeltaTime, 5),0,0));

	if (Sprinting && !Idle && !Falling) {
		TargetCameraRelativeLocation += FVector(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f));
	}
	TargetCameraRelativeLocation = FMath::VInterpTo(TargetCameraRelativeLocation, FVector::ZeroVector, DeltaTime, 2);
	CameraComponent->SetRelativeLocation(FMath::VInterpTo(CameraComponent->RelativeLocation, TargetCameraRelativeLocation + FVector(0,60,0), DeltaTime, 5));

	if (IsLocallyControlled()) {
		Velocity = GetVelocity().GetSafeNormal();
		Velocity.Z = 0;
		MeshRotation = GetMesh()->GetComponentRotation();
		ControlRotation = GetControlRotation();
		RotVelocity = GetMesh()->GetPhysicsAngularVelocityInDegrees();
		MeshRelativeRotation = GetMesh()->RelativeRotation;
		AimPitch = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;

		if (InGameHUD != nullptr) {
			ProjectedLocation = InGameHUD->ProjectedLocation;
			ProjectedDirection = InGameHUD->ProjectedDirection;
		}

		FState NewState;
		NewState.NewAiming = Aiming;
		NewState.NewSprinting = Sprinting;
		NewState.NewVelocity = Velocity;
		NewState.NewRotVelocity = RotVelocity;
		NewState.NewMeshRotation = MeshRotation;
		NewState.MeshRelativeRotation = MeshRelativeRotation;
		NewState.NewAimPitch = AimPitch;
		NewState.NewFalling = Falling;
		NewState.NewWeaponEquipped = WeaponEquipped;
		NewState.NewFiringWeapon = FiringWeapon;
		NewState.NewControlRotation = ControlRotation;
		NewState.NewProjectedLocation = ProjectedLocation;
		NewState.NewProjectedDirection = ProjectedDirection;
		NewState.CurrentWeaponIndex = CurrentWeaponIndex;
		NewState.PreviousWeaponIndex = PreviousWeaponIndex;
		NewState.SwitchingWeapon = SwitchingWeapon;
		NewState.FireAnimPlaying = FireAnimPlaying;
		NewState.Taunting = Taunting;
		NewState.ReloadingWeapon = ReloadingWeapon;
		SetServerState(NewState);
	}
	if (Taunting || Sprinting || !WeaponEquipped) {
		if (GetCharacterMovement()->GetLastInputVector() == FVector::ZeroVector) {
			TargetRotation = MeshRotation;
			TargetRotation.Pitch = 0;
			TargetRotation.Roll = 0; 
		}
		else {
			TargetRotation = (Velocity.Rotation() - FRotator(0, 90, 0));
		}
		TargetRotation.Pitch = FMath::ClampAngle((TargetRotation - MeshRotation).Yaw, WeaponEquipped ? -30.0f : -15.0f, WeaponEquipped ? 30.0f : 15.0f);
		GetMesh()->SetWorldRotation(FMath::RInterpTo(MeshRotation, TargetRotation, DeltaTime, 8));
	}
	else {
		GetMesh()->SetWorldRotation(FMath::RInterpTo(MeshRotation, FRotator(0,ControlRotation.Yaw, 0) - FRotator(0, 90, 0), DeltaTime, 10) );
	}

	if (Aiming) {
		if (WeaponEquipped) {
			GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}
	else if(Sprinting){
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	DefaultCameraFOV = Sprinting ? 100 : 80;
	if (Aiming && CurrentWeapon != nullptr) { DefaultCameraFOV = CurrentWeapon->WeaponSettings.CameraZoomFOV; }

	if (CurrentWeapon != nullptr) {
		CameraComponent->SetFieldOfView(FMath::FInterpTo(CameraComponent->FieldOfView, DefaultCameraFOV, DeltaTime, CurrentWeapon->WeaponSettings.CameraFOVResetTime));
	}

	if (CurrentWeapon != nullptr) {
		CurrentWeapon->AimLocation = ProjectedLocation;
		CurrentWeapon->AimDirection = ProjectedDirection;
	}

	if (GetVelocity() != FVector(0,0,0)) {
		Taunting = false;
	}

	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::FromInt(CurrentHealth));

	FHitResult AimHit;
	GetWorld()->LineTraceSingleByChannel(
		AimHit,
		ProjectedLocation,
		ProjectedDirection + ProjectedDirection * 1000000,
		ECollisionChannel::ECC_Visibility
	);

	if (AimHit.GetActor() != nullptr) {
		AWeapon *WeaponContainer = Cast<AWeapon>(AimHit.GetActor());
		if (WeaponContainer != nullptr) {
			HoveredWeapon = WeaponContainer;
		}
		else {
			HoveredWeapon = nullptr;
		}
	}
	else {
		HoveredWeapon = nullptr;
	}
}


// Movement Functionality
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::StopAiming);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AMainCharacter::StartFireWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Released, this, &AMainCharacter::StopFireWeapon);

	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AMainCharacter::DropWeapon);

	PlayerInputComponent->BindAction("ReplaceWeapon", IE_Pressed, this, &AMainCharacter::ReplaceWeapon);

	PlayerInputComponent->BindAction("Taunt", IE_Pressed, this, &AMainCharacter::StartTaunting);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::StartReloadingWeapon);

	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &AMainCharacter::ScrollUp);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &AMainCharacter::ScrollDown);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMainCharacter::EquipWeapon);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMainCharacter::OpenPauseMenu);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}
void AMainCharacter::Jump() {
	if (!Dead) {
		ACharacter::Jump();
	}
}
void AMainCharacter::StartReloadingWeapon() {
	if (CurrentWeapon == nullptr || ReloadingWeapon) { return; }
	if (CurrentWeapon->CurrentMagazineAmmo == CurrentWeapon->WeaponSettings.MagazineCapacity || CurrentWeapon->CurrentMaxAmmo == 0) { return; }
	ReloadingWeapon = true;
	FiringWeapon = false;
	if (Aiming == false) { return; }
	Aiming = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->WeaponSettings.WeaponZoomOutSound, CurrentWeapon->GetComponentLocation());
}
void AMainCharacter::MulticastDropWeapon_Implementation() {
	if (CurrentWeapon == nullptr || SwitchingWeapon) { return; }
	Aiming = false;
	WeaponModels[CurrentWeaponIndex] = nullptr;
	AWeapon *WeaponActor = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass(), CurrentWeapon->GetComponentLocation(), CurrentWeapon->GetComponentRotation());
	CurrentWeapon->AttachToComponent(WeaponActor->Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	WeaponActor->AssignedWeaponComponent = CurrentWeapon;
	WeaponActor->Mesh->SetStaticMesh(CurrentWeapon->GetStaticMesh());
	CurrentWeapon->UnregisterComponent();
	CurrentWeapon->CanShoot = false;
	CurrentWeapon = nullptr;
	WeaponEquipped = false;
}
void AMainCharacter::DropWeapon_Implementation()
{
	MulticastDropWeapon();
}
bool AMainCharacter::DropWeapon_Validate() {
	return true;
}

void AMainCharacter::MulticastReplaceWeapon_Implementation() {
	if (!SwitchingWeapon && HoveredWeapon != nullptr && HoveredWeapon->AssignedWeaponComponent != nullptr) {
		for (int32 i = 0; i < 2; i++)
		{
			if (WeaponModels.IsValidIndex(i) && WeaponModels[i] == nullptr) {
				WeaponModels[i] = HoveredWeapon->AssignedWeaponComponent;
				WeaponModels[i]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNames[i]);
				WeaponModels[i]->RegisterComponent();
				HoveredWeapon->Destroy();
				return;
			}
		}
		if (CurrentWeapon != nullptr) {
			DropWeapon();
			CurrentWeapon = HoveredWeapon->AssignedWeaponComponent;
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNames[CurrentWeaponIndex]);
			CurrentWeapon->RegisterComponent();
		}
		WeaponModels[CurrentWeaponIndex] = HoveredWeapon->AssignedWeaponComponent;

		HoveredWeapon->Destroy();
		StartSwitchingWeapon();
	}
}
void AMainCharacter::ReplaceWeapon_Implementation()
{
	MulticastReplaceWeapon();
}
bool AMainCharacter::ReplaceWeapon_Validate() { return true; }

void AMainCharacter::ReloadWeapon() {
	if (CurrentWeapon == nullptr) { return; }
	CurrentWeapon->ReloadWeapon();
	ReloadingWeapon = false;

}
void AMainCharacter::StartTaunting() { 
	if (GetVelocity() == FVector(0, 0, 0)) {
		Taunting = true;
	}
}
void AMainCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !Dead)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}
void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !Dead)
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
void AMainCharacter::StartSprinting() { 
	Sprinting = true;
	if (CurrentWeapon == nullptr || Aiming == false) { return; }
	Aiming = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->WeaponSettings.WeaponZoomOutSound, CurrentWeapon->GetComponentLocation());
}
void AMainCharacter::StopSprinting(){
	Sprinting = false;
}
void AMainCharacter::StartAiming() {
	if (CurrentWeapon == nullptr) { return; }
	Aiming = true;
	Sprinting = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->WeaponSettings.WeaponZoomInSound, CurrentWeapon->GetComponentLocation());
}
void AMainCharacter::StopAiming(){
	if (CurrentWeapon == nullptr || Aiming == false) { return; }
	Aiming = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->WeaponSettings.WeaponZoomOutSound, CurrentWeapon->GetComponentLocation());
}

void AMainCharacter::EquipWeapon() {
	WeaponEquipped = !WeaponEquipped;
	Aiming = false;
	if (CurrentWeapon == nullptr) { return; }
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->WeaponSettings.WeaponZoomOutSound, CurrentWeapon->GetComponentLocation());
}

void AMainCharacter::OpenPauseMenu()
{
	if (GetWorld()->GetGameInstance() == nullptr) { return; }
	UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	GameInstance->OpenPauseMenu();
}

bool AMainCharacter::Server_CreateChatDisplay_Validate(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor) {
	return true;
}
void AMainCharacter::Server_CreateChatDisplay_Implementation(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor) {
	Multicast_CreateChatDisplay(PlayerName, Message, NewAssignedColor);
}
void AMainCharacter::Multicast_CreateChatDisplay_Implementation(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor) {
	if (InGameHUD == nullptr) {
		InGameHUD = CreateWidget<UInGameHUD>(GetGameInstance(), InGameHUDClass, FName("GameHUD"));
		InGameHUD->AddToViewport(0);
	}
	if (ChatDisplayWidget == nullptr) { //Stop execution if widget already exists
		if (GetGameInstance() != nullptr&&GetPlayerState() != nullptr&&ChatDisplayWidgetClass != nullptr) {
			ChatDisplayWidget = CreateWidget<UChatDisplayWidget>(GetGameInstance(), ChatDisplayWidgetClass, FName(*FString("Chat" + GetPlayerState()->GetUniqueID())));
		}
	}
	if (ChatDisplayWidget != nullptr) { 
		WidgetComponent->SetWidget(ChatDisplayWidget);
		ChatDisplayWidget->PlayerName->SetText(PlayerName);
	}

	if (Message.ToString() != FString()) {
		ChatDisplayWidget->AddChatTab(FText(Message), NewAssignedColor);

		if (InGameHUD == nullptr) { return; }
		TArray <AActor*> Characters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainCharacter::StaticClass(), Characters);

		for (AActor *Character : Characters) {
			AMainCharacter *CharacterCast = Cast<AMainCharacter>(Character);

			if (CharacterCast != nullptr && CharacterCast->InGameHUD != nullptr && CharacterCast->InGameHUD->ChatBox != nullptr) {
				UChatTab *NewChatTab = CreateWidget<UChatTab>(UGameplayStatics::GetPlayerController(GetWorld(), 0), ChatTabClass, *Message.ToString());
				NewChatTab->Setup(PlayerName, Message, NewAssignedColor);
				CharacterCast->InGameHUD->ChatBox->AddChild(NewChatTab);
			}
		}
	}
}


bool AMainCharacter::SetServerState_Validate(FState NewState) { return true; }
void AMainCharacter::SetServerState_Implementation(FState NewState) {
	ServerState = NewState;
	SetState(ServerState);
}
void AMainCharacter::OnRep_ServerState() {
	SetState(ServerState);
}
void AMainCharacter::SetState(FState NewState) {
	if (Role != ROLE_AutonomousProxy) {
		Aiming = NewState.NewAiming;
		Sprinting = NewState.NewSprinting;
		Velocity = NewState.NewVelocity;
		MeshRotation = NewState.NewMeshRotation;
		RotVelocity = NewState.NewRotVelocity;
		MeshRelativeRotation = NewState.MeshRelativeRotation;
		AimPitch = NewState.NewAimPitch;
		Falling = NewState.NewFalling;
		WeaponEquipped = NewState.NewWeaponEquipped;
		FiringWeapon = NewState.NewFiringWeapon;
		ControlRotation = NewState.NewControlRotation;
		ProjectedLocation = NewState.NewProjectedLocation;
		ProjectedDirection = NewState.NewProjectedDirection;
		CurrentWeaponIndex = NewState.CurrentWeaponIndex;
		PreviousWeaponIndex = NewState.PreviousWeaponIndex;
		SwitchingWeapon = NewState.SwitchingWeapon;
		Taunting = NewState.Taunting;
		FireAnimPlaying = NewState.FireAnimPlaying;
		ReloadingWeapon = NewState.ReloadingWeapon;
	}
}
void AMainCharacter::StartFireWeapon() {
	if (CurrentWeapon != nullptr && CurrentWeapon->CanShoot && !ReloadingWeapon && CurrentWeapon->CurrentMagazineAmmo > 0) {
		FiringWeapon = true;
	}
	if (ReloadingWeapon) {
		ReloadingWeapon = false;
	}
}
void AMainCharacter::StopFireWeapon() {
	FiringWeapon = false;
}
void AMainCharacter::AnimNotify_Fire() {
	if (CurrentWeapon != nullptr && CurrentWeapon->CanShoot) {
		CurrentWeapon->FireWeapon();
		if (CurrentWeapon->WeaponSettings.SemiAuto) {
			FiringWeapon = false;
		}
	}
}
void AMainCharacter::ScrollUp() {
	if (CurrentWeaponIndex + 1 > Weapons.Num() - 1 || SwitchingWeapon || WeaponModels[CurrentWeaponIndex + 1] == nullptr) { return; }
	PreviousWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex += 1;
	CurrentWeaponIndex = FMath::Clamp(CurrentWeaponIndex, 0, Weapons.Num() - 1);
	FiringWeapon = false;
	StartSwitchingWeapon();
}
void AMainCharacter::ScrollDown() {
	if (CurrentWeaponIndex - 1 < 0 || SwitchingWeapon || WeaponModels[CurrentWeaponIndex - 1] == nullptr) { return; }
	PreviousWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex -= 1;
	CurrentWeaponIndex = FMath::Clamp(CurrentWeaponIndex, 0, Weapons.Num() - 1);
	FiringWeapon = false;
	StartSwitchingWeapon();
}

void AMainCharacter::StartSwitchingWeapon() {
	if (SwitchingWeapon == false) {
		if (WeaponModels.IsValidIndex(CurrentWeaponIndex) && WeaponModels[CurrentWeaponIndex] != nullptr && CurrentWeapon != WeaponModels[CurrentWeaponIndex]) {
			SwitchingWeapon = true;
			WeaponEquipped = true;
			if (CurrentWeapon != nullptr) {
				CurrentWeapon->CanShoot = false;
				ReloadingWeapon = false;
			}
		}
		else {
			WeaponEquipped = false;
		}
	}
}

void AMainCharacter::PlacedWeapon() {
	if (CurrentWeapon != nullptr) {
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNames[PreviousWeaponIndex]);

	}
}

void AMainCharacter::SwitchWeapon() {
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Red, FString::FromInt(CurrentWeaponIndex));
	if (WeaponModels.IsValidIndex(CurrentWeaponIndex) && WeaponModels[CurrentWeaponIndex] != nullptr) {
		CurrentWeapon = WeaponModels[CurrentWeaponIndex];
		if (CurrentWeapon == nullptr) { return; }
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Handle"));
		CurrentWeapon->RegisterComponent();
		UpdateAmmoDisplay(CurrentWeapon->CurrentMagazineAmmo, CurrentWeapon->CurrentMaxAmmo);
	}
}


void AMainCharacter::FinishedSwitchingWeapon() {
	SwitchingWeapon = false;
	if (CurrentWeapon != nullptr) {
		CurrentWeapon->CanShoot = true;
	}
}

void AMainCharacter::SpawnProjectile(FTransform Transform, TSubclassOf<AProjectile> Projectile, FProjectileSettings Settings) {
	AProjectile *NewProjectile = GetWorld()->SpawnActor<AProjectile>(Projectile, Transform);
	NewProjectile->ProjectileSettings = Settings;
	if (Role == ROLE_AutonomousProxy || (IsLocallyControlled() && Role == ROLE_Authority)) {
		NewProjectile->HudRef = InGameHUD;
	}
	if (Role == ROLE_Authority) {
		NewProjectile->OwnedByServer = true;
	}
}

bool AMainCharacter::ServerSpawnProjectile_Validate(FTransform Transform, TSubclassOf<AProjectile> Projectile, FProjectileSettings Settings) { return true; }
void AMainCharacter::ServerSpawnProjectile_Implementation(FTransform Transform, TSubclassOf<AProjectile> Projectile, FProjectileSettings Settings) {
	MulticastSpawnProjectile(Transform, Projectile, Settings);
}
void AMainCharacter::MulticastSpawnProjectile_Implementation(FTransform Transform, TSubclassOf<AProjectile> Projectile, FProjectileSettings Settings) {
	if (IsLocallyControlled() == false) {
		SpawnProjectile(Transform, Projectile, Settings);
	}
}

void AMainCharacter::DamagePlayer(int Amount) {
	CurrentHealth -= Amount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, 100);

	if (Amount > 0) {
		NextCanHealthRegenTime = GetWorld()->GetTimeSeconds() + 5;
		SetHealth(CurrentHealth, false);
	}
	else {
		SetHealth(CurrentHealth, true);
	}
}

void AMainCharacter::SetHealth_Implementation(float Health, bool Heal) {
	if (IsLocallyControlled()) {
		CurrentHealth = Health;
		if (InGameHUD != nullptr) {
			InGameHUD->HealthBar->SetPercent(Health / 100);

			if (!Heal) {
				InGameHUD->DamageVignette->SetRenderOpacity(InGameHUD->VignetteTargetOpacity + 0.6);
				InGameHUD->VignetteTargetOpacity = 1 - (Health / 100);
			}
			else {
				InGameHUD->VignetteTargetOpacity = 1 - (Health / 100);
			}
		}
		
	}
	if (Health == 0 && !Dead) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		//GetMesh()->AddImpulse((Hit.ImpactPoint - Hit.TraceStart).GetSafeNormal() * 5000);
		GetCapsuleComponent()->SetSimulatePhysics(false);
		GetCapsuleComponent()->BodyInstance.bLockTranslation = true;
		CameraBoom->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
		CameraComponent->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepWorldTransform);
		Dead = true;
	}
}

void AMainCharacter::UpdateAmmoDisplay(int MagazineAmmo, int MaxAmmo) {
	if (InGameHUD == nullptr || InGameHUD->AmmoDisplay == nullptr || !IsLocallyControlled()) { return; }

	InGameHUD->AmmoDisplay->SetText(FText::FromString(FString::FromInt(MagazineAmmo) + " | " + FString::FromInt(MaxAmmo)));
}
