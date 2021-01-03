/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
// Authors: Yosha Vandaele
/*=============================================================================*/
// EGraph2D.h: Derived graph type, intended to represent a 2D space or be used as a graph that needs a 2D visualisation
/*=============================================================================*/
#pragma once

#include "EIGraph.h"
#include "EGraphConnectionTypes.h"
#include "EGraphNodeTypes.h"
#include <iomanip>

namespace Elite
{
	template<class T_NodeType, class T_ConnectionType>
	class Graph2D : public IGraph<T_NodeType, T_ConnectionType>
	{
	public:
		Graph2D(bool isDirectional);
		Graph2D(const Graph2D& other);
		virtual shared_ptr<IGraph<T_NodeType, T_ConnectionType>> Clone() const override;

		void Update();

		using IGraph::GetNodePos;
		virtual Vector2 GetNodePos(T_NodeType* pNode) const override { return pNode->GetPosition(); }
		virtual int GetNodeIdxAtWorldPos(const Elite::Vector2& pos) const override;

		void SetConnectionCostsToDistance();
		void SetNodesColor(const vector<GraphNode2D*>& nodes, const Color& color);

	private:
		// functions
		void OnLeftMouseButtonPressed(const MouseData& mouseData);
		void OnLeftMouseButtonReleased(const MouseData& mouseData);
		void OnRightMouseButtonPressed(const MouseData& mouseData);

		T_ConnectionType* GetConnectionAtPosition(const Vector2& pos) const;

		// variables
		int m_SelectedNodeIdx = -1;
		bool m_IsLeftMouseButtonDown = false;
		Vector2 m_MousePos;
		const float m_ConnectionSelectionOffset = 1.f;
	};

	template<class T_NodeType, class T_ConnectionType>
	Graph2D<T_NodeType, T_ConnectionType>::Graph2D(bool isDirectional)
		: IGraph(isDirectional)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Graph2D<T_NodeType, T_ConnectionType>::Graph2D(const Graph2D& other)
		: IGraph<T_NodeType, T_ConnectionType>(other)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline shared_ptr<IGraph<T_NodeType, T_ConnectionType>> Graph2D<T_NodeType, T_ConnectionType>::Clone() const
	{
		return shared_ptr<Graph2D>(new Graph2D(*this));
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::Update()
	{
		auto mouseLeftData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		auto mouseRightData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eRight);
		auto mouseMotionData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseMotion);

		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
			OnLeftMouseButtonReleased(mouseLeftData);
		if (INPUTMANAGER->IsMouseButtonDown(InputMouseButton::eLeft))
			OnLeftMouseButtonPressed(mouseLeftData);
		if (INPUTMANAGER->IsMouseButtonDown(InputMouseButton::eRight))
			OnRightMouseButtonPressed(mouseRightData);

		// Update pNode, edge and debug drawing positions
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseMotionData.X, (float)mouseMotionData.Y });
		auto isMouseMoving = INPUTMANAGER->IsMouseMoving();

		if (isMouseMoving)
			m_MousePos = mousePos;

		if (m_SelectedNodeIdx != invalid_node_index)
		{
			auto nodePos = GetNode(m_SelectedNodeIdx)->GetPosition();
			if (m_IsLeftMouseButtonDown)
			{
				DEBUGRENDERER2D->DrawCircle(nodePos, GetNodeRadius(GetNode(m_SelectedNodeIdx)), { 1,1,1 }, -1);
				m_Nodes[m_SelectedNodeIdx]->SetPosition(m_MousePos);
			}

			if (!m_IsLeftMouseButtonDown)
			{
				DEBUGRENDERER2D->DrawSegment(nodePos, m_MousePos, DEFAULT_CONNECTION_COLOR, -1);
			}
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	inline int Graph2D<T_NodeType, T_ConnectionType>::GetNodeIdxAtWorldPos(const Elite::Vector2& pos) const
	{
		float posErrorMargin = 1.5f;
		auto foundIt = find_if(m_Nodes.begin(), m_Nodes.end(),
			[pos, posErrorMargin, this](T_NodeType* pNode)
		{ return (pNode->GetPosition() - pos).MagnitudeSquared() < pow(posErrorMargin * GetNodeRadius(pNode), 2); });

		if (foundIt != m_Nodes.end())
			return (*foundIt)->GetIndex();
		else
			return invalid_node_index;
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::SetConnectionCostsToDistance()
	{
		for (auto& connectionList : m_Connections)
		{
			for (auto& connection : connectionList)
			{
				auto posFrom = GetNodePos(connection->GetFrom());
				auto posTo = GetNodePos(connection->GetTo());
				connection->SetCost(abs(Distance(posFrom, posTo)));
			}
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::SetNodesColor(const vector<GraphNode2D*>& nodes, const Color& color)
	{
		for (auto& n : nodes)
		{
			if (n)
				m_Nodes[n->GetIndex()]->SetColor(color);
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::OnLeftMouseButtonPressed(const MouseData& mouseData)
	{
		m_IsLeftMouseButtonDown = true;
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
		int clickedIdx = GetNodeIdxAtWorldPos(mousePos);

		if (m_SelectedNodeIdx != invalid_node_index)
		{
			if (clickedIdx != invalid_node_index && m_SelectedNodeIdx != clickedIdx)
			{
				if(IsUniqueConnection(m_SelectedNodeIdx, clickedIdx))
					AddConnection(new GraphConnection2D(m_SelectedNodeIdx, clickedIdx));
			}

			m_SelectedNodeIdx = invalid_node_index;
		}
		else if (clickedIdx != invalid_node_index)
		{
			m_SelectedNodeIdx = clickedIdx;
		}
		else
		{
			AddNode(new T_NodeType(GetNextFreeNodeIndex(), mousePos));
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::OnLeftMouseButtonReleased(const MouseData& mouseData)
	{
		m_IsLeftMouseButtonDown = false;
	}

	template<class T_NodeType, class T_ConnectionType>
	void Graph2D<T_NodeType, T_ConnectionType>::OnRightMouseButtonPressed(const MouseData& mouseData)
	{
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
		int clickedIdx = GetNodeIdxAtWorldPos(mousePos);

		if (clickedIdx != invalid_node_index)
			RemoveNode(clickedIdx);

		auto clickedConnection = GetConnectionAtPosition(mousePos);
		if (clickedConnection)
			RemoveConnection(clickedConnection->GetFrom(), clickedConnection->GetTo());
	}

	template<class T_NodeType, class T_ConnectionType>
	T_ConnectionType* Graph2D<T_NodeType, T_ConnectionType>::GetConnectionAtPosition(const Vector2& pos) const
	{
		T_ConnectionType* result = nullptr;
		for (auto connectionList : m_Connections)
		{
			for (auto connection : connectionList)
			{
				auto segmentStart = GetNodePos(connection->GetTo());
				auto segmentEnd =	GetNodePos(connection->GetFrom());

				auto projectedPoint = ProjectOnLineSegment(segmentStart, segmentEnd, pos);

				if (DistanceSquared(projectedPoint, pos) < m_ConnectionSelectionOffset)
					result = connection;
			}
		}

		return result;
	}
}
