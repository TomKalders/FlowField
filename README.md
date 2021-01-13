# Flow Field Implementation

## Framework
The framework used in this project us a school provided c++ framework (Elite Framework).
It contains math and SDL libraries. They also provided basic agents, graphs and navigation colliders.

## Purpose
The purpose of the flow field is to emulate seemingly intelligent movement among a large group of agents.
The idea is that instead of calculating a path for every single agent in a virtual world, you calculate one (or more) flow field(s)
from which the agents can read a direction from to know what way to go.
This optimizes performance for path finding with a large group of agents.

## Implementation
I first started by creating a new node types for the provided graphs.
  * FlowfieldNode: This holds the cost and direction.
  * IntegrationfieldNode: This holds the integration cost.

I made 2 graphs: the flow field (with cost field) and the integration field.

### Cost field
The first thing to create is the cost field. The lower the cost of a node, the easier the terrain is to traverse.
You can set the cost of a node to be very high to act as impassable terrain. I used a value of 100000 as the cost of impassable terrain.
In this implementation I only have a cost of 0 (easy terrain) or a cost of 100000 (impassable terrain).

### Integration field
The integration field is where most of the work happens. It is calculated using a pathfinding algorithm.
In this implementation I used a Breadth First Search (BFS) algorithm. 
Before we start the algorithm we first need a **Goal Node**. When we have a goal node we can start the BFS.

The steps the algorithm takes are:
  * Create a closed and open list.
  * Add the goal node to the open list.
  * Loop over the openlist while it's not empty.
     * Get the **current node** out of the openlist.
     * Loop over the neighbours of the current node.
        * Check if the neighbour node's cost is not higher or equal to impassable terrain (100000).
            * Check if the neighbour node is not in the closed list.
                * Calculate the intigration cost (current node's integration cost + cost of the connection + neighbour's cost field cost)
                     * If the neighbour's integration cost > calculated intigration cost
                        * Set the neighbour's intigration cost to the calculated intigration cost
                     * Check if the neighbour node is not in the openlist
                        * Add the neighbour node to the openlist
     * Add the currentnode to the closed list.
                        
                        
                        
