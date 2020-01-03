// Fill out your copyright notice in the Description page of Project Settings.


#include "Part.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/BuildModeUI/PartSettingUI/ComplexFloatSetting.h"
#include "DrawDebugHelpers.h"
#include "BasicTypes.h"
#include "UI/BuildModeUI/PartSettingUI/PartSettingMenu.h"

// Sets default values
UPart::UPart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	/*Scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(Scene);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeleMesh"));
	SkeletalMesh->SetupAttachment(Scene);*/

}

// Called when the game starts or when spawned
void UPart::BeginPlay()
{
	Super::BeginPlay();
	
	DrawDebugPoint(GetWorld(), GetComponentLocation(), 20, FColor::Green, false, 100, 100);
}

// Called every frame
void UPart::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPart::OnSelected()
{
}

void UPart::AllowVariableEdit(FComplexFloat &VariableToEdit)
{
	// Create a slider to edit a variable on the Part.
	//UUserWidget *NewWidget = CreateWidget(MenuRef, MenuRef->ComplexFloatTab, MakeUniqueObjectName(MenuRef, MenuRef->ComplexFloatTab, FName("ComplexFloatTab")));
	//MenuRef->PartSettingMenu->DetailContainer->AddChild(NewWidget);
}

void UPart::AllowVariableEdit(FComplexInt32 &VariableToEdit)
{
	// Create a slider to edit a variable on the Part.
	UComplexFloatSetting *NewWidget = CreateWidget<UComplexFloatSetting>(MenuRef, MenuRef->ComplexFloatTab, MakeUniqueObjectName(MenuRef, MenuRef->ComplexFloatTab, FName("ComplexFloatTab")));
	MenuRef->SettingTabs.Add(NewWidget);
	MenuRef->PartSettingMenu->DetailContainer->AddChild(NewWidget);
	FSlateChildSize Size = FSlateChildSize();
	Size.SizeRule = ESlateSizeRule::Fill;
	Cast<UVerticalBoxSlot>(NewWidget->Slot)->SetSize(Size);
	NewWidget->SetupSetting(&VariableToEdit);
}

void UPart::AllowVariableEdit(FComplexString &VariableToEdit)
{
	// Create a slider to edit a variable on the Part.
}

