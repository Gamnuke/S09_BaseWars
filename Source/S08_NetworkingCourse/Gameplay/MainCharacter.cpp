// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextRenderComponent.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

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

// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh(), FName("headSocket"));

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

	SwitchWeapon();
}

void AMainCharacter::OnRep_ColorAssigned() {
	Server_CreateChatDisplay(FText::FromString(this->GetPlayerState()->GetPlayerName()), FText(), AssignedColor);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

		SetServerState(NewState);
	}

	if (Sprinting || !WeaponEquipped) {
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
	if (Aiming) { DefaultCameraFOV = 50; }

	if (CurrentWeapon != nullptr) {
		CameraComponent->SetFieldOfView(FMath::FInterpTo(CameraComponent->FieldOfView, DefaultCameraFOV, DeltaTime, CurrentWeapon->WeaponSettings.CameraFOVResetTime));
	}

	if (CurrentWeapon != nullptr) {
		CurrentWeapon->AimLocation = ProjectedLocation;
		CurrentWeapon->AimDirection = ProjectedDirection;
	}
}

// Movement Functionality
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::StopAiming);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AMainCharacter::StartFireWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Released, this, &AMainCharacter::StopFireWeapon);

	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &AMainCharacter::ScrollUp);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &AMainCharacter::ScrollDown);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMainCharacter::EquipWeapon);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMainCharacter::OpenPauseMenu);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}
void AMainCharacter::MoveForward(float Value)
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
void AMainCharacter::MoveRight(float Value)
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
void AMainCharacter::StartSprinting() { 
	Sprinting = true;
	Aiming = false;
}
void AMainCharacter::StopSprinting(){
	Sprinting = false;
}
void AMainCharacter::StartAiming() {
	Aiming = true;
	Sprinting = false;
}
void AMainCharacter::StopAiming(){
	Aiming = false;
}

void AMainCharacter::EquipWeapon() {
	WeaponEquipped = !WeaponEquipped;
	Aiming = false;
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
		ChatDisplayWidget = CreateWidget<UChatDisplayWidget>(GetGameInstance(), ChatDisplayWidgetClass, FName(*FString("Chat" + GetPlayerState()->GetUniqueID())));
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

			if (CharacterCast->InGameHUD == nullptr) {
				GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 5, FColor::Green, FString("NULL!"));

			}
			if (CharacterCast != nullptr && CharacterCast->InGameHUD != nullptr) {

				UChatTab *NewChatTab = CreateWidget<UChatTab>(UGameplayStatics::GetPlayerController(GetWorld(),0), ChatTabClass, *Message.ToString());
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
}
void AMainCharacter::StartFireWeapon() {
	FiringWeapon = true;
}
void AMainCharacter::StopFireWeapon() {
	FiringWeapon = false;
}
void AMainCharacter::AnimNotify_Fire() {
	if (CurrentWeapon != nullptr) {
		CurrentWeapon->FireWeapon();
		CameraComponent->SetFieldOfView(CameraComponent->FieldOfView + CurrentWeapon->WeaponSettings.CameraFOVIncrement);
		TargetCameraPitch += CurrentWeapon->WeaponSettings.CameraPitchIncrement;
	}
}
void AMainCharacter::ScrollUp() {
	CurrentWeaponIndex += 1;
	SwitchWeapon();
}
void AMainCharacter::ScrollDown() {
	CurrentWeaponIndex -= 1;
	SwitchWeapon();
}

void AMainCharacter::SwitchWeapon() {
	CurrentWeaponIndex = FMath::Clamp(CurrentWeaponIndex, 0, Weapons.Num() - 1);
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Red, FString::FromInt(CurrentWeaponIndex));
	if (CurrentWeapon != nullptr) {
		CurrentWeapon->DetachFromParent();
		CurrentWeapon->DestroyComponent();
	}
	if (Weapons[CurrentWeaponIndex] != nullptr) {
		CurrentWeapon = NewObject<UWeaponComponent>(this, Weapons[CurrentWeaponIndex], *("Weapon" + FString::FromInt(CurrentWeaponIndex)));
		CurrentWeapon->SetupAttachment(GetMesh(), FName("Handle"));
		CurrentWeapon->RegisterComponent();
	}
}