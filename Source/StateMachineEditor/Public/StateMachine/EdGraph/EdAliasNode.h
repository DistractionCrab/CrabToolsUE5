#pragma once

#include "CoreMinimal.h"
#include "StateMachine/IStateMachineLike.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdAliasNode.generated.h"


UCLASS(MinimalAPI)
class UEdAliasNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	UPROPERTY()
	FName AliasLabel;

	UPROPERTY(EditDefaultsOnly, Category="Alias")
	TSet<FName> AliasedStates;

	/* 
	 * Whether or not the alias state set is referring to what states to alias, or what not to alias.
	 * If false, then this alis represents those states in the alias set. If true, represents any
	 * state not in the alias set.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Alias")
	bool bComplement = false;

public:

	UEdAliasNode();
	virtual ~UEdAliasNode();
	
	FName SetStateName(FName NewName) override;
	void Delete();
	void RenameNode(FName Name);
	bool Matches(UEdStateNode* Node) const;

	virtual FName GetStateName() const override;
	/* Returns the name which should appear on graph nodes. */
	virtual FName GetNodeName() const override { return this->AliasLabel; }
	virtual bool HasEvent(FName EName) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

};