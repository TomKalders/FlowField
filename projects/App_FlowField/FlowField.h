//#pragma once
//class  
//{
//};

#ifndef FLOWFIELD_APPLICATION_H
#define FLOWFIELD_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphEditor.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"


//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class FlowField final : public IApp
{
public:
	//Constructor & Destructor
	FlowField() = default;
	virtual ~FlowField();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	const bool ALLOW_DIAGONAL_MOVEMENT = true;
	Elite::Vector2 m_StartPosition = Elite::ZeroVector2;
	Elite::Vector2 m_TargetPosition = Elite::ZeroVector2;
	int m_GoalNodeIdx{ invalid_node_index };


	//Grid datamembers
	static const int COLUMNS = 10;
	static const int ROWS = 10;
	unsigned int m_SizeCell = 15;
	Elite::GridGraph<Elite::FlowFieldNode, Elite::GraphConnection>* m_pGridGraph;
	Elite::GridGraph<Elite::IntegrationFieldNode, Elite::GraphConnection>* m_pIntegrationField;


	//Editor and Visualisation
	Elite::EGraphEditor m_GraphEditor{};
	Elite::EGraphRenderer m_GraphRenderer{};

	//Debug rendering information
	bool m_bDrawGrid = true;
	bool m_bDrawNodeNumbers = false;
	bool m_bDrawConnections = false;
	bool m_bDrawConnectionsCosts = false;
	bool m_StartSelected = true;
	int m_SelectedHeuristic = 4;
	Elite::Heuristic m_pHeuristicFunction = Elite::HeuristicFunctions::Chebyshev;

	bool m_bDrawIntegration = false;
	bool m_bDrawIntegrationNodeNumbers = false;
	bool m_bDrawIntegrationConnections = false;
	bool m_bDrawIntegrationConnectionsCosts = false;

	//Functions
	void MakeGridGraph();
	void CalculateFlowField();
	void CreateCostField();
	void CalculateIntegrationField();
	void CalculateVectors();
	void ResetField();
	void UpdateImGui();

	const std::vector<Elite::Vector2> m_Directions{
		Elite::Vector2{-1,  1},
		Elite::Vector2{ 0,  1},
		Elite::Vector2{ 1,  1},
		Elite::Vector2{ 1,  0},
		Elite::Vector2{ 1, -1},
		Elite::Vector2{ 0, -1},
		Elite::Vector2{-1, -1},
		Elite::Vector2{-1,  0},
		Elite::Vector2{ 0,  0}
	};

	//C++ make the class non-copyable
	FlowField(const FlowField&) = delete;
	FlowField& operator=(const FlowField&) = delete;
};
#endif