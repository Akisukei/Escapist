// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

/// indicates parameter will be modified for us
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	GetPhysicsHandleComponent();
	SetupInputComponents();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// if physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {
		/// move the object we're holding
		PhysicsHandle->SetTargetLocation(GetTraceLineEnd());
	}
}

// Get PhysicsHandleComponent (assuming only available during runtime)
void UGrabber::GetPhysicsHandleComponent() {

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find PhysicsHandleComponent in %s"), *GetOwner()->GetName());
	}
}

// Set up InputComponent (component appears during runtime)
void UGrabber::SetupInputComponents() {
	
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) 
	{
		InputComponent->BindAction("GrabAction", IE_Pressed, this, &UGrabber::GrabAction);
		InputComponent->BindAction("GrabAction", IE_Released, this, &UGrabber::ReleaseAction);
	}
	else UE_LOG(LogTemp, Error, TEXT("Unable to find InputComponent in %s"), *GetOwner()->GetName());
}

// Get the first Physics Body within pawn's reach with Ray-casting (or line tracing)
FHitResult UGrabber::GetFirstPhysicsBodyInReach() {

	FVector LineTraceEnd = GetTraceLineEnd();
	//ShowDebugLine(LineTraceEnd);

	/// Line-trace AKA Ray-casting out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);
	/// Read params by hovering over
	/// reminder :: is used to extract value from enum
	/// InTraceComplex is like visibility collision view mode, false is like player collision

	return Hit;
}

// Get trace line's end location
FVector UGrabber::GetTraceLineEnd() {

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// PlayerViewPointLocation is where player is and where it starts
	/// PlayerViewPointRotation is the line's angle
	/// GrabReach is how long in cm will the reach be
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * GrabReach;
}

// Draw a red trace in world to visualize grab distance
void UGrabber::ShowDebugLine(FVector LineTraceEnd) {

	DrawDebugLine(GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);
	/// Don't want to persist line so set false so it will recreate every tick
	/// Life time is not relevant if lines do not persist
}

void UGrabber::GrabAction() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"));

	auto HitResult = GetFirstPhysicsBodyInReach();
	/// auto infers to its object type
	if (HitResult.GetActor() != nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *(HitResult.GetActor()->GetName()));

		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,	/// not a skeletal mesh but a static mesh
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::ReleaseAction() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"));

	PhysicsHandle->ReleaseComponent();
}

