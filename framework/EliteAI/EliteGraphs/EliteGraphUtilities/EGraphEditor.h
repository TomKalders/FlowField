#pragma once

#include "framework\EliteAI\EliteGraphs\EGraphNodeTypes.h"
#include "framework\EliteAI\EliteGraphs\EGraphConnectionTypes.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EGraph2D.h"

namespace Elite 
{
	class EGraphEditor final
	{
	public:
		EGraphEditor() = default;
		~EGraphEditor() = default;
		 
		bool UpdateGraph(GridGraph<GridTerrainNode, GraphConnection>* pGraph);

		template <class T_NodeType, class T_ConnectionType>
		bool UpdateGraph(GridGraph<T_NodeType, T_ConnectionType>* pGraph);
	private:
		int m_SelectedTerrainType = (int)TerrainType::Ground;
		
	};
	
	template<class T_NodeType, class T_ConnectionType>
	inline bool EGraphEditor::UpdateGraph(GridGraph<T_NodeType, T_ConnectionType>* pGraph)
	{
		//Check if clicked on grid

		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
		{
			auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
			Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
			int idx = pGraph->GetNodeIdxAtWorldPos(mousePos);

			if (idx != invalid_node_index)
			{
				pGraph->AddConnectionsToAdjacentCells(idx);
				return true;
			}
		}
		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
		{
			auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eRight);
			Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
			int idx = pGraph->GetNodeIdxAtWorldPos(mousePos);

			if (idx != invalid_node_index)
			{
				pGraph->RemoveConnectionsToAdjacentNodes(idx);
				return true;
			}
		}
		return false;
	}
}


