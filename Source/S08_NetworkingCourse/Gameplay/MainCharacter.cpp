// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
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

#include "PlatformerGameInstance.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatTab.h"

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
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 1000, FColor::Green, GetWorld()->GetName());


		UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance == nullptr) { return; }
		GameInstance->SetupGame();
	}
}

void AMainCharacter::OnRep_ColorAssigned() {
	Server_CreateChatDisplay(FText::FromString(PlayerState->GetPlayerName()), FText(), AssignedColor);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld()->GetTimeSeconds() > NextDisplayUpdate && IsActorInitialized()) {
		NextDisplayUpdate = GetWorld()->GetTimeSeconds() + 10;
		if (PlayerState != nullptr) {

			Server_CreateChatDisplay(FText::FromString(PlayerState->GetPlayerName()), FText(), AssignedColor);
		}
	}

	
	/*if (GetOwner()->GetLocalRole() == ROLE_Authority || ROLE_SimulatedProxy) {
		APawn *Pawn = UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0);
		if (Pawn != nullptr) {
			AMainCharacter *Character = Cast<AMainCharacter>(Pawn);
			if (Character == nullptr || InGameHUD == nullptr) { return; }
			FHitResult OcclusionTrace;
			GetWorld()->LineTraceSingleByChannel(
				OcclusionTrace,
				Character->GetActorLocation(),
				this->GetActorLocation(),
				ECollisionChannel::ECC_Visibility
			);
			float Distance = FVector::Dist(OcclusionTrace.TraceEnd, OcclusionTrace.TraceStart);
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, DeltaTime, FColor::Green, OcclusionTrace.bBlockingHit?FString("C++ Hit!"): FString("C++ Miss!"));

			if (OcclusionTrace.bBlockingHit || OcclusionTrace.Distance > 1000) {
				InGameHUD->SetVisibility(ESlateVisibility::Collapsed);
			}
			else {
				InGameHUD->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}*/
}

// Movement Functionality
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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
		ChatDisplayWidget = CreateWidget<UChatDisplayWidget>(GetGameInstance(), ChatDisplayWidgetClass, FName(*FString("Chat" + PlayerState->GetUniqueID())));
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
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::FromInt(Characters.Num()));
			
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
