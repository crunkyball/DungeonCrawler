// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_SimpleParallel.generated.h"

namespace EBTParallelChild
{
	enum Type
	{
		MainTask,
		BackgroundTree,
		SearchIndicator,
	};
}

UENUM()
namespace EBTParallelMode
{
	// keep in sync with DescribeFinishMode
	enum Type
	{
		AbortBackground UMETA(DisplayName="Immediate" , ToolTip="When main task finishes, immediately abort background tree."),
		WaitForBackground UMETA(DisplayName="Delayed" , ToolTip="When main task finishes, wait for background tree to finish."),
	};
}

struct FBTParallelMemory : public FBTCompositeMemory
{
	/** finish result of main task */
	TEnumAsByte<EBTNodeResult::Type> MainTaskResult;

	/** set when main task is running */
	uint8 bMainTaskIsActive : 1;

	/** try running background tree task even if main task has finished */
	uint8 bForceBackgroundTree : 1;
};

/**
 * Simple Parallel composite node.
 * Allows for running two children: one which must be a single task node (with optional decorators), and the other of which can be a complete subtree.
 */
UCLASS()
class AIMODULE_API UBTComposite_SimpleParallel : public UBTCompositeNode
{
	GENERATED_UCLASS_BODY()

	/** how background tree should be handled when main task finishes execution */
	UPROPERTY(EditInstanceOnly, Category=Parallel)
	TEnumAsByte<EBTParallelMode::Type> FinishMode;

	/** handle child updates */
	int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const;

	virtual void NotifyChildExecution(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, int32 ChildIdx, EBTNodeResult::Type& NodeResult) const override;
	virtual void NotifyNodeDeactivation(struct FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const override;
	virtual bool CanPushSubtree(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, int32 ChildIdx) const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

	/** helper for showing values of EBTParallelMode enum */
	static FString DescribeFinishMode(EBTParallelMode::Type Mode);

#if WITH_EDITOR
	virtual bool CanAbortLowerPriority() const override;
	virtual bool CanAbortSelf() const override;
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR
};