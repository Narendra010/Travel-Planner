#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <string>
#include <map>
#include <set>

using namespace std;

const int INF = numeric_limits<int>::max();

// Define a structure to hold city information
struct CityInfo {
    int distance;
    int fare;
    int time;
};

class Graph {
public:
    map<string, int> nodeIndex; // Maps node names to indices
    vector<vector<CityInfo>> adjMatrix; // Adjacency matrix for edge weights and fare

    Graph() {}

    void addEdge(string from, string to, CityInfo info) {
        int i = nodeIndex[from];
        int j = nodeIndex[to];
        adjMatrix[i][j] = adjMatrix[j][i] = info;
    }

    void dijkstra(string startNode, string endNode, int basis) {
        int startIdx = nodeIndex[startNode];
        int endIdx = nodeIndex[endNode];
        int numNodes = adjMatrix.size();

        vector<int> distance(numNodes, INF);
        vector<int> parent(numNodes, -1); // To keep track of parent nodes for path
        set<int> visited;
        distance[startIdx] = 0;

        for (int i = 0; i < numNodes; i++) {
            int u = -1;
            for (int j = 0; j < numNodes; j++) {
                if (visited.find(j) == visited.end() && (u == -1 || distance[j] < distance[u])) {
                    u = j;
                }
            }

            if (distance[u] == INF) {
                break;
            }

            visited.insert(u);

            for (int v = 0; v < numNodes; v++) {
                if (adjMatrix[u][v].distance != INF) {
                    int newDist;
                    switch (basis) {
                        case 1: // Shortest distance
                            newDist = distance[u] + adjMatrix[u][v].distance;
                            break;
                        case 2: // Smallest fare
                            newDist = distance[u] + adjMatrix[u][v].fare;
                            break;
                        case 3: // Shortest time
                            newDist = distance[u] + adjMatrix[u][v].time;
                            break;
                        default:
                            newDist = INF;
                    }
                    if (newDist < distance[v]) {
                        distance[v] = newDist;
                        parent[v] = u;
                    }
                }
            }
        }

        // Print shortest distance/fare/time and path
        switch (basis) {
            case 1: // Shortest distance
                cout << "Shortest distance from " << startNode << " to " << endNode << ": " << distance[endIdx] << endl;
                break;
            case 2: // Smallest fare
                cout << "Smallest fare from " << startNode << " to " << endNode << ": " << distance[endIdx] << endl;
                break;
            case 3: // Shortest time
                cout << "Shortest time from " << startNode << " to " << endNode << ": " << distance[endIdx] << endl;
                break;
            default:
                cerr << "Invalid basis." << endl;
                return;
        }

        // Print path
        cout << "Path you should follow to reach you destination : ";
        int curr = endIdx;
        vector<string> path;
        while (curr != -1) {
            path.push_back(getNodeName(curr));
            curr = parent[curr];
        }
        for (int i = path.size() - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) {
                cout << " -> ";
            }
        }
        cout << endl;
    }

    void addNode(string nodeName) {
        int numNodes = adjMatrix.size();
        nodeIndex[nodeName] = numNodes;
        for (int i = 0; i < numNodes; i++) {
            adjMatrix[i].push_back(CityInfo{INF, INF, INF});
        }
        adjMatrix.push_back(vector<CityInfo>(numNodes + 1, CityInfo{INF, INF, INF}));
    }

    string getNodeName(int index) {
        for (const auto& entry : nodeIndex) {
            if (entry.second == index) {
                return entry.first;
            }
        }
        return "";
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file for writing: " << filename << endl;
            string newFilename;
            cout << "Enter a new filename to save data: ";
            cin >> newFilename;
            saveToFile(newFilename); // Recursively call saveToFile with a new filename
            return;
        }

        int numNodes = adjMatrix.size();
        file << numNodes << endl;

        // Write node names
        for (const auto& entry : nodeIndex) {
            file << entry.first << endl;
        }

        // Write edge information (distance, fare, time)
        for (int i = 0; i < numNodes; i++) {
            for (int j = i + 1; j < numNodes; j++) {
                file << adjMatrix[i][j].distance << " ";
                file << adjMatrix[i][j].fare << " ";
                file << adjMatrix[i][j].time << endl;
            }
        }

        file.close();
        cout << "Data saved to file: " << filename << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file for reading: " << filename << endl;
            return;
        }

        int numNodes;
        file >> numNodes;
        file.ignore();  // Consume the newline character

        // Read node names
        for (int i = 0; i < numNodes; i++) {
            string nodeName;
            getline(file, nodeName);
            addNode(nodeName);
        }

        // Read edge information (distance, fare, time)
        for (int i = 0; i < numNodes; i++) {
            for (int j = i + 1; j < numNodes; j++) {
                CityInfo info;
                file >> info.distance >> info.fare >> info.time;
                adjMatrix[i][j] = adjMatrix[j][i] = info;
            }
        }

        file.close();
        cout << "Data loaded from file: " << filename << endl;
    }
};

