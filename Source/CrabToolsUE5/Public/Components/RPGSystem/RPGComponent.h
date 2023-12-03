#pragma once

#include <limits>
#include "Templates/SharedPointer.h"
#include "Utils/Enums.h"
#include "RPGComponent.generated.h"




UINTERFACE(MinimalAPI, Blueprintable)
class UStatusInterface : public UInterface
{
	GENERATED_BODY()
};

class IStatusInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Tick(float DeltaTime);
	void Tick_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Apply(URPGComponent* Comp);
	void Apply_Implementation(URPGComponent* Comp) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Remove(URPGComponent* Comp);
	void Remove_Implementation(URPGComponent* Comp) {}
};

#pragma region Integer Attributes

UINTERFACE(MinimalAPI, Blueprintable)
class UIntOperatorInterface : public UInterface
{
	GENERATED_BODY()
};

class IIntOperatorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Attributes")
	int Operate(int Value);
	int Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Attributes")
	int Priority();
	int Priority_Implementation() { return 0; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIntAttributeObserver, int, Value, int, Min, int, Max);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FIntAttributeCallback, int, Value, int, Min, int, Max);

USTRUCT(BlueprintType)
struct FIntAttribute
{
	GENERATED_USTRUCT_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta=(AllowPrivateAccess=true))
	int BaseValue;
	// Computed value after applying operators.
	int CompValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta=(GetOptions="GetIntAttributeNames", AllowPrivateAccess = true))
	FName MaxAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta=(GetOptions="GetIntAttributeNames", AllowPrivateAccess = true))
	FName MinAttribute;

	TArray<TWeakObjectPtr<UObject>> Operators;

	FIntAttribute* MaxRef;
	FIntAttribute* MinRef;

public:
	FIntAttributeObserver OnChangedEvent;

public:
	int GetValue();
	void SetBase(int Value);
	int GetBase();
	int GetMax();
	int GetMin();
	void Initialize(URPGComponent* Comp);
	void Operate(UObject* Operator);
	void Refresh();

};

#pragma endregion

#pragma region Float Attributes


UINTERFACE(MinimalAPI, Blueprintable)
class UFloatOperatorInterface : public UInterface
{
	GENERATED_BODY()
};

class IFloatOperatorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Attributes")
	float Operate(float Value);
	float Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Attributes")
	int Priority();
	int Priority_Implementation() { return 0; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFloatAttributeObserver, int, Value, int, Min, int, Max);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FFloatAttributeCallback, int, Value, int, Min, int, Max);

USTRUCT(BlueprintType)
struct FFloatAttribute
{
	GENERATED_USTRUCT_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float BaseValue;
	// Computed value after applying operators.
	float CompValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (GetOptions = "GetFloatAttributeNames", AllowPrivateAccess = true))
	FName MaxAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (GetOptions = "GetFloatAttributeNames", AllowPrivateAccess = true))
	FName MinAttribute;

	TArray<TWeakObjectPtr<UObject>> Operators;

	FFloatAttribute* MaxRef;
	FFloatAttribute* MinRef;

public:
	FFloatAttributeObserver OnChangedEvent;

public:
	float GetValue();
	void SetBase(float Value);
	float GetBase();
	float GetMax();
	float GetMin();
	void Initialize(URPGComponent* Comp);
	void Operate(UObject* Operator);
	void Refresh();

};

#pragma endregion


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API URPGComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FIntAttribute> IntAttributes;

	UPROPERTY(EditAnywhere, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FFloatAttribute> FloatAttributes;

	TArray<TWeakObjectPtr<UObject>> Statuses;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG")
	int GetIntAttributeValue(FName Key);
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void SetIntAttributeValue(FName Key, int Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG")
	float GetFloatAttributeValue(FName Key);
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void SetFloatAttributeValue(FName Key, float Value);

	UFUNCTION(BlueprintCallable, Category="RPG")
	void ObserveIntAttribute(FName Key, const FIntAttributeCallback& Callback);
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ObserveFloatAttribute(FName Key, const FFloatAttributeCallback& Callback);

	UFUNCTION()
	TArray<FString> GetIntAttributeNames() const;

	UFUNCTION()
	TArray<FString> GetFloatAttributeNames() const;

	FIntAttribute* GetIntAttribute(FName Key) const;
	FFloatAttribute* GetFloatAttribute(FName Key) const;
};