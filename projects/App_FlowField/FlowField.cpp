//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "FlowField.h"
#include "projects/Shared/BaseAgent.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAstar.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EBFS.h"

using namespace Elite;

//Destructor
FlowField::~FlowField()
{
	SAFE_DELETE(m_pGridGraph);
	SAFE_DELETE(m_pIntegrationField);
}

//Functions
void FlowField::Start()
{
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(39.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(73.0f, 35.0f));

	//Create Graph
	MakeGridGraph();
	//CreateCostField();
	CalculateFlowField();
}

void FlowField::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	//INPUT
	//bool const middleMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle);
	//if (middleMousePressed)
	//{
	//	MouseData mouseData = { INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle) };
	//	Elite::Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
	//}

	bool const leftMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft);

	//IMGUI
	UpdateImGui();
}

void FlowField::Render(float deltaTime) const
{
	UNREFERENCED_PARAMETER(deltaTime);
	//Render grid
	m_GraphRenderer.RenderGraph(
		m_pGridGraph,
		m_bDrawGrid,
		m_bDrawNodeNumbers,
		m_bDrawConnections,
		m_bDrawConnectionsCosts
	);

	auto nodes = m_pGridGraph->GetAllNodes();

	for (int i{}; i < ROWS; i++)
	{
		for (int j{}; j < COLUMNS; j++)
		{
			Vector2 position{ j * m_SizeCell + (m_SizeCell / 2.f), i * m_SizeCell + (m_SizeCell / 2.f) };
			auto node = m_pGridGraph->GetNodeAtWorldPos(position);

			DEBUGRENDERER2D->DrawPoint(position, 2.f, { 1, 0, 0 }, 0);
			DEBUGRENDERER2D->DrawDirection(position, node->GetDirection(), 3.f, { 0, 1, 0 });
		}
	}
}

void FlowField::MakeGridGraph()
{
	m_pGridGraph = new GridGraph<FlowFieldNode, GraphConnection>(COLUMNS, ROWS, m_SizeCell, false, true, 1.f, 1.5f);
	m_pIntegrationField = new GridGraph<IntegrationFieldNode, GraphConnection>(COLUMNS, ROWS, m_SizeCell, false, true, 1.f, 1.5f);

	m_GoalNodeIdx = (COLUMNS * ROWS) - 1;
	//m_GoalNodeIdx = 45;
}

void FlowField::CalculateFlowField()
{
	CalculateIntegrationField();
	CalculateVectors();
}

void FlowField::CreateCostField()
{
	//73, 74, 75, 76 avoid
	//43, 44, 45, 46 impassable

	for (int i{ 73 }; i < 77; i++)
	{
		auto node = m_pGridGraph->GetNode(i);
		node->SetCost(1 + (randomInt(244) + 1));
	}

	for (int i{ 73 }; i < 77; i++)
	{
		auto node = m_pGridGraph->GetNode(i);
		node->SetCost(255);
	}
}

void FlowField::CalculateIntegrationField()
{
	//Set total cost in all cells to 65535
	ResetField();
	std::list<int> openList{};

	IntegrationFieldNode* goalNode = m_pIntegrationField->GetNode(m_GoalNodeIdx);
	goalNode->SetIntegrationCost(0);
	openList.push_back(m_GoalNodeIdx);

	while (openList.size() > 0)
	{
		int currentIdx = openList.front();
		IntegrationFieldNode* currentNode = m_pIntegrationField->GetNode(currentIdx);
		openList.pop_front();

		auto neighbours = m_pGridGraph->GetConnections(currentIdx);
		int neighbourCount = neighbours.size();

		for (const auto& neighbour : neighbours)
		{
			FlowFieldNode* neighbourFlowNode = m_pGridGraph->GetNode(neighbour->GetTo());
			IntegrationFieldNode* neighbourNode = m_pIntegrationField->GetNode(neighbour->GetTo());
			
			int endNodeCost{currentNode->GetIntegrationCost() + neighbourFlowNode->GetCost()};

			if (endNodeCost > neighbourFlowNode->GetCost())
				endNodeCost -= 1;

			if (endNodeCost < neighbourNode->GetIntegrationCost())
			{
				if (std::find(openList.begin(), openList.end(), neighbourNode->GetIndex()) == openList.end())
				{
					openList.push_back(neighbourNode->GetIndex());
				}
			}

			neighbourNode->SetIntegrationCost(endNodeCost);
		}
	}
}

void FlowField::CalculateVectors()
{
	auto nodes = m_pGridGraph->GetAllNodes();

	for (const auto& node : nodes)
	{
		int idx = node->GetIndex();
		auto neighbours = m_pIntegrationField->GetConnections(idx);

		int lowestCostNeighbourIdx{};
		int lowestCost{ INT_MAX };
		for (const auto& neighbour : neighbours)
		{
			auto neighbourNode = m_pIntegrationField->GetNode(neighbour->GetTo());
			if (neighbourNode->GetIntegrationCost() < lowestCost)
			{
				lowestCost = neighbourNode->GetIntegrationCost();
				lowestCostNeighbourIdx = neighbourNode->GetIndex();
			}
		}

		auto target = m_pGridGraph->GetNode(lowestCostNeighbourIdx);
		Vector2 nodePosition{ m_pGridGraph->GetNodePos(node) };
		Vector2 neighbourPosition{ m_pGridGraph->GetNodePos(target) };
		
		Vector2 direction = neighbourPosition - nodePosition;
		direction = direction.GetNormalized();

		node->SetDirection(direction);
	}
}

void FlowField::ResetField()
{
	for (auto& node : m_pIntegrationField->GetAllNodes())
	{
		node->SetIntegrationCost(65535);
	}
}

void FlowField::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 115;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: target");
		ImGui::Text("RMB: start");
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("A* Pathfinding");
		ImGui::Spacing();

		ImGui::Text("Middle Mouse");
		ImGui::Text("controls");
		std::string buttonText{ "" };
		if (m_StartSelected)
			buttonText += "Start Node";
		else
			buttonText += "End Node";

		if (ImGui::Button(buttonText.c_str()))
		{
			m_StartSelected = !m_StartSelected;
		}

		ImGui::Checkbox("Grid", &m_bDrawGrid);
		ImGui::Checkbox("NodeNumbers", &m_bDrawNodeNumbers);
		ImGui::Checkbox("Connections", &m_bDrawConnections);
		ImGui::Checkbox("Connections Costs", &m_bDrawConnectionsCosts);
		//ImGui::Spacing();
		//ImGui::Spacing();
		//ImGui::Text("Integration Field");
		//ImGui::Checkbox("Grid", &m_bDrawIntegration);
		//ImGui::Checkbox("NodeNumbers", &m_bDrawIntegrationNodeNumbers);
		//ImGui::Checkbox("Connections", &m_bDrawIntegrationConnections);
		//ImGui::Checkbox("Connections Costs", &m_bDrawIntegrationConnectionsCosts);

		if (ImGui::Combo("", &m_SelectedHeuristic, "Manhattan\0Euclidean\0SqrtEuclidean\0Octile\0Chebyshev", 4))
		{
			switch (m_SelectedHeuristic)
			{
			case 0:
				m_pHeuristicFunction = HeuristicFunctions::Manhattan;
				break;
			case 1:
				m_pHeuristicFunction = HeuristicFunctions::Euclidean;
				break;
			case 2:
				m_pHeuristicFunction = HeuristicFunctions::SqrtEuclidean;
				break;
			case 3:
				m_pHeuristicFunction = HeuristicFunctions::Octile;
				break;
			case 4:
				m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
				break;
			default:
				m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
				break;
			}
		}
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}
