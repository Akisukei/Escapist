// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	CloseDoor();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOnPlate() > 30.f) // TODO change hard code
	{
		OpenDoor();
		DoorOpenedTime = GetWorld()->GetTimeSeconds();
	}

	if (GetWorld()->GetTimeSeconds() - DoorOpenedTime > DoorClosingDelay) 
	{
		CloseDoor();
	}
}

// Get the total mass in kg of actors on the pressure plate
float UOpenDoor::GetTotalMassOnPlate() {

	float TotalMassInKg = 0.f;
	TArray<AActor*> OverlappingActors;

	if (PressurePlate != nullptr) 
	{
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);
		for (const auto* Actor : OverlappingActors)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s on pressure plate"), *Actor->GetName());
			TotalMassInKg += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}

	return TotalMassInKg;
}

// Opens the door
void UOpenDoor::OpenDoor()
{
	if(Owner != nullptr) Owner->SetActorRotation(FRotator(0.f,OpenAngle, 0.f));
}

// Closes the door
void UOpenDoor::CloseDoor()
{
	if (Owner != nullptr) Owner->SetActorRotation(FRotator(0.f, 90.f, 0.f));
}