// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class S08_NetworkingCourseTarget : TargetRules
{
	public S08_NetworkingCourseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("S08_NetworkingCourse");
	}
}
