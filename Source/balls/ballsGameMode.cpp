// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ballsGameMode.h"
#include "ballsBall.h"

AballsGameMode::AballsGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AballsBall::StaticClass();
}
