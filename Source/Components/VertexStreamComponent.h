#pragma once
namespace Component
{
	class CVertexStreamComponent : public CComponent
	{
	public:
		explicit CVertexStreamComponent(const std::string& aPath);
		~CVertexStreamComponent();
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
		const std::string& GetMapPath() const;
	protected:
		std::string myPath;
	};
}

