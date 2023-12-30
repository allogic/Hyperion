#pragma once

#include <Interface/Node.h>

namespace interface
{
	class GridPanel : public Node
	{
	public:

		inline auto GetRows() const { return mRows; }
		inline auto GetColumns() const { return mColumns; }

	public:

		inline void SetRows(U32 Value) { mRows = Value; }
		inline void SetColumns(U32 Value) { mColumns = Value; }

	public:

		GridPanel(Node* Parent);
		virtual ~GridPanel();

	public:

		virtual void Update() override;
		virtual void Render() override;

	private:

		U32 mRows = 0;
		U32 mColumns = 0;
	};
}