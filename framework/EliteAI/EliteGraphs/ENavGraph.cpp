#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	std::vector<const Triangle*> triangles{};
	const std::vector<Line*> lines{ m_pNavMeshPolygon->GetLines() };
	for (Line* line : lines)
	{
		const std::vector<const Triangle*> tempTriangles{m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index)};
		for (size_t i{}; i < tempTriangles.size(); i++)
		{
			triangles.push_back(tempTriangles[i]);
		}
		if (tempTriangles.size() > 1)
		{
			Elite::Vector2 midPoint{(line->p1.x + line->p2.x) / 2.f, (line->p1.y + line->p2.y) / 2.f };
			AddNode(new NavGraphNode{ int(m_Nodes.size()), line->index, midPoint});
		}
	}
	
	//2. Create connections now that every node is created
	for (const Triangle* triangle : triangles)
	{
		std::vector<NavGraphNode*> validNodes{};
		for (unsigned int lineIndex : triangle->metaData.IndexLines)
		{
			size_t counter{};
			while (counter < m_Nodes.size())
			{
				if (lineIndex == m_Nodes[counter]->GetLineIndex())
				{
					validNodes.push_back(m_Nodes[counter]);
				}
				counter++;
			}
		}
		if (validNodes.size() == 2)
		{
			if (IsUniqueConnection(validNodes[0]->GetIndex(), validNodes[1]->GetIndex()))
				AddConnection(new GraphConnection2D{ validNodes[0]->GetIndex(), validNodes[1]->GetIndex()});
		}
		else if (validNodes.size() == 3)
		{
			if (IsUniqueConnection(validNodes[0]->GetIndex(), validNodes[1]->GetIndex()))
			{
			AddConnection(new GraphConnection2D{ validNodes[0]->GetIndex(), validNodes[1]->GetIndex() });
			AddConnection(new GraphConnection2D{ validNodes[1]->GetIndex(), validNodes[2]->GetIndex() });
			AddConnection(new GraphConnection2D{ validNodes[2]->GetIndex(), validNodes[0]->GetIndex() });
			}
		}	
	}
	
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();
}

