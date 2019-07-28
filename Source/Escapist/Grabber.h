// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPIST_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere)
	float GrabReach = 100.f; /// in cm

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	/// SETUP
	void GetPhysicsHandleComponent();
	void SetupInputComponents();

	/// REG. METHODS
	FHitResult GetFirstPhysicsBodyInReach();
	FVector GetTraceLineEnd(); /// the end of grab reach distance
	void ShowDebugLine(FVector); /// show debug line for testing

	/// ACTIONS
	void GrabAction();
	void ReleaseAction();
};
