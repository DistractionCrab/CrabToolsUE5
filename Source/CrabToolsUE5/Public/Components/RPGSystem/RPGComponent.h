#pragma once

#include <limits>
#include "Templates/SharedPointer.h"
#include "Utils/Enums.h"
#include "RPGComponent.generated.h"


UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class UStatus: public UObject
{
	GENERATED_BODY()

	URPGComponent* Owner;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Apply();
	void Apply_Internal(URPGComponent* Comp);
	virtual void Apply_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Remove();
	void Remove_Internal();
	virtual void Remove_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Status", meta = (HideSelfPin, DefaultToSelf))
	URPGComponent* GetOwner() const { return this->Owner; }
};


#pragma region Integer Attributes


UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UIntOperator: public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	int Operate(int Value);
	virtual int Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {  }


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntAttributeObserver, int, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIntAttributeCallback, int, Value);

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FIntAttribute 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RPG|Attributes", meta=(AllowPrivateAccess=true))
	int BaseValue = 0;
	int CompValue;

	URPGComponent* Owner;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<UIntOperator*> Operators;

	TArray<FIntResource*> Dependencies;

public:
	FIntAttributeObserver IntChangedEvent;

	int GetValue() const;
	void Operate(UIntOperator* Op);
	void UnOperate(UIntOperator* Op);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();
	void AddDependency(FIntResource* Dep);
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FIntResource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	int Value = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetIntAttributeNames"))
	FName MinAttribute;
	FIntAttribute* MinAttributeRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions="GetIntAttributeNames"))
	FName MaxAttribute;
	FIntAttribute* MaxAttributeRef;

	URPGComponent* Owner;


public:
	FIntAttributeObserver IntChangedEvent;

	int GetValue() const { return this->Value; }
	void SetValue(int UValue);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();

	int GetMax() const;
	int GetMin() const;
	float GetPercent() const;
};

#pragma endregion

#pragma region Float Attributes & Resources


UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UFloatOperator : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	float Operate(float Value);
	virtual float Operate_Implementation(float Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {  }


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatAttributeObserver, float, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFloatAttributeCallback, float, Value);

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FFloatAttribute
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float BaseValue = 0;
	float CompValue;

	URPGComponent* Owner;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<UFloatOperator*> Operators;

	TArray<FFloatResource*> Dependencies;

public:
	FFloatAttributeObserver FloatChangedEvent;

	float GetValue() const;
	void Operate(UFloatOperator* Op);
	void UnOperate(UFloatOperator* Op);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();
	void AddDependency(FFloatResource* Dep);
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FFloatResource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float Value = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetFloatAttributeNames"))
	FName MinAttribute;
	FFloatAttribute* MinAttributeRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetFloatAttributeNames"))
	FName MaxAttribute;
	FFloatAttribute* MaxAttributeRef;

	URPGComponent* Owner;


public:
	FFloatAttributeObserver FloatChangedEvent;

	float GetValue() const { return this->Value; }
	void SetValue(float UValue);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();

	float GetMax() const;
	float GetMin() const;
	float GetPercent() const;
};

#pragma endregion

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API URPGComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Instanced, Category="RPG|Status")
	TArray<UStatus*> Statuses;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG", meta=(AllowPrivateAccess=true))
	FIntAttribute ZeroInt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG", meta = (AllowPrivateAccess = true))
	FFloatAttribute ZeroFloat;

public:
	URPGComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	virtual void PostLoad() override;
	void Validate();

public:
	UFUNCTION()
	int IntPassThrough(int Value) { return Value; }

	UFUNCTION()
	TArray<FString> GetIntAttributeNames() const;

	UFUNCTION()
	TArray<FString> GetFloatAttributeNames() const;	

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void ApplyStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status", meta=(ExpandEnumAsExecs="Result", DeterminesOutputType="SClass"))
	UStatus* GetStatus(TSubclassOf<UStatus> SClass, ESearchResult& Result);

	
};