int main() {
    // Create separate Graph objects for different zones
    Graph zoneA;
    Graph zoneB;
    Graph zoneC;
    Graph zoneD;

    cout<<"\n <--------------------------------------------Welcome to your daily travel planner----------------------------------------------->\n";
    cout << "Choose a zone from (A, B, C, D) in which you're traveling today : ";
    char chosenZone;
    cin >> chosenZone;
    chosenZone = toupper(chosenZone);

    Graph* chosenGraph;

    // Assign the chosen Graph based on the selected zone
    switch (chosenZone) {
        case 'A':
            chosenGraph = &zoneA;
            break;
        case 'B':
            chosenGraph = &zoneB;
            break;
        case 'C':
            chosenGraph = &zoneC;
            break;
        case 'D':
            chosenGraph = &zoneD;
            break;
        case 'X': exit(0);
        default:
            cerr << "Invalid zone." << endl;
            return 1;
    }

    cout << "1. Load data from file" << endl;
    cout << "2. Enter data manually" << endl;
    int choice;
    cout << "Choose an option (1/2): ";
    cin >> choice;
    cin.ignore();  // Consume the newline character

    if (choice == 1) {
        // Load data from file
        string filename;
        cout << "Ok. Then enter the filename to load data from: ";
        cin >> filename;
        cin.ignore(); // Consume the newline character
        chosenGraph->loadFromFile(filename);
    } else if (choice == 2) {
        // Enter data manually
        int numNodes;
        cout << "Enter the number of locations: ";
        cin >> numNodes;
        cin.ignore();  // Consume the newline character
        for (int i = 0; i < numNodes; i++) {
            string nodeName;
            cout << "Enter the name of location " << i + 1 << ": ";
            getline(cin, nodeName);
            chosenGraph->addNode(nodeName);
        }

        // Enter edge information (distance, fare, time)
        for (int i = 0; i < numNodes; i++) {
            for (int j = i + 1; j < numNodes; j++) {
                CityInfo info;
                cout << "Enter the distance from " << chosenGraph->getNodeName(i) << " to " << chosenGraph->getNodeName(j) << ": ";
                cin >> info.distance;
                cout << "Enter the fare from " << chosenGraph->getNodeName(i) << " to " << chosenGraph->getNodeName(j) << ": ";
                cin >> info.fare;
                cout << "Enter the time from " << chosenGraph->getNodeName(i) << " to " << chosenGraph->getNodeName(j) << ": ";
                cin >> info.time;
                chosenGraph->addEdge(chosenGraph->getNodeName(i), chosenGraph->getNodeName(j), info);
            }
        }

        string filename;
        cout << "Enter the filename to save data: ";
        cin >> filename;
        chosenGraph->saveToFile(filename);
    } else {
        cerr << "Invalid choice." << endl;
        return 1;
    }

    // Find shortest path
    string startNode, endNode;
    int basis;
    cin.ignore();  // Consume the newline character
    cout << "Enter the start point name : ";
    getline(cin, startNode);
    cout << "Enter the destination : ";
    getline(cin, endNode);
    cout << "Choose basis for shortest path:" << endl;
    cout << "1) Shortest distance" << endl;
    cout << "2) Smallest fare" << endl;
    cout << "3) Shortest time" << endl;
    cout << "Enter your choice (1/2/3): ";
    cin >> basis;

    if (chosenGraph->nodeIndex.find(startNode) != chosenGraph->nodeIndex.end() &&
        chosenGraph->nodeIndex.find(endNode) != chosenGraph->nodeIndex.end()) {
        chosenGraph->dijkstra(startNode, endNode, basis);
    } else {
        cerr << "Start location or destination not found." << endl;
    }

    return 0;
}
