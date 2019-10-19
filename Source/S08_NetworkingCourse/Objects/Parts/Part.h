// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/NetworkUI/Menu.h"
#include "Public/Misc/Optional.h"
#include "Components/MeshComponent.h"
#include "Part.generated.h"

USTRUCT(Blueprintable)
struct FPartStats {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString DetailText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Speed) int32 Cost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Damage) int32 Damage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Damage) bool UseDamage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = FireRate) float FireRate;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = FireRate) bool UseFireRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Speed) float Speed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Speed) bool UseSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Mobility) bool IsMovable = true;

	FString GetText() {
		if (UseDamage) { DetailText += FString("Damage: ") + FString::FromInt(Damage) + FString("\n"); }
		if (UseFireRate) { DetailText += FString("Fire Rate: ") + FString::SanitizeFloat(FireRate) + FString("\n"); }
		if (UseSpeed) { DetailText += FString("Speed: ") + FString::SanitizeFloat(Speed) + FString("\n"); }
		return DetailText;
	}
};

UCLASS()
class S08_NETWORKINGCOURSE_API APart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APart();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *Mesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USkeletalMeshComponent *SkeletalMesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USceneComponent *Scene;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString PartName = FString("DefaultPart");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TEnumAsByte<ESubCategory> Category;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool DefaultLocked = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FPartStats Details = FPartStats();
};
