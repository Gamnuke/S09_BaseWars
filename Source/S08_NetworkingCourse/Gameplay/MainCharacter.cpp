// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

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
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, ChatDisplayWidget)
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Server_CreateChatDisplay(Cast<APlayerController>(Controller));

	if (InGameHUDClass == nullptr || Controller == nullptr) { return; }
	InGameHUD = CreateWidget<UInGameHUD>(GetGameInstance(), InGameHUDClass, FName("GameHUD"));
	InGameHUD->AddToViewport(0);

	//AssignedColor = FSlateColor(FLinearColor(FMath::FRand(), FMath::FRand(), FMath::FRand()));
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld()->GetTimeSeconds() > NextDisplayUpdate && IsActorInitialized()) {
		NextDisplayUpdate = GetWorld()->GetTimeSeconds() + 10;
		if (PlayerState != nullptr) {
			Server_CreateChatDisplay(FText::FromString(PlayerState->GetName()), FText());
		}
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

bool AMainCharacter::Server_CreateChatDisplay_Validate(const FText &PlayerName, const FText &Message) {
	return true;
}
void AMainCharacter::Server_CreateChatDisplay_Implementation(const FText &PlayerName, const FText &Message) {
	Multicast_CreateChatDisplay(PlayerName, Message);
}
void AMainCharacter::Multicast_CreateChatDisplay_Implementation(const FText &PlayerName, const FText &Message) {
	if (ChatDisplayWidget != nullptr) { return; } //Stop execution if widget already exists
	ChatDisplayWidget = CreateWidget<UChatDisplayWidget>(GetGameInstance(), ChatDisplayWidgetClass, FName(*FString("Chat" + PlayerState->GetUniqueID())));
	WidgetComponent->SetWidget(ChatDisplayWidget);

	if (ChatDisplayWidget != nullptr) { return; } //Stop execution if widget already exists
	ChatDisplayWidget->PlayerName->SetText(PlayerName);

	if (Message.ToString() != FString()) {
		ChatDisplayWidget->AddChatTab(Message);
	}
}
