#pragma once
#include "Component.h"
#include "ItemTypes.h"

class CItemFactory;

class CItemHolderComponent : public CComponent
{
public:
	CItemHolderComponent(CItemFactory& aItemFactory);
	~CItemHolderComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

private:
	eItemTypes myItem;

	CItemFactory& myItemFactory;
};

