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

	if (PhysicsHandle->GrabbedComponent) {

		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
			OUT PlayerViewPointLocation,
			OUT PlayerViewPointRotation
		);

		/// Draw a red trace in world to visualize grab distance
		FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * GrabReach;
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
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
	
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) 
	{
		InputComponent->BindAction("GrabAction", IE_Pressed, this, &UGrabber::GrabAction);
		InputComponent->BindAction("GrabAction", IE_Released, this, &UGrabber::ReleaseAction);
	}
	else UE_LOG(LogTemp, Error, TEXT("Unable to find InputComponent in %s"), *GetOwner()->GetName());
}

// Get the first Physics Body within pawn's reach with Ray-casting (or line tracing)
FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// Draw a red trace in world to visualize grab distance
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * GrabReach;
	/// PlayerViewPointLocation is where player is and where it starts
	/// PlayerViewPointRotation points which angle it will come out of
	/// GrabReach is how long in cm will the reach be

	//DrawDebugLine(GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);
	/// Don't want to persist line so set false so it will recreate every tick
	/// Life time is not relevant if lines do not persist

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
	/// InTraceComplex is visibility collision view mode, false is like player collision

	//AActor* HitActor = Hit.GetActor();
	//if (HitActor) { UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *(HitActor->GetName())); }
	return Hit;
}

void UGrabber::GrabAction() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"));

	auto HitResult = GetFirstPhysicsBodyInReach();
	if (HitResult.GetActor() != nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *(HitResult.GetActor()->GetName()));
		auto ComponentToGrab = HitResult.GetComponent();
		/// auto infers to its object type

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,	// no skeletal mesh
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::ReleaseAction() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"));

	PhysicsHandle->ReleaseComponent();
}

