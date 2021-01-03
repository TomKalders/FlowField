#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		//Here we will calculate the path using A*
		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord;

		openList.push_back(NodeRecord{ pStartNode, nullptr, 0.f, GetHeuristicCost(pStartNode, pGoalNode) });

		while (!openList.empty())
		{
			float tempCost{FLT_MAX};
			for (int i{}; i < int(openList.size()); i++)
			{
				if (openList[i].estimatedTotalCost <= tempCost)
				{
					currentRecord = openList[i];
					tempCost = openList[i].estimatedTotalCost;
				}
			}
			if (currentRecord.pNode == pGoalNode)
			{
				break;
			}
			else
			{
				list<T_ConnectionType*> neighbours = m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex());
				float gCost{};
				for (T_ConnectionType* connection : neighbours)
				{
					gCost = currentRecord.costSoFar + connection->GetCost();
					T_NodeType* neighbourNode{ m_pGraph->GetNode(connection->GetTo()) };
					bool check{ true };

					for (int i{}; i < int(closedList.size()); i++)
					{
						if (closedList[i].pNode == neighbourNode)
						{
							if (gCost <= closedList[i].costSoFar)
							{
								closedList.erase(closedList.begin() + i);
							}
							else
							{
								check = false;
							}
						}
					}
					for (int i{}; i < int(openList.size()); i++)
					{
						if (openList[i].pNode == neighbourNode)
						{
							if (gCost <= openList[i].costSoFar)
							{
								openList.erase(openList.begin() + i);
							}
							else
							{
								check = false;
							}
						}
					}
					if (check)
						openList.push_back(NodeRecord{ neighbourNode, connection, gCost, GetHeuristicCost(neighbourNode, pGoalNode) + gCost });
				}

				
				for (int i{}; i < int(openList.size()); i++)
				{
					if (openList[i] == currentRecord)
					{
						openList.erase(openList.begin() + i);
						closedList.push_back(currentRecord);
					}
				}
			}
		}

		path.push_back(currentRecord.pNode);
		while (currentRecord.pNode != pStartNode)
		{
			T_NodeType* previousNode = m_pGraph->GetNode(currentRecord.pConnection->GetFrom());
			for (int i{}; i < int(closedList.size()); i++)
			{
				if (closedList[i].pNode == previousNode)
				{
					path.push_back(closedList[i].pNode);
					currentRecord = closedList[i];
				}
			}
		}
		std::reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